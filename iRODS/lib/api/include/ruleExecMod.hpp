/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ruleExecMod.h
 */

#ifndef RULE_EXEC_MOD_HPP
#define RULE_EXEC_MOD_HPP

/* This is a Metadata type API call */
#include "rods.hpp"
#include "rcMisc.hpp"
#include "procApiRequest.hpp"
#include "apiNumber.hpp"
#include "initServer.hpp"
#include "reGlobalsExtern.hpp"

typedef struct {
    char ruleId[NAME_LEN];
    keyValPair_t condInput;
} ruleExecModInp_t;

#define RULE_EXEC_MOD_INP_PI "str ruleId[NAME_LEN];struct KeyValPair_PI;"

#if defined(RODS_SERVER)
#define RS_RULE_EXEC_MOD rsRuleExecMod
/* prototype for the server handler */
int
rsRuleExecMod( rsComm_t *rsComm, ruleExecModInp_t *ruleExecModInp );
int
_rsRuleExecMod( rsComm_t *rsComm, ruleExecModInp_t *ruleExecModInp );
#else
#define RS_RULE_EXEC_MOD NULL
#endif

extern "C" {

    /* prototype for the client */
    int
    rcRuleExecMod( rcComm_t *conn, ruleExecModInp_t *ruleExecModInp );

}

#endif	/* RULE_EXEC_MOD_H */
