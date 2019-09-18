/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers (integers)
*/

#include "misc/auxiliary.h"

#ifdef HAVE_RINGS
#include "factory/factory.h"

#include "misc/mylimits.h"
#include "reporter/reporter.h"

#include "coeffs/rintegers.h"
#include "coeffs/mpr_complex.h"
#include "coeffs/rmodulon.h"
#include "coeffs/longrat.h"
#include "coeffs/numbers.h"

#include <string.h>

THREAD_VAR omBin gmp_nrz_bin = omGetSpecBin(sizeof(mpz_t));

static void nrzCoeffWrite  (const coeffs, BOOLEAN /*details*/)
{
  PrintS("ZZ");
}

static char* nrzCoeffName(const coeffs)
{
  return (char*)"ZZ";
}

static char* nrzCoeffString(const coeffs cf)
{
  return omStrDup(nrzCoeffName(cf));
}

#if SI_INTEGER_VARIANT == 2
#include "rintegers2.cc"
#elif SI_INTEGER_VARIANT == 3
#include "rintegers3.cc"
#elif SI_INTEGER_VARIANT == 1
BOOLEAN nrzInitChar(coeffs r,  void *)
{
  return nlInitChar(r,(void*)1);
}
#else
#error set SI_INTEGER_VARIANT
#endif
#endif
