
/*-------------------------------------------------------------------------
 *
 * irodsReServer.c--The irods Rule Execution server
 *
 *
 *-------------------------------------------------------------------------
 */

#include "irodsReServer.hpp"
#include "reServerLib.hpp"
#include "rsApiHandler.hpp"
#include "rsIcatOpr.hpp"
#include <syslog.h>
#include "miscServerFunct.hpp"
#include "reconstants.hpp"
#include "irods_server_properties.hpp"
#include "readServerConfig.hpp"

// =-=-=-=-=-=-=-
// irods includes
#include "irods_get_full_path_for_config_file.hpp"


extern int msiAdmClearAppRuleStruct( ruleExecInfo_t *rei );

int usage( char *prog );

int
main( int argc, char **argv ) {
    int status;
    int c;
    rsComm_t rsComm;
    int runMode = SERVER;
    int flagval = 0;
    char *logDir = NULL;
    char *tmpStr;
    int logFd;
    char *ruleExecId = NULL;
    int jobType = 0;
    bool run_server_as_root = false;

    ProcessType = RE_SERVER_PT;

    irods::server_properties::getInstance().get_property<bool>( RUN_SERVER_AS_ROOT_KW, run_server_as_root );

#ifndef windows_platform
    if ( run_server_as_root ) {
        if ( initServiceUser() < 0 ) {
            exit( 1 );
        }
    }
#endif


#ifndef _WIN32
    signal( SIGINT, signalExit );
    signal( SIGHUP, signalExit );
    signal( SIGTERM, signalExit );
    signal( SIGUSR1, signalExit );
    signal( SIGPIPE, rsPipSigalHandler );
    /* XXXXX switched to SIG_DFL for embedded python. child process
     * went away. But probably have to call waitpid.
     * signal(SIGCHLD, SIG_IGN); */
    signal( SIGCHLD, SIG_DFL );
#endif

    /* Handle option to log sql commands */
    tmpStr = getenv( SP_LOG_SQL );
    if ( tmpStr != NULL ) {
#ifdef SYSLOG
        int j = atoi( tmpStr );
        rodsLogSqlReq( j );
#else
        rodsLogSqlReq( 1 );
#endif
    }

    /* Set the logging level */
    tmpStr = getenv( SP_LOG_LEVEL );
    if ( tmpStr != NULL ) {
        int i;
        i = atoi( tmpStr );
        rodsLogLevel( i );
    }
    else {
        rodsLogLevel( LOG_NOTICE ); /* default */
    }

#ifdef SYSLOG
    /* Open a connection to syslog */
    openlog( "rodsReServer", LOG_ODELAY | LOG_PID, LOG_DAEMON );
#endif

    while ( ( c = getopt( argc, argv, "sSvD:j:t:" ) ) != EOF ) {
        switch ( c ) {
        case 's':
            runMode = SINGLE_PASS;
            break;
        case 'S':
            runMode = STANDALONE_SERVER;
            break;
        case 'v':   /* verbose */
            flagval |= v_FLAG;
            break;
        case 'D':   /* user specified a log directory */
            logDir = strdup( optarg );
            break;
        case 'j':
            runMode = SINGLE_PASS;
            ruleExecId = strdup( optarg );
            break;
        case 't':
            jobType = atoi( optarg );
            break;
        default:
            usage( argv[0] );
            exit( 1 );
        }
    }

    status = initRsComm( &rsComm );

    if ( status < 0 ) {
        cleanupAndExit( status );
    }

    if ( ( logFd = logFileOpen( runMode, logDir, RULE_EXEC_LOGFILE ) ) < 0 ) {
        exit( 1 );
    }

    daemonize( runMode, logFd );

    status = initAgent( RULE_ENGINE_INIT_CACHE, &rsComm );
    if ( status < 0 ) {
        cleanupAndExit( status );
    }

    if ( ruleExecId != NULL ) {
        status = reServerSingleExec( &rsComm, ruleExecId, jobType );
        if ( status >= 0 ) {
            exit( 0 );
        }
        else {
            exit( 1 );
        }
    }
    else {
        reServerMain( &rsComm, logDir );
    }
    cleanupAndExit( status );

    exit( 0 );
}

int usage( char *prog ) {
    fprintf( stderr, "Usage: %s [-sSv] [j jobID] [-t jobType] [-D logDir] \n", prog );
    fprintf( stderr, "-s   Run like a client process - no fork\n" );
    fprintf( stderr, "-S   Run like a daemon server process - forked\n" );
    fprintf( stderr, "-j jobID   Run a single job\n" );
    fprintf( stderr, "-t jobType An integer for job type. \n" );
    return 0;
}

