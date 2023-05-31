/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: flint: rational functions over Q (using fmpq_mpoly)
*/

#ifndef FLINTCF_QRAT_H
#define FLINTCF_QRAT_H

#include "misc/auxiliary.h"
#include "coeffs/coeffs.h"
#ifdef HAVE_FLINT
#include <flint/flint.h>
#if __FLINT_RELEASE >= 20503
#include <flint/fmpq_mpoly.h>


typedef struct
{
  char **names;
  int N;
  #ifdef QA_DEBUG
  coeffs C;
  #endif
} QaInfo;

typedef struct
{
   fmpq_mpoly_t num;
   fmpq_mpoly_t den;
   #ifdef QA_DEBUG
   number p;
   #endif
} fmpq_rat_struct;

typedef struct /* this is a temporary hack until we define this structure properly */
{
   fmpq_mpoly_ctx_struct * ctx;
   coeffs C;
} fmpq_rat_data_struct;
#endif
#endif

BOOLEAN flintQrat_InitChar(coeffs cf, void * infoStruct);

coeffs flintQratInitCfByName(char *s, n_coeffType n);

#endif

