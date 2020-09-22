/**
 * @file NTLconvert.h
 *
 * Conversion to and from NTL
**/

#ifndef INCL_NTLCONVERT_H
#define INCL_NTLCONVERT_H

#ifdef HAVE_NTL

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"

#include <NTL/config.h>

#ifdef NTL_STD_CXX
#ifdef NOSTREAMIO
#  ifdef HAVE_IOSTREAM
#    include <iostream>
#    define OSTREAM std::ostream
#    define ISTREAM std::istream
#  elif defined(HAVE_IOSTREAM_H)
#    include <iostream.h>
#    define OSTREAM ostream
#    define ISTREAM istream
#  endif
#endif /* ! NOSTREAMIO */
#endif


#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/lzz_pXFactoring.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/ZZ_pEXFactoring.h>
#include <NTL/lzz_pEXFactoring.h>
#include <NTL/GF2EXFactoring.h>
#include <NTL/mat_ZZ.h>
#include <NTL/mat_lzz_p.h>
#include <NTL/mat_lzz_pE.h>

#ifdef NTL_CLIENT               // in <NTL/tools.h>: using of name space NTL
NTL_CLIENT
#endif




#include "int_int.h"
#include "cf_assert.h"


ZZ_pX convertFacCF2NTLZZpX(const CanonicalForm & f);
zz_pX convertFacCF2NTLzzpX(const CanonicalForm & f);
GF2X convertFacCF2NTLGF2X(const CanonicalForm & f);
CanonicalForm convertNTLZZpX2CF(const ZZ_pX & poly,const Variable & x);
CanonicalForm convertNTLzzpX2CF(const zz_pX & poly,const Variable & x);
CanonicalForm convertNTLGF2X2CF(const GF2X & poly,const Variable & x);
CanonicalForm convertNTLZZX2CF(const ZZX & polynom,const Variable & x);
CFFList convertNTLvec_pair_ZZpX_long2FacCFFList(const vec_pair_ZZ_pX_long& e,const ZZ_p & cont,const Variable & x);
CFFList convertNTLvec_pair_zzpX_long2FacCFFList(const vec_pair_zz_pX_long& e,const zz_p cont,const Variable & x);

CFFList convertNTLvec_pair_GF2X_long2FacCFFList(const vec_pair_GF2X_long& e,const GF2 cont,const Variable & x);
CanonicalForm convertZZ2CF(const ZZ & coefficient);
ZZ convertFacCF2NTLZZ(const CanonicalForm & f);
ZZX convertFacCF2NTLZZX(const CanonicalForm & f);
CFFList convertNTLvec_pair_ZZX_long2FacCFFList(const vec_pair_ZZX_long& e,const ZZ & cont,const Variable & x);
CanonicalForm convertNTLZZpE2CF(const ZZ_pE & coefficient,const Variable & x);
CFFList convertNTLvec_pair_ZZpEX_long2FacCFFList(const vec_pair_ZZ_pEX_long & e,const ZZ_pE & cont,const Variable & x,const Variable & alpha);
CanonicalForm convertNTLGF2E2CF(const GF2E & coefficient,const Variable & x);
CFFList convertNTLvec_pair_GF2EX_long2FacCFFList(const vec_pair_GF2EX_long& e,const GF2E & cont,const Variable & x,const Variable & alpha);
GF2EX convertFacCF2NTLGF2EX(const CanonicalForm & f,const GF2X & mipo);
ZZ_pEX convertFacCF2NTLZZ_pEX(const CanonicalForm & f,const ZZ_pX & mipo);
zz_pEX convertFacCF2NTLzz_pEX(const CanonicalForm & f,const zz_pX & mipo);
CanonicalForm convertNTLzzpE2CF(const zz_pE & f, const Variable & x);
CFFList convertNTLvec_pair_zzpEX_long2FacCFFList(const vec_pair_zz_pEX_long & e,const zz_pE & cont,const Variable & x,const Variable & alpha);
CanonicalForm convertNTLzz_pEX2CF(const zz_pEX & f, const Variable & x, const Variable & alpha);
CanonicalForm convertNTLZZ_pEX2CF(const ZZ_pEX & f, const Variable & x, const Variable & alpha);

mat_ZZ* convertFacCFMatrix2NTLmat_ZZ(const CFMatrix &m);
CFMatrix* convertNTLmat_ZZ2FacCFMatrix(const mat_ZZ &m);
mat_zz_p* convertFacCFMatrix2NTLmat_zz_p(const CFMatrix &m);
CFMatrix* convertNTLmat_zz_p2FacCFMatrix(const mat_zz_p &m);
mat_zz_pE* convertFacCFMatrix2NTLmat_zz_pE(const CFMatrix &m);
CFMatrix* convertNTLmat_zz_pE2FacCFMatrix(const mat_zz_pE &m, const Variable & alpha);

EXTERN_VAR long fac_NTL_char;
#endif
#endif
