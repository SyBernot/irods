/*** Copyright (c), The Regents of the University of California            ***
 *** For more infophybunation please refer to files in the COPYRIGHT directory ***/
/* phybunUtil.h - Header for for phybunUtil.c */

#ifndef PHYBUN_UTIL_HPP
#define PHYBUN_UTIL_HPP

#include "rodsClient.hpp"
#include "parseCommandLine.hpp"
#include "rodsPath.hpp"

#ifdef _WIN32
#include "sys/timeb.h"
#endif

extern "C" {

    int
    phybunUtil( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs,
                rodsPathInp_t *rodsPathInp );
    int
    initCondForPhybunOpr( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                          structFileExtAndRegInp_t *phyBundleCollInp,
                          rodsPathInp_t *rodsPathInp );

}

#endif	/* PHYBUN_UTIL_H */
