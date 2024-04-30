#ifndef RINTEGERS_H
#define RINTEGERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"

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

void    nrzWrite       (number a, const coeffs r); /*for rmodulon.cc*/
void    nrzDelete      (number *a, const coeffs);  /*for rmodulon.cc*/
int     nrzSize        (number a, const coeffs);   /*for rmodulon.cc*/
void nrzWriteFd(number n, const ssiInfo* d, const coeffs); /*for rmodulon.cc*/
number nrzReadFd(const ssiInfo *d, const coeffs);  /*for rmodulon.cc*/

number  nrzInit        (long i, const coeffs r); /*for SAGE, better: n_Init*/

#endif
