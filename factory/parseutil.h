// emacs edit mode for this file is -*- C++ -*-
// $Id: parseutil.h,v 1.1 1997-04-15 09:34:48 schmidt Exp $

#ifndef INCL_PARSEUTIL_H
#define INCL_PARSEUTIL_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:42  stobbe
Initial revision

*/

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

#endif
