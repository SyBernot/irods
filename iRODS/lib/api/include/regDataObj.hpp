/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* regDataObj.h
 */

#ifndef REG_DATA_OBJ_HPP
#define REG_DATA_OBJ_HPP

/* This is Object File I/O type API call */

#include "rods.hpp"
#include "rcMisc.hpp"
#include "procApiRequest.hpp"
#include "apiNumber.hpp"
#include "initServer.hpp"
#include "dataObjInpOut.hpp"

#if defined(RODS_SERVER)
#define RS_REG_DATA_OBJ rsRegDataObj
/* prototype for the server handler */
int
rsRegDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
              dataObjInfo_t **outDataObjInfo );
int
_rsRegDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo );
int
svrRegDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo );
#else
#define RS_REG_DATA_OBJ NULL
#endif

extern "C" {

    /* prototype for the client call */
    int
    rcRegDataObj( rcComm_t *conn, dataObjInfo_t *dataObjInfo,
                  dataObjInfo_t **outDataObjInfo );

    /* rcRegDataObj - Register a iRODS dataObject.
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   dataObjInfo_t *dataObjInfo - the dataObjInfo to register
     *   dataObjInfo_t **outDataObjInfo - the output DataObjInfo which has
     *      the dataId from the registration as output
     *
     * OutPut -
     *   int status - status of the operation.
     */

}

#endif  /* REG_DATA_OBJ_H */
