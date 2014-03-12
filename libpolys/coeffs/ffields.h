#ifndef FFIELDS_H
#define FFIELDS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: finite fields with a none-prime number of elements (via tables)
*/
#include <misc/auxiliary.h>

struct n_Procs_s;
typedef struct  n_Procs_s  *coeffs;

//// Initialize r (n_GF)
BOOLEAN nfInitChar(coeffs r, void*);

/// Show the mininimal polynom..
/// by appending it to the current string buffer via StringAppend
/// (avoid creating a temp. buffer of its own)
/// NOTE: this is used by char *  sleftv::String(void *d, BOOLEAN typed, int dim) (from Singular/subexpr.cc)
void    nfShowMipo(const coeffs r);

#endif
