// emacs edit mode for this file is -*- C++ -*-
// $Id: int_rat.h,v 1.0 1996-05-17 10:59:41 stobbe Exp $

#ifndef INCL_INTERNALRATIONAL_H
#define INCL_INTERNALRATIONAL_H

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>
#include "cf_gmp.h"

#include "int_cf.h"


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
    void print( ostream&, char* );
    bool isZero() const;
    bool isOne() const;
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

    int intval() const;

    int sign() const;

    InternalCF * normalize_myself();

    friend class InternalInteger;
#ifdef SINGULAR
    friend MP_INT gmp_numerator ( const CanonicalForm & f );
    friend MP_INT gmp_denominator ( const CanonicalForm & f );
#endif /* SINGULAR */
#ifdef MULTIPROTOCOL
    friend MP_INT gmp_numerator_mp ( const CanonicalForm & f );
    friend MP_INT gmp_denominator_mp ( const CanonicalForm & f );
#endif /* MULTIPROTOCOL */
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

#endif /* INCL_INTERNALRATIONAL_H */
