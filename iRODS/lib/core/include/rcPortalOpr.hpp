/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* rcPortalOpr.h - header file for rcPortalOpr.c
 */



#ifndef RC_PORTAL_OPR_HPP
#define RC_PORTAL_OPR_HPP

#include "rods.hpp"
#include "rodsError.hpp"
#include "objInfo.hpp"
#include "dataObjInpOut.hpp"
#include "QUANTAnet_rbudpBase_c.hpp"
#include "QUANTAnet_rbudpSender_c.hpp"
#include "QUANTAnet_rbudpReceiver_c.hpp"
extern "C" {

#define MAX_PROGRESS_CNT	8

    typedef struct RcPortalTransferInp {
        rcComm_t *conn;
        int destFd;
        int srcFd;
        int threadNum;
        int status;
        rodsLong_t	bytesWritten;
        unsigned char shared_secret[ NAME_LEN ];
    } rcPortalTransferInp_t;

    typedef enum {
        RBUDP_CLIENT,
        RBUDP_SERVER
    } rbudpProcType_t;

    int
    fillRcPortalTransferInp( rcComm_t *conn, rcPortalTransferInp_t *myInput,
                             int destFd, int srcFd, int threadNum );
    int
    putFileToPortal( rcComm_t *conn, portalOprOut_t *portalOprOut,
                     char *locFilePath, char *objPath, rodsLong_t dataSize );
    int
    getFileFromPortal( rcComm_t *conn, portalOprOut_t *portalOprOut,
                       char *locFilePath, char *objPath, rodsLong_t dataSize );
    void
    rcPartialDataPut( rcPortalTransferInp_t *myInput );
    void
    rcPartialDataGet( rcPortalTransferInp_t *myInput );
    int
    rcvTranHeader( int sock, transferHeader_t *myHeader );

    int
    sendTranHeader( int sock, int oprType, int flags, rodsLong_t offset,
                    rodsLong_t length );
    int
    fillBBufWithFile( rcComm_t *conn, bytesBuf_t *myBBuf, char *locFilePath,
                      rodsLong_t dataSize );
    int
    putFile( rcComm_t *conn, int l1descInx, char *locFilePath, char *objPath,
             rodsLong_t dataSize );
    int
    getIncludeFile( rcComm_t *conn, bytesBuf_t *dataObjOutBBuf, char *locFilePath );
    int
    getFile( rcComm_t *conn, int l1descInx, char *locFilePath, char *objPath,
             rodsLong_t dataSize );
    int
    putFileToPortalRbudp( portalOprOut_t *portalOprOut,
                          char *locFilePath, char *objPath, int locFd, rodsLong_t dataSize,
                          int veryVerbose, int sendRate, int packetSize );
    int
    getFileToPortalRbudp( portalOprOut_t *portalOprOut,
                          char *locFilePath, int locFd, rodsLong_t dataSize, int veryVerbose,
                          int packetSize );
    int
    initRbudpClient( rbudpBase_t *rbudpBase, portList_t *myPortList );
    int
    initFileRestart( rcComm_t *conn, char *fileName, char *objPath,
                     rodsLong_t fileSize, int numThr );
    int
    writeLfRestartFile( char *infoFile, fileRestartInfo_t *info );
    int
    readLfRestartFile( char *infoFile, fileRestartInfo_t **info );
    int
    clearLfRestartFile( fileRestart_t *fileRestart );
    int
    lfRestartPutWithInfo( rcComm_t *conn, fileRestartInfo_t *info );
    int
    lfRestartGetWithInfo( rcComm_t *conn, fileRestartInfo_t *info );
    int
    putSeg( rcComm_t *conn, rodsLong_t segSize, int localFd,
            openedDataObjInp_t *dataObjWriteInp, bytesBuf_t *dataObjWriteInpBBuf,
            int bufLen, int *writtenSinceUpdated, fileRestartInfo_t *info,
            rodsLong_t *dataSegLen );
    int
    getSeg( rcComm_t *conn, rodsLong_t segSize, int localFd,
            openedDataObjInp_t *dataObjReadInp, bytesBuf_t *dataObjReadInpBBuf,
            int bufLen, int *writtenSinceUpdated, fileRestartInfo_t *info,
            rodsLong_t *dataSegLen );
    int
    catDataObj( rcComm_t *conn, char *objPath );
}

#endif	/* RC_PORTAL_OPR_H */
