#ifndef RINTEGERS_H
#define RINTEGERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include <misc/auxiliary.h>
#include <coeffs/coeffs.h>

#ifdef HAVE_RINGS

#if SI_INTEGER_VARIANT == 3
#define SR_HDL(A) ((long)(A))
#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)
#define n_Z_IS_SMALL(A)     (SR_HDL(A) & SR_INT)
#define INT_IS_SMALL(A) ( ((A << 1) >> 1) == A )
#endif

//extern int nrzExp;
//extern unsigned long nrzModul;

BOOLEAN nrzInitChar    (coeffs r,  void * parameter);

// will be reused by rmodulon.cc
void    nrzWrite       (number &a, const coeffs r);

#endif

#endif
