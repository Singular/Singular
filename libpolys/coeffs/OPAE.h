#ifndef OPAE_H
#define OPAE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <coeffs/coeffs.h>


BOOLEAN n_AEInitChar(coeffs , void *);
BOOLEAN nAECoeffIsEqual     (number a, number b, const coeffs r);
number  nAEMult        (number a, number b, const coeffs r);
number  nAESub         (number a, number b, const coeffs r);
number  nAEAdd         (number a, number b, const coeffs r);
number  nAEDiv         (number a, number b, const coeffs r);
number  nAEIntMod      (number a, number b, const coeffs r);// Hir wollte wir was gucken
number  nAEExactDiv    (number a, number b, const coeffs r);
number  nAEInit        (long i, const coeffs r);
number  nAEInitMPZ     (mpz_t m, const coeffs r); //nachgucken/fragen
int     nAESize        (number a, const coeffs r);///
int     nAEInt         (number &a, const coeffs r);
number  nAEMPZ         (number a, const coeffs r); //nachgucken/fragen
number  nAENeg         (number c, const coeffs r);
number  nAECopy        (number a, number b, const coeffs r); // nachgicken
number  nAERePart      (number a, number b, const coeffs r); // nachgicken
number  nAEImPart      (number a, number b, const coeffs r); // nachgicken

void    nAEWriteLong   (number &a, const coeffs r);//
void    nAEWriteShort  (number &a, const coeffs r);//


const char *  nAERead  (const char *s, number *a, const coeffs r);
number nAENormalize    (number a, number b, const coeffs r);//
BOOLEAN nAEGreater     (number a, number b, const coeffs r);//
BOOLEAN nAEEqual       (number a, number b, const coeffs r);
BOOLEAN nAEIsZero      (number a, const coeffs r);
BOOLEAN nAEIsOne       (number a, const coeffs r);
BOOLEAN nAEIsMOne      (number a, const coeffs r);
BOOLEAN nAEGreaterZero (number a, number b, const coeffs r);
void    nAEPower       (number a, int i, number * result, const coeffs r);
number nAEGetDenom     (number &a, const coeffs r);//
number nAEGetNumerator (number &a, const coeffs r);//
number nAEGcd          (number a, number b, const coeffs r);
number nAELcm          (number a, number b, const coeffs r);

void    nAEDelete       (number *a, const coeffs r);//
number    nAESetMap      (number a, const coeffs r);//
char*    nAEName         (number a, const coeffs r);//
void    nAEInpMult      (number &a ,number b, const coeffs r);//
void    nAECoeffWrite   (const coeffs r, BOOLEAN details);//

BOOLEAN nAEClearContent  (number a, const coeffs r);//
BOOLEAN nAEClearDenominators  (number a, const coeffs r);//



#endif
