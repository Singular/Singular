#ifndef FFIELDS_H
#define FFIELDS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: finite fields with a none-prime number of elements (via tables)
*/
#include <coeffs/coeffs.h>

typedef struct 
{
  int GFChar;
  int GFDegree;
  char* GFPar_name;
} GFInfo;


BOOLEAN nfInitChar(coeffs r, void*);

//static BOOLEAN nfCoeffsEqual(const coeffs r, n_coeffType n, void* parameter);

BOOLEAN nfGreaterZero (number k, const coeffs r);
number  nfMult        (number a, number b, const coeffs r);
number  nfInit        (int i, const coeffs r);
number  nfPar         (int i, const coeffs r);
int     nfParDeg      (number n, const coeffs r);
int     nfInt         (number &n, const coeffs r);
number  nfAdd         (number a, number b, const coeffs r);
number  nfSub         (number a, number b, const coeffs r);
void    nfPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nfIsZero      (number a, const coeffs r);
BOOLEAN nfIsOne       (number a, const coeffs r);
BOOLEAN nfIsMOne      (number a, const coeffs r);
number  nfDiv         (number a, number b, const coeffs r);
number  nfNeg         (number c, const coeffs r);
number  nfInvers      (number c, const coeffs r);
BOOLEAN nfGreater     (number a, number b, const coeffs r);
BOOLEAN nfEqual       (number a, number b, const coeffs r);
void    nfWrite       (number &a, const coeffs r);
const char *  nfRead  (const char *s, number *a, const coeffs r);
#ifdef LDEBUG
BOOLEAN nfDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
//void    nfSetChar     (const coeffs r);

nMapFunc nfSetMap     (const coeffs src, const coeffs dst);
char *  nfName        (number n, const coeffs r);
void    nfReadTable   (const int c, const coeffs r);

void    nfCoeffWrite(const coeffs r);
void    nfShowMipo(const coeffs r);
#endif

