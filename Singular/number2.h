#ifndef NUMBER2_H
#define NUMBER2_H

#include<libpolys/coeffs/coeffs.h>
struct snumber2;
typedef struct snumber2 *   number2;
struct snumber2
{ coeffs cf;
  number n;
};

static inline number2 n2Init(long i, coeffs c)
{ number2 N=(number2)omAlloc0(sizeof(snumber2)); if (c!=NULL) { N->cf=c; N->n=n_Init(i,c);} return N;}

char *crString(coeffs c);

void crPrint(coeffs cf);

BOOLEAN jjCRING_Zp(leftv res, leftv a, leftv b);
BOOLEAN jjCRING_Zm(leftv res, leftv a, leftv b);
#endif
