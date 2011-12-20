#ifndef FLINT_CONVERT_H
#define FLINT_CONVERT_H

#include <config.h>
#include "canonicalform.h"

#ifdef HAVE_FLINT
#ifdef __cplusplus
extern "C"
{
#endif
#include <fmpz.h>
#include <fmpq.h>
#include <fmpz_poly.h>
#include <fmpq_poly.h>
#include <nmod_poly.h>
#ifdef __cplusplus
}
#endif

void convertCF2Fmpz (fmpz_t result, const CanonicalForm& f);
void convertFacCF2Fmpz_poly_t (fmpz_poly_t result, const CanonicalForm& f);
CanonicalForm convertFmpz2CF (fmpz_t coefficient);
CanonicalForm convertFmpz_poly_t2FacCF (fmpz_poly_t poly, const Variable& x);
void convertFacCF2nmod_poly_t (nmod_poly_t result, const CanonicalForm& f);
CanonicalForm convertnmod_poly_t2FacCF (nmod_poly_t poly, const Variable& x);
void convertCF2Fmpq (fmpq_t result, const CanonicalForm& f);
CanonicalForm convertFmpq_poly_t2FacCF (fmpq_poly_t p, const Variable& x);
void convertFacCF2Fmpq_poly_t (fmpq_poly_t result, const CanonicalForm& f);
CFFList convertFLINTnmod_poly_factor2FacCFFList (nmod_poly_factor_t fac,
                                                  mp_limb_t leadingCoeff,
                                                  const Variable& x
                                                 );

#endif
#endif
