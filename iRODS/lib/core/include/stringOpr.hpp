/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* stringOpr.h - header file for stringOpr.c
 */



#ifndef STRING_OPR_HPP
#define STRING_OPR_HPP

#include "rodsDef.hpp"

extern "C" {

    char *rstrcpy( char *dest, const char *src, int maxLen );
    char *rstrcat( char *dest, const char *src, int maxLen );
    char *rstrncat( char *dest, const char *src, int srcLen, int maxLen );

    int
    rSplitStr( char *inStr, char* outStr1, int maxOutLen1,
               char* outStr2, int maxOutLen2, char key );
    int
    copyStrFromBuf( char **buf, char *outStr, int maxOutLen );
    int
    isAllDigit( char *myStr );
    int
    splitPathByKey( const char *srcPath, char *dir, char *file, char key );
    int
    getParentPathlen( char *path );
    int
    trimQuotes( char *s );
    int
    checkStringForSystem( char *inString );
    int
    checkStringForEmailAddress( char *inString );

}

#endif	/* STRING_OPR_H */
