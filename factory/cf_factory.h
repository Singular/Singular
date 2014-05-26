/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_CF_FACTORY_H
#define INCL_CF_FACTORY_H

// #include "config.h"

#include "cf_defs.h"
#include "variable.h"

#include <factory/cf_gmp.h>

class InternalCF;
class CanonicalForm;

class CFFactory
{
private:
    static int currenttype;
public:
    static int gettype () { return currenttype; }
    static void settype ( int type );
    static InternalCF * basic ( long value );
    static InternalCF * basic ( int type, long value );
    static InternalCF * basic ( const char * str );
    static InternalCF * basic ( const char * str, int base );
    static InternalCF * basic ( int type, const char * const str );
    static InternalCF * basic ( int type, long value, bool nonimm );
    static InternalCF * basic ( const mpz_ptr num );
    static InternalCF * rational ( long num, long den );
    static InternalCF * rational ( const mpz_ptr num, const mpz_ptr den, bool normalize );
    static InternalCF * poly ( const Variable & v, int exp, const CanonicalForm & c );
    static InternalCF * poly ( const Variable & v, int exp = 1 );
};

void getmpi ( InternalCF * value, mpz_t mpi);

#endif /* ! INCL_CF_FACTORY_H */
