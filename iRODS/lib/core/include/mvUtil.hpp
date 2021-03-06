/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* getUtil.h - Header for for getUtil.c */

#ifndef MV_UTIL_HPP
#define MV_UTIL_HPP

#include <stdio.h>
#include "rodsClient.hpp"
#include "parseCommandLine.hpp"
#include "rodsPath.hpp"

extern "C" {

    int
    mvUtil( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs,
            rodsPathInp_t *rodsPathInp );
    int
    initCondForMv( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                   dataObjCopyInp_t *dataObjCopyInp );
    int
    mvObjUtil( rcComm_t *conn, char *srcPath, char *targPath, objType_t objType,
               rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
               dataObjCopyInp_t *dataObjCopyInp );

}

#endif	/* MV_UTIL_H */
