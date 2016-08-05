/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: flint: nmod_poly
*/

#ifndef FLINTCF_ZN_H
#define FLINTCF_ZN_H

#include <singularconfig.h>
#include <misc/auxiliary.h>

#ifdef SINGULAR_4_1
#ifdef HAVE_FLINT
typedef struct
{
  int ch;
  char *name;
} flintZn_struct;
BOOLEAN flintZn_InitChar(coeffs cf, void * infoStruct);
#endif
#endif

#endif

