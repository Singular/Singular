/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_INT_RAT_H
#define INCL_INT_RAT_H

/**
 * @file int_rat.h
 *
 * Factory's internal rationals
**/
// #include "config.h"

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

#include "canonicalform.h"
#include "int_cf.h"
#include "imm.h"

/**
 * factory's class for rationals
 *
 * a rational is represented as two mpz_t's _num, _den
 *
 * Note: If you want to compute over Q make sure that SW_RATIONAL is set to 1!
 *
 * @sa InternalInteger
**/
class InternalRational : public InternalCF
{
private:
    mpz_t _num;
    mpz_t _den;
    THREAD_VAR static int initialized;
    static mpz_ptr MPQNUM( const InternalCF * const c );
    static mpz_ptr MPQDEN( const InternalCF * const c );
    static void normalize( const mpz_ptr, const mpz_ptr, mpz_ptr, mpz_ptr );
public:
    InternalRational();
    InternalRational( const InternalCF& )
    {
	ASSERT( 0, "ups there is something wrong in your code" );
    }
    InternalRational( const int i );
    InternalRational( const int n, const int d );
    InternalRational( const long i );
    InternalRational( const long n, const long d );
    InternalRational( const char * str );
    InternalRational( const mpz_ptr );
    InternalRational( const mpz_ptr , const mpz_ptr );
    ~InternalRational();
    InternalCF* deepCopyObject() const;
    const char * classname() const { return "InternalRational"; }
#ifndef NOSTREAMIO
    void print( OSTREAM&, char* );
#endif /* NOSTREAMIO */
    InternalCF* genZero();
    InternalCF* genOne();

    bool is_imm() const;
    int levelcoeff() const { return RationalDomain; }

    InternalCF* num();
    InternalCF* den();

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

    InternalCF * bgcdsame ( const InternalCF * const ) const;
    InternalCF * bgcdcoeff ( const InternalCF * const );

    InternalCF * bextgcdsame ( InternalCF *, CanonicalForm &, CanonicalForm & );
    InternalCF * bextgcdcoeff ( InternalCF *, CanonicalForm &, CanonicalForm & );

    long intval() const;

    int sign() const;

    InternalCF * normalize_myself();

    friend class InternalInteger;
    friend void gmp_numerator ( const CanonicalForm & f, mpz_ptr result );
    friend void gmp_denominator ( const CanonicalForm & f, mpz_ptr result );
    friend CanonicalForm make_cf ( const mpz_ptr n, const mpz_ptr d );
};

inline mpz_ptr InternalRational::MPQNUM( const InternalCF * const c )
{
    return (((InternalRational*)c)->_num);
}

inline mpz_ptr InternalRational::MPQDEN( const InternalCF * const c )
{
    return (((InternalRational*)c)->_den);
}

#endif /* ! INCL_INT_RAT_H */
