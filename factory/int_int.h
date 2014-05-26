/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_INT_INT_H
#define INCL_INT_INT_H

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

#include "int_cf.h"
// #include <factory/cf_gmp.h>
#include "gmpext.h"

#ifdef HAVE_OMALLOC
#ifndef OM_NDEBUG
#define OM_NDEBUG
#endif
#  include <omalloc/omalloc.h>
#endif

class InternalInteger : public InternalCF
{
private:
    mpz_t thempi;

    // auxilliary methods
    inline InternalCF * normalizeMyself ();
    inline InternalCF * uiNormalizeMyself ();

    static inline InternalCF * normalizeMPI ( mpz_ptr );
    static inline InternalCF * uiNormalizeMPI ( mpz_ptr );

    static inline mpz_ptr MPI ( const InternalCF * const c );
#ifdef HAVE_OMALLOC
  static const omBin InternalInteger_bin;
#endif
public:
#ifdef HAVE_OMALLOC
  void* operator new(size_t)
    {
      void* addr;
      omTypeAllocBin(void*, addr, InternalInteger_bin);
      return addr;
    }
  void operator delete(void* addr, size_t)
    {
      omFreeBin(addr, InternalInteger_bin);
    }
#endif

    InternalInteger();
    InternalInteger( const InternalCF& )
    {
        ASSERT( 0, "ups there is something wrong in your code" );
    }
    InternalInteger( const int i );
    InternalInteger( const long i );
    InternalInteger( const char * str, const int base=10 );
    InternalInteger( const mpz_ptr );
    ~InternalInteger();
    InternalCF* deepCopyObject() const;
    const char * classname() const { return "InternalInteger"; }
#ifndef NOSTREAMIO
    void print( OSTREAM&, char* );
#endif /* NOSTREAMIO */
    InternalCF* genZero();
    InternalCF* genOne();

    bool is_imm() const;

    int levelcoeff() const { return IntegerDomain; }
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

    int intmod( int p ) const;

    int sign() const;

    InternalCF* sqrt();

    int ilog2();

    friend class InternalRational;
    friend void gmp_numerator ( const CanonicalForm & f, mpz_ptr result);
    friend void gmp_denominator ( const CanonicalForm & f, mpz_ptr result );
    friend void getmpi ( InternalCF * value, mpz_t mpi);
};

//{{{ inline InternalCF * InternalInteger::normalizeMyself, uiNormalizeMyself ()
//{{{ docu
//
// normalizeMyself(), uiNormalizeMyself() - normalize CO.
//
// If CO fits into an immediate integer, delete CO and return the
// immediate.  Otherwise, return a pointer to CO.
//
// `uiNormalizeMyself()' is the same as `normalizeMyself()'
// except that CO is expected to be non-begative.  In this case,
// we may use `mpz_get_ui()' to convert the underlying mpi into
// an immediate which is slightly faster than the signed variant.
//
// Note: We do not mind reference counting at this point!  CO is
// deleted unconditionally!
//
//}}}
inline InternalCF *
InternalInteger::normalizeMyself ()
{
    ASSERT( getRefCount() == 1, "internal error: must not delete CO" );

    if ( mpz_is_imm( thempi ) ) {
        InternalCF * result = int2imm( mpz_get_si( thempi ) );
        delete this;
        return result;
    } else
        return this;
}

inline InternalCF *
InternalInteger::uiNormalizeMyself ()
{
    ASSERT( getRefCount() == 1, "internal error: must not delete CO" );

    if ( mpz_is_imm( thempi ) ) {
        InternalCF * result = int2imm( mpz_get_ui( thempi ) );
        delete this;
        return result;
    } else
        return this;
}
//}}}

//{{{ static inline InternalCF * InternalInteger::normalizeMPI, uiNormalizeMPI ( mpz_ptr aMpi )
//{{{ docu
//
// normalizeMPI(), uiNormalizeMPI() - normalize a mpi.
//
// If `aMpi' fits into an immediate integer, clear `aMpi' and
// return the immediate.  Otherwise, return a new
// `InternalInteger' with `aMpi' as underlying mpi.
//
// `uiNormalizeMPI()' is the same as `normalizeMPI()' except that
// `aMpi' is expected to be non-begative.  In this case, we may
// use `mpz_get_ui()' to convert `aMpi' into an immediate which
// is slightly faster than the signed variant.
//
//}}}
inline InternalCF *
InternalInteger::normalizeMPI ( mpz_ptr aMpi )
{
    if ( mpz_is_imm( aMpi ) ) {
        InternalCF * result = int2imm( mpz_get_si( aMpi ) );
        mpz_clear( aMpi );
        return result;
    } else
        return new InternalInteger( aMpi );
}

inline InternalCF *
InternalInteger::uiNormalizeMPI ( mpz_ptr aMpi )
{
    if ( mpz_is_imm( aMpi ) ) {
        InternalCF * result = int2imm( mpz_get_ui( aMpi ) );
        mpz_clear( aMpi );
        return result;
    } else
        return new InternalInteger( aMpi );
}
//}}}

//{{{ inline mpz_ptr InternalInteger::MPI ( const InternalCF * const c )
//{{{ docu
//
// MPI() - return underlying mpi of `c'.
//
// `c' is expected to be an `InternalInteger *'.  `c's underlying
// mpi is returned.
//
//}}}
inline mpz_ptr
InternalInteger::MPI ( const InternalCF * const c )
{
    return (((InternalInteger*)c)->thempi);
}
//}}}

#endif /* ! INCL_INT_INT_H */
