/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_pp.h,v 1.4 2003-10-15 17:19:41 Singular Exp $ */

#ifndef INCL_INT_PP_H
#define INCL_INT_PP_H

#include <config.h>

#include "cf_gmp.h"

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

#include "assert.h"

#include "int_cf.h"


class InternalPrimePower : public InternalCF
{
private:
    MP_INT thempi;
    static int initialized;
    static int prime;
    static int exp;
    static MP_INT primepow;
    static MP_INT primepowhalf;
    static int initialize();
    static MP_INT & MPI( const InternalCF * const c );
public:
    InternalPrimePower();
    InternalPrimePower( const InternalCF& )
    {
	ASSERT( 0, "ups there is something wrong in your code" );
    }
    InternalPrimePower( const int i );
    InternalPrimePower( const char * str, const int base=10 );
    InternalPrimePower( const MP_INT & );
    ~InternalPrimePower();
    InternalCF* deepCopyObject() const;
    const char * const classname() const { return "InternalPrimePower"; }
#ifndef NOSTREAMIO
    void print( ostream&, char* );
#endif /* NOSTREAMIO */
    bool isZero() const;
    bool isOne() const;
    InternalCF* genZero();
    InternalCF* genOne();
    InternalCF* normalize_myself();

    static void setPrimePower( int p, int k );
    static int getp();
    static int getk();

    bool is_imm() const;

    int levelcoeff() const { return PrimePowerDomain; }
    InternalCF* neg();

    int comparesame( InternalCF* );

    InternalCF* addsame( InternalCF* );
    InternalCF* subsame( InternalCF* );
    InternalCF* mulsame( InternalCF* );
    InternalCF* dividesame( InternalCF* );
    InternalCF* modulosame( InternalCF* );
    InternalCF* divsame( InternalCF* );
    InternalCF* modsame( InternalCF* );
    void divremsame( InternalCF*, InternalCF*&, InternalCF*& );
    bool divremsamet( InternalCF*, InternalCF*&, InternalCF*& );

    int comparecoeff( InternalCF* );

    InternalCF* addcoeff( InternalCF* );
    InternalCF* subcoeff( InternalCF*, bool );
    InternalCF* mulcoeff( InternalCF* );
    InternalCF* dividecoeff( InternalCF*, bool );
    InternalCF* modulocoeff( InternalCF*, bool );
    InternalCF* divcoeff( InternalCF*, bool );
    InternalCF* modcoeff( InternalCF*, bool );
    void divremcoeff( InternalCF*, InternalCF*&, InternalCF*&, bool );
    bool divremcoefft( InternalCF*, InternalCF*&, InternalCF*&, bool );

    int intval() const;

    int intmod( int p ) const;

    int sign() const;
    friend MP_INT getmpi ( InternalCF * value, bool symmetric );
};

inline MP_INT & InternalPrimePower::MPI( const InternalCF * const c )
{
    return (((InternalPrimePower*)c)->thempi);
}

#endif /* ! INCL_INT_PP_H */
