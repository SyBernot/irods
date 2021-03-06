/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* fileRename.h - This file may be generated by a program or script
 */

#ifndef FILE_RENAME_HPP
#define FILE_RENAME_HPP

/* This is a Object File I/O type API call */

#include "rods.hpp"
#include "rcMisc.hpp"
#include "procApiRequest.hpp"
#include "apiNumber.hpp"
#include "initServer.hpp"

#include "fileDriver.hpp"

typedef struct {
    rodsHostAddr_t addr;
    char oldFileName[MAX_NAME_LEN];
    char newFileName[MAX_NAME_LEN];
    char rescHier[MAX_NAME_LEN];
    char objPath[MAX_NAME_LEN];
} fileRenameInp_t;

#define fileRenameInp_PI "struct RHostAddr_PI; str oldFileName[MAX_NAME_LEN]; str newFileName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN];"

typedef struct {
    char file_name[ MAX_NAME_LEN ];
} fileRenameOut_t;

#define fileRenameOut_PI "str file_name[MAX_NAME_LEN];"

#if defined(RODS_SERVER)
#define RS_FILE_RENAME rsFileRename
/* prototype for the server handler */
int
rsFileRename( rsComm_t *rsComm, fileRenameInp_t *fileRenameInp, fileRenameOut_t** );
int
_rsFileRename( rsComm_t *rsComm, fileRenameInp_t *fileRenameInp, fileRenameOut_t**,
               rodsServerHost_t *rodsServerHost );
int
remoteFileRename( rsComm_t *rsComm, fileRenameInp_t *fileRenameInp, fileRenameOut_t**,
                  rodsServerHost_t *rodsServerHost );
#else
#define RS_FILE_RENAME NULL
#endif

/* prototype for the client call */
int
rcFileRename( rcComm_t *conn, fileRenameInp_t *fileRenameInp, fileRenameOut_t** );

#endif  /* FILE_RENAME_H */
