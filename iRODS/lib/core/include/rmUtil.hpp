/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* getUtil.h - Header for for getUtil.c */

#ifndef RM_UTIL_HPP
#define RM_UTIL_HPP

#include "rodsClient.hpp"
#include "parseCommandLine.hpp"
#include "rodsPath.hpp"

#ifdef _WIN32
#include "sys/timeb.h"
#endif

extern "C" {

    int
    rmUtil( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs,
            rodsPathInp_t *rodsPathInp );
    int
    rmDataObjUtil( rcComm_t *conn, char *srcPath,
                   rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                   dataObjInp_t *dataObjInp );
    int
    initCondForRm( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                   dataObjInp_t *dataObjInp, collInp_t *collInp );
    int
    rmCollUtil( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                rodsArguments_t *rodsArgs, dataObjInp_t *dataObjInp, collInp_t *collInp );
    int
    mvDataObjToTrash( rcComm_t *conn, dataObjInp_t *dataObjInp );
    int
    mkTrashPath( rcComm_t *conn, dataObjInp_t *dataObjInp, char *trashPath );
    int
    mvCollToTrash( rcComm_t *conn, dataObjInp_t *dataObjInp );

}

#endif	/* RM_UTIL_H */
