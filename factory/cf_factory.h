/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_factory.h,v 1.4 2003-10-15 17:19:40 Singular Exp $ */

#ifndef INCL_CF_FACTORY_H
#define INCL_CF_FACTORY_H

#include <config.h>

#include "cf_gmp.h"

#include "cf_defs.h"
#include "variable.h"

class InternalCF;
class CanonicalForm;

class CFFactory
{
private:
    static int currenttype;
public:
    static int gettype () { return currenttype; }
    static void settype ( int type );
    static InternalCF * basic ( int value );
    static InternalCF * basic ( int type, int value );
    static InternalCF * basic ( const char * str );
    static InternalCF * basic ( const char * str, int base );
    static InternalCF * basic ( int type, const char * const str );
    static InternalCF * basic ( int type, int value, bool nonimm );
    static InternalCF * basic ( const MP_INT & num );
    static InternalCF * rational ( int num, int den );
    static InternalCF * rational ( const MP_INT & num, const MP_INT & den, bool normalize );
    static InternalCF * poly ( const Variable & v, int exp, const CanonicalForm & c );
    static InternalCF * poly ( const Variable & v, int exp = 1 );
};

MP_INT getmpi ( InternalCF * value, bool symmetric = true );

#endif /* ! INCL_CF_FACTORY_H */
