/* $Id$ */
#ifndef INCL_NTLCONVERT_H
#define INCL_NTLCONVERT_H

#include <config.h>

#include "cf_gmp.h"

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_berlekamp.h"
#include "fac_cantzass.h"
#include "fac_univar.h"
#include "fac_multivar.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"

#ifdef HAVE_NTL

#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/lzz_pXFactoring.h>
#include <NTL/GF2XFactoring.h>
#include "int_int.h"
#include <limits.h>
#include <NTL/ZZ_pEXFactoring.h>
#include <NTL/lzz_pEXFactoring.h>
#include <NTL/GF2EXFactoring.h>
#include <NTL/mat_ZZ.h>
#include <NTL/mat_lzz_p.h>
#include <NTL/mat_lzz_pE.h>


#ifdef NTL_CLIENT               // in <NTL/tools.h>: using of name space NTL
NTL_CLIENT
#endif

ZZ_pX convertFacCF2NTLZZpX(CanonicalForm f);
zz_pX convertFacCF2NTLzzpX(CanonicalForm f);
GF2X convertFacCF2NTLGF2X(CanonicalForm f);
CanonicalForm convertNTLZZpX2CF(ZZ_pX poly,Variable x);
CanonicalForm convertNTLzzpX2CF(zz_pX poly,Variable x);
CanonicalForm convertNTLGF2X2CF(GF2X poly,Variable x);
CanonicalForm convertNTLZZX2CF(ZZX polynom,Variable x);
CFFList convertNTLvec_pair_ZZpX_long2FacCFFList(vec_pair_ZZ_pX_long e,ZZ_p multi,Variable x);
CFFList convertNTLvec_pair_zzpX_long2FacCFFList(vec_pair_zz_pX_long e,zz_p multi,Variable x);

CFFList convertNTLvec_pair_GF2X_long2FacCFFList(vec_pair_GF2X_long e,GF2 multi,Variable x);
CanonicalForm convertZZ2CF(ZZ coefficient);
ZZ convertFacCF2NTLZZ(const CanonicalForm f);
ZZX convertFacCF2NTLZZX(CanonicalForm f);
CFFList convertNTLvec_pair_ZZX_long2FacCFFList(vec_pair_ZZX_long e,ZZ multi,Variable x);
CanonicalForm convertNTLZZpE2CF(ZZ_pE coefficient,Variable x);
CFFList convertNTLvec_pair_ZZpEX_long2FacCFFList(vec_pair_ZZ_pEX_long e,ZZ_pE multi,Variable x,Variable alpha);
CanonicalForm convertNTLGF2E2CF(GF2E coefficient,Variable x);
CFFList convertNTLvec_pair_GF2EX_long2FacCFFList(vec_pair_GF2EX_long e,GF2E multi,Variable x,Variable alpha);
GF2EX convertFacCF2NTLGF2EX(CanonicalForm f,GF2X mipo);
ZZ_pEX convertFacCF2NTLZZ_pEX(CanonicalForm f,ZZ_pX mipo);
zz_pEX convertFacCF2NTLzz_pEX(CanonicalForm f,zz_pX mipo);
CanonicalForm convertNTLzzpE2CF(zz_pE f, Variable x);
CFFList convertNTLvec_pair_zzpEX_long2FacCFFList(vec_pair_zz_pEX_long e,zz_pE multi,Variable x,Variable alpha);
CanonicalForm convertNTLzz_pEX2CF(zz_pEX f, Variable x, Variable alpha);

mat_ZZ* convertFacCFMatrix2NTLmat_ZZ(CFMatrix &m);
CFMatrix* convertNTLmat_ZZ2FacCFMatrix(mat_ZZ &m);
mat_zz_p* convertFacCFMatrix2NTLmat_zz_p(CFMatrix &m);
CFMatrix* convertNTLmat_zz_p2FacCFMatrix(mat_zz_p &m);
mat_zz_pE* convertFacCFMatrix2NTLmat_zz_pE(CFMatrix &m);
CFMatrix* convertNTLmat_zz_pE2FacCFMatrix(mat_zz_pE &m, Variable alpha);

extern int fac_NTL_char;
#endif
#endif
