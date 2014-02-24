#ifndef OPAEP_H
#define OPAEP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <coeffs/coeffs.h>


BOOLEAN n_pAEInitChar(coeffs , void *);
BOOLEAN nAEpCoeffIsEqual     (number a, number b, const coeffs r);
number  nAEpMult        (number a, number b, const coeffs r);
number  nAEpSub         (number a, number b, const coeffs r);
number  nAEpAdd         (number a, number b, const coeffs r);
number  nAEpDiv         (number a, number b, const coeffs r);
number  nAEpIntDiv      (number a, number b, const coeffs r); //Hir wollte wir was gucken
number  nAEpIntMod      (number a, number b, const coeffs r);// Hir wollte wir was gucken
number  nAEpExactDiv    (number a, number b, const coeffs r);
number  nAEpInit        (long i, const coeffs r);
number  nAEpInitMPZ     (mpz_t m, const coeffs r); //nachgucken/fragen
int     nAEpSize        (number a, const coeffs r);///
int     nAEpInt         (number &a, const coeffs r);
number  nAEpMPZ         (number a, const coeffs r); //nachgucken/fragen
number  nAEpNeg         (number c, const coeffs r);
number  nAEpCopy        (number a, number b, const coeffs r); // nachgicken
number  nAEpRePart      (number a, number b, const coeffs r); // nachgicken
number  nAEpImPart      (number a, number b, const coeffs r); // nachgicken

void    nAEpWriteLong   (number &a, const coeffs r);//
void    nAEpWriteShort  (number &a, const coeffs r);//


const char *  nAEpRead  (const char *s, number *a, const coeffs r);
number nAEpNormalize    (number a, number b, const coeffs r);//
BOOLEAN nAEpGreater     (number a, number b, const coeffs r);//
BOOLEAN nAEpEqual       (number a, number b, const coeffs r);
BOOLEAN nAEpIsZero      (number a, const coeffs r);
BOOLEAN nAEpIsOne       (number a, const coeffs r);
BOOLEAN nAEpIsMOne      (number a, const coeffs r);
BOOLEAN nAEpGreaterZero (number a, number b, const coeffs r);
void    nAEpPower       (number a, int i, number * result, const coeffs r);
number nAEpGetDenom     (number &a, const coeffs r);//
number nAEpGetNumerator (number &a, const coeffs r);//
number nAEpGcd          (number a, number b, const coeffs r);
number nAEpLcm          (number a, number b, const coeffs r);

void    nAEpDelete       (number *a, const coeffs r);//
number    nAEpSetMap      (number a, const coeffs r);//
char*    nAEpName         (number a, const coeffs r);//
void    nAEpInpMult      (number &a ,number b, const coeffs r);//
void    nAEpCoeffWrite   (const coeffs r, BOOLEAN details);//

BOOLEAN nAEpClearContent  (number a, const coeffs r);//
BOOLEAN nAEpClearDenominators  (number a, const coeffs r);//



#endif
