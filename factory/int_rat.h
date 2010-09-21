/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_INT_RAT_H
#define INCL_INT_RAT_H

#include <config.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "cf_gmp.h"

#include "assert.h"

#include "canonicalform.h"
#include "int_cf.h"
#include "imm.h"

class InternalRational : public InternalCF
{
private:
    MP_INT _num;
    MP_INT _den;
    static int initialized;
    static MP_INT & MPQNUM( const InternalCF * const c );
    static MP_INT & MPQDEN( const InternalCF * const c );
    static void normalize( const MP_INT &, const MP_INT &, MP_INT &, MP_INT & );
public:
    InternalRational();
    InternalRational( const InternalCF& )
    {
	ASSERT( 0, "ups there is something wrong in your code" );
    }
    InternalRational( const int i );
    InternalRational( const int n, const int d );
    InternalRational( const char * str );
    InternalRational( const MP_INT & );
    InternalRational( const MP_INT &, const MP_INT & );
    ~InternalRational();
    InternalCF* deepCopyObject() const;
    const char * const classname() const { return "InternalRational"; }
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

    int intval() const;

    int sign() const;

    InternalCF * normalize_myself();

    friend class InternalInteger;
    friend void gmp_numerator ( const CanonicalForm & f, mpz_ptr result );
    friend void gmp_denominator ( const CanonicalForm & f, mpz_ptr result );
    friend CanonicalForm make_cf ( const MP_INT & n, const MP_INT & d );
};

inline MP_INT & InternalRational::MPQNUM( const InternalCF * const c )
{
    return (((InternalRational*)c)->_num);
}

inline MP_INT & InternalRational::MPQDEN( const InternalCF * const c )
{
    return (((InternalRational*)c)->_den);
}

#endif /* ! INCL_INT_RAT_H */