void
reServerMain( rsComm_t *rsComm, char* logDir ) {
    int status = 0;
    genQueryOut_t *genQueryOut = NULL;
    time_t endTime;
    int runCnt;
    reExec_t reExec;
    int repeatedQueryErrorCount = 0; // JMC - backport 4520

    initReExec( rsComm, &reExec );
    LastRescUpdateTime = time( NULL );
    while ( 1 ) {
#ifndef windows_platform
#ifndef SYSLOG
        chkLogfileName( logDir, RULE_EXEC_LOGFILE );
#endif
#endif
        chkAndResetRule( rsComm );
        rodsLog( LOG_NOTICE,
                 "reServerMain: checking the queue for jobs" );
        status = getReInfo( rsComm, &genQueryOut );
        if ( status < 0 ) {
            if ( status != CAT_NO_ROWS_FOUND ) {
                rodsLog( LOG_ERROR,
                         "reServerMain: getReInfo error. status = %d", status );
#ifdef ORA_ICAT // JMC - backport 4520
                /* For Oracle, since we don't disconnect (to avoid a
                   memory leak in the OCI library), we do want to
                   disconnect when there are repeated errors so we can
                   reconnect and possibly recover from an Oracle
                   outage. */
                if ( repeatedQueryErrorCount > 3 ) { // JMC - backport 4520
                    disconnectRcat( rsComm );
                    repeatedQueryErrorCount = 0;
                }
#endif
            }
            else {   // JMC - backport 4520
                repeatedQueryErrorCount++;
            }
            reSvrSleep( rsComm );
            continue;
        }
        else {   // JMC - backport 4520
            repeatedQueryErrorCount = 0;
        }
        endTime = time( NULL ) + RE_SERVER_EXEC_TIME;
        runCnt = runQueuedRuleExec( rsComm, &reExec, &genQueryOut, endTime, 0 );
        if ( runCnt > 0 ||
                ( genQueryOut != NULL && genQueryOut->continueInx > 0 ) ) {
            /* need to refresh */
            svrCloseQueryOut( rsComm, genQueryOut );
            freeGenQueryOut( &genQueryOut );
            status = getReInfo( rsComm, &genQueryOut );
            if ( status < 0 ) {
                reSvrSleep( rsComm );
                continue;
            }
        }

        /* run the failed job */

        runCnt =
            runQueuedRuleExec( rsComm, &reExec, &genQueryOut, endTime,
                               RE_FAILED_STATUS );
        svrCloseQueryOut( rsComm, genQueryOut );
        freeGenQueryOut( &genQueryOut );
        if ( runCnt > 0 ||
                ( genQueryOut != NULL && genQueryOut->continueInx > 0 ) ) {
            continue;
        }
        else {
            /* nothing got run */
            reSvrSleep( rsComm );
        }
    }
}

int
reSvrSleep( rsComm_t *rsComm ) {
    int status;
    rodsServerHost_t *rodsServerHost = NULL;

    if ( ( status = disconnRcatHost( rsComm, MASTER_RCAT,
                                     rsComm->myEnv.rodsZone ) ) == LOCAL_HOST ) {
#ifdef RODS_CAT
#ifndef ORA_ICAT
        /* For Oracle, we don't disconnect.  This is to avoid a
           memory leak in the OCI library */
        disconnectRcat( rsComm );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "reSvrSleep: disconnectRcat error. status = %d", status );
        }
#endif
#endif
    }
    rodsSleep( RE_SERVER_SLEEP_TIME, 0 );

    if ( ( status = getAndConnRcatHost( rsComm, MASTER_RCAT,
                                        rsComm->myEnv.rodsZone, &rodsServerHost ) ) == LOCAL_HOST ) {
#ifdef RODS_CAT
        status = connectRcat( rsComm );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "reSvrSleep: connectRcat error. status = %d", status );
        }
#endif
    }
    return ( status );
}

int
chkAndResetRule( rsComm_t *rsComm ) {
//    char *configDir;
//    char rulesFileName[MAX_NAME_LEN];
    int status = 0;
    uint mtime;

//    configDir = getConfigDir();
//    snprintf( rulesFileName, MAX_NAME_LEN, "%s/reConfigs/core.re",
//              configDir );

    std::string re_full_path;
    irods::error ret = irods::get_full_path_for_config_file( "core.re", re_full_path );
    if ( !ret.ok() ) {
        irods::log( PASS( ret ) );
        return ret.code();
    }

    path p( re_full_path );
    if ( !exists( p ) ) {
        status = UNIX_FILE_STAT_ERR - errno;
        rodsLog( LOG_ERROR,
                 "chkAndResetRule: unable to read rule config file %s, status = %d",
                 re_full_path.c_str(), status );
        return ( status );
    }

    mtime = ( uint ) last_write_time( p );

    if ( CoreIrbTimeStamp == 0 ) {
        /* first time */
        CoreIrbTimeStamp = mtime;
        return ( 0 );
    }

    if ( mtime > CoreIrbTimeStamp ) {
        /* file has been changed */
        rodsLog( LOG_NOTICE,
                 "chkAndResetRule: reconf file %s has been changed. re-initializing",
                 re_full_path.c_str() );
        CoreIrbTimeStamp = mtime;
        clearCoreRule();
        /* The shared memory cache may have already been updated, do not force reload */
        status = initRuleEngine( RULE_ENGINE_TRY_CACHE, NULL, reRuleStr, reFuncMapStr, reVariableMapStr );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "chkAndResetRule: initRuleEngine error, status = %d", status );
        }
    }
    return status;
}

