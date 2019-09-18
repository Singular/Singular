/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_INT_PP_H
#define INCL_INT_PP_H

// #include "config.h"

#include "factory/cf_gmp.h"

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "cf_assert.h"

#include "int_cf.h"


class InternalPrimePower : public InternalCF
{
private:
    mpz_t thempi;
    THREAD_VAR static bool initialized;
    THREAD_VAR static int prime;
    THREAD_VAR static int exp;
    THREAD_VAR static mpz_t primepow;
    THREAD_VAR static mpz_t primepowhalf;
    static void initialize();
    static mpz_ptr MPI( const InternalCF * const c );
public:
    InternalPrimePower();
    InternalPrimePower( const InternalCF& )
    {
	ASSERT( 0, "ups there is something wrong in your code" );
    }
    InternalPrimePower( const int i );
    InternalPrimePower( const char * str, const int base=10 );
    InternalPrimePower( const mpz_ptr );
    ~InternalPrimePower();
    InternalCF* deepCopyObject() const;
    const char * classname() const { return "InternalPrimePower"; }
#ifndef NOSTREAMIO
    void print( OSTREAM&, char* );
#endif /* NOSTREAMIO */
    bool isZero() const;
    bool isOne() const;
    InternalCF* genZero();
    InternalCF* genOne();
    InternalCF* normalize_myself();

    static void setPrimePower( int p, int k );

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

    long intval() const;

    int intmod( int p ) const;

    int sign() const;
    friend mpz_ptr getmpi ( InternalCF * value, bool symmetric );
};

inline mpz_ptr InternalPrimePower::MPI( const InternalCF * const c )
{
    return (((InternalPrimePower*)c)->thempi);
}

#endif /* ! INCL_INT_PP_H */
