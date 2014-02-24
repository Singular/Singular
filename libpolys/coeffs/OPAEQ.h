#ifndef OPAEQ_H
#define OPAEQ_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <coeffs/coeffs.h>


BOOLEAN n_QAEInitChar(coeffs , void *);
BOOLEAN nAEQCoeffIsEqual     (number a, number b, const coeffs r);
number  nAEQMult        (number a, number b, const coeffs r);
number  nAEQSub         (number a, number b, const coeffs r);
number  nAEQAdd         (number a, number b, const coeffs r);
number  nAEQDiv         (number a, number b, const coeffs r);
number  nAEQIntDiv      (number a, number b, const coeffs r); //Hir wollte wir was gucken
number  nAEQIntMod      (number a, number b, const coeffs r);// Hir wollte wir was gucken
number  nAEQExactDiv    (number a, number b, const coeffs r);
number  nAEQInit        (long i, const coeffs r);
number  nAEQInitMPZ     (mpz_t m, const coeffs r); //nachgucken/fragen
int     nAEQSize        (number a, const coeffs r);///
int     nAEQInt         (number &a, const coeffs r);
number  nAEQMPZ         (number a, const coeffs r); //nachgucken/fragen
number  nAEQNeg         (number c, const coeffs r);
number  nAEQCopy        (number a, number b, const coeffs r); // nachgicken
number  nAEQRePart      (number a, number b, const coeffs r); // nachgicken
number  nAEQImPart      (number a, number b, const coeffs r); // nachgicken

void    nAEQWriteLong   (number &a, const coeffs r);//
void    nAEQWriteShort  (number &a, const coeffs r);//


const char *  nAEQRead  (const char *s, number *a, const coeffs r);
number nAEQNormalize    (number a, number b, const coeffs r);//
BOOLEAN nAEQGreater     (number a, number b, const coeffs r);//
BOOLEAN nAEQEqual       (number a, number b, const coeffs r);
BOOLEAN nAEQIsZero      (number a, const coeffs r);
BOOLEAN nAEQIsOne       (number a, const coeffs r);
BOOLEAN nAEQIsMOne      (number a, const coeffs r);
BOOLEAN nAEQGreaterZero (number a, number b, const coeffs r);
void    nAEQPower       (number a, int i, number * result, const coeffs r);
number nAEQGetDenom     (number &a, const coeffs r);//
number nAEQGetNumerator (number &a, const coeffs r);//
number nAEQGcd          (number a, number b, const coeffs r);
number nAEQLcm          (number a, number b, const coeffs r);

void    nAEQDelete       (number *a, const coeffs r);//
number    nAEQSetMap      (number a, const coeffs r);//
char*    nAEQName         (number a, const coeffs r);//
void    nAEQInpMult      (number &a ,number b, const coeffs r);//
void    nAEQCoeffWrite   (const coeffs r, BOOLEAN details);//

BOOLEAN nAEQClearContent  (number a, const coeffs r);//
BOOLEAN nAEQClearDenominators  (number a, const coeffs r);//



#endif
