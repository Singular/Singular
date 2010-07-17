#ifndef F5C_HEADER
#define F5C_HEADER

#ifdef HAVE_F5C
#include <kernel/F5cData.h>
#include <kernel/F5cLists.h>

/*!
 * main function of the F5C implementation in SINGULAR
 * 
 * INPUT:   Ideal i for which a gröbner basis should be computed;
 *          ring r.
 * OUTPUT:  ideal g (the gröbner basis of i computed via F5C)
*/
ideal f5cMain(ideal i, ring r);

#endif
// HAVE_F5C
#endif
// F5C_HEADER
