/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: parseutil.h,v 1.2 1997-06-19 12:22:00 schmidt Exp $ */

#ifndef INCL_PARSEUTIL_H
#define INCL_PARSEUTIL_H

#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "variable.h"

class PUtilBase;

class ParseUtil
{
private:
    PUtilBase * value;
public:
    ParseUtil();
    ParseUtil( const ParseUtil & );
    ParseUtil( const CanonicalForm & );
    ParseUtil( const Variable & );
    ParseUtil( int );
    ParseUtil( char * );
    ~ParseUtil();
    ParseUtil & operator= ( const ParseUtil & );
    ParseUtil & operator= ( const CanonicalForm & );
    ParseUtil & operator= ( const Variable & );
    ParseUtil & operator= ( int );
    CanonicalForm getval();
    int getintval();
};

#endif /* ! INCL_PARSEUTIL_H */
