#ifndef NUMBER2_H
#define NUMBER2_H

#include <misc/auxiliary.h>

#ifdef SINGULAR_4_1
#include <omalloc/omalloc.h>
#include <coeffs/coeffs.h>
#include <kernel/structs.h>
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

BOOLEAN jjEQUAL_CR(leftv res, leftv a, leftv b); // compare cring

// type conversion:
BOOLEAN jjNUMBER2CR(leftv res, leftv a, leftv b); // <any>,cring ->number2
BOOLEAN jjN2_CR(leftv res, leftv a);              // number2 ->cring
BOOLEAN jjCM_CR(leftv res, leftv a);              // cmatrix ->cring
BOOLEAN jjBIM2_CR(leftv res, leftv a);              // bigint ->cring
BOOLEAN jjR2_CR(leftv res, leftv a);              // ring ->cring
BOOLEAN jjN2_N(leftv res, leftv a);             // number2 ->number

// operations:
BOOLEAN jjNUMBER2_OP1(leftv res, leftv a);
BOOLEAN jjNUMBER2_OP2(leftv res, leftv a, leftv b);

number2 n2Copy(const number2 d);
void n2Delete(number2 &d);
char *n2String(number2 d, BOOLEAN typed);
void n2Print(number2 d);

BOOLEAN jjCMATRIX_3(leftv, leftv, leftv,leftv);
#endif
#endif
