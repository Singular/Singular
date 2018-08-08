#ifndef NUMBER2_H
#define NUMBER2_H

#include "kernel/mod2.h"

#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

#include "coeffs/coeffs.h"
#include "kernel/structs.h"
#ifdef SINGULAR_4_2
struct snumber2;
struct spoly2;
typedef struct snumber2 *   number2;
typedef struct spoly2 *     poly2;
struct snumber2
{ coeffs cf;
  number n;
};

struct spoly2
{ ring cf;
  poly n;
};

static inline number2 n2Init(long i, coeffs c)
{ number2 N=(number2)omAlloc0(sizeof(snumber2)); if (c!=NULL) { N->cf=c; N->n=n_Init(i,c);} return N;}

static inline poly2 p2Init(long i, ring c)
{ poly2 N=(poly2)omAlloc0(sizeof(spoly2)); if (c!=NULL) { N->cf=c; N->n=p_ISet(i,c);} return N;}

// type conversion:
BOOLEAN jjNUMBER2CR(leftv res, leftv a, leftv b); // <any>,cring ->number2
BOOLEAN jjN2_CR(leftv res, leftv a);              // number2 ->cring
BOOLEAN jjCM_CR(leftv res, leftv a);              // cmatrix ->cring
BOOLEAN jjBIM2_CR(leftv res, leftv a);            // bigint ->cring
BOOLEAN jjN2_N(leftv res, leftv a);               // number2 ->number
BOOLEAN jjP2_R(leftv res, leftv a);              // poly2 ->ring
// operations:
BOOLEAN jjNUMBER2_OP1(leftv res, leftv a);
BOOLEAN jjNUMBER2_OP2(leftv res, leftv a, leftv b);
BOOLEAN jjNUMBER2_POW(leftv res, leftv a, leftv b);
BOOLEAN jjPOLY2_OP1(leftv res, leftv a);
BOOLEAN jjPOLY2_OP2(leftv res, leftv a, leftv b);
BOOLEAN jjPOLY2_POW(leftv res, leftv a, leftv b);

number2 n2Copy(const number2 d);
void n2Delete(number2 &d);
char *n2String(number2 d, BOOLEAN typed);
void n2Print(number2 d);

poly2 p2Copy(const poly2 d);
void p2Delete(poly2 &d);
char *p2String(poly2 d, BOOLEAN typed);
void p2Print(poly2 d);

BOOLEAN jjCMATRIX_3(leftv, leftv, leftv,leftv);
#endif
char *crString(coeffs c);

void crPrint(coeffs cf);

BOOLEAN jjR2_CR(leftv res, leftv a);              // ring ->cring

BOOLEAN jjCRING_Zp(leftv res, leftv a, leftv b);
BOOLEAN jjCRING_Zm(leftv res, leftv a, leftv b);

BOOLEAN jjEQUAL_CR(leftv res, leftv a, leftv b); // compare cring

#endif
