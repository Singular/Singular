/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*Dense Integer Polynomials
*/
//Schauen was hier überhaupt sinn macht

#include "misc/auxiliary.h"

#ifdef SINGULAR_4_2

#include "omalloc/omalloc.h"
#include "factory/factory.h"
#include "misc/mylimits.h"
#include "reporter/reporter.h"

#include "coeffs.h"
#include "numbers.h"
#include "mpr_complex.h"
#include "AEQ.h"
#include "modulop.h"

#include <string.h>

BOOLEAN nAEQCoeffIsEqual     (number a, number b, const coeffs r);
number  nAEQMult        (number a, number b, const coeffs r);
number  nAEQSub         (number a, number b, const coeffs r);
number  nAEQAdd         (number a, number b, const coeffs r);
number  nAEQDiv         (number a, number b, const coeffs r);
number  nAEQIntMod      (number a, number b, const coeffs r);// Hir wollte wir was gucken
number  nAEQExactDiv    (number a, number b, const coeffs r);
number  nAEQInit        (long i, const coeffs r);
number  nAEQInitMPZ     (mpz_t m, const coeffs r); //nachgucken/fragen
int     nAEQSize        (number a, const coeffs r);///
long    nAEQInt         (number &a, const coeffs r);
number  nAEQMPZ         (number a, const coeffs r); //nachgucken/fragen
number  nAEQNeg         (number c, const coeffs r);
number  nAEQCopy        (number a, number b, const coeffs r); // nachgicken
number  nAEQRePart      (number a, number b, const coeffs r); // nachgicken
number  nAEQImPart      (number a, number b, const coeffs r); // nachgicken

void    nAEQWriteLong   (number a, const coeffs r);//
void    nAEQWriteShort  (number a, const coeffs r);//


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
void    nAEQInpMult      (number &a ,number b, const coeffs r);//
void    nAEQCoeffWrite   (const coeffs r, BOOLEAN details);//

BOOLEAN nAEQClearContent  (number a, const coeffs r);//
BOOLEAN nAEQClearDenominators  (number a, const coeffs r);//




// DEFINITION DER FUNKTIONEN

number  nAEQAdd(number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_add_to(*g);
    return (number) res;
}

number  nAEQMult(number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_mult_n_to(*g);
    return (number) res;
}

number  nAEQSub(number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_sub_to(*g);
    return (number) res;
}


number  nAEQDiv(number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    Q_poly *s=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_div_to(*res,*s,*g);
    return (number) res;
}


number  nAEQIntMod(number a, number, const coeffs)
{
    return a;
}

number  nAEQExactDiv(number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    Q_poly *s=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_div_to(*res,*s,*g);
    return (number) res;
}



number nAEQInit(long i, const coeffs)
{
    number res = (number) i;
    return res;
}

number nAEQInitMPZ(mpz_t m, const coeffs)
{
    number res= (number) m;
    return res;
}

int nAEQSize (number a, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    return f->deg;
}

long nAEQInt(number &, const coeffs)
{
    return 1;
}


number nAEQMPZ(number a, const coeffs)
{
    return a;
}


number nAEQNeg(number c, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (c);
    Q_poly *res=new Q_poly;
    res->Q_poly_set(*f);
    res->Q_poly_neg();
    return (number) res;
}

number nAEQCopy(number c, const coeffs)
{
    return (number) c;
}

number nAEQRePart(number c, const coeffs)
{
    return (number) c;
}

number nAEQImPart(number c, const coeffs)
{
    return (number) c;
}

void    nAEQWriteLong   (number , const coeffs)
{
    return;
}

void    nAEQWriteShort  (number , const coeffs)
{
    return ;
}


const char *  nAEQRead  (const char *, number *, const coeffs)
{
    return "";
}

number nAEQNormalize    (number a, number , const coeffs) // ?
{
    return a;
}

BOOLEAN nAEQGreater     (number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    if (f->deg > g->deg) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEQEqual     (number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    if (f->is_equal(*g) == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEQIsZero      (number a, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    if (f->is_zero() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEQIsOne      (number a, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    if (f->is_one() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEQIsMOne      (number a, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    if (f->is_one() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEQGreaterZero     (number a, const coeffs r)
{
    if (nAEQIsZero(a, r) == FALSE) { return TRUE; }
    else { return FALSE; }
}

void    nAEQPower       (number, int, number *, const coeffs)
{
    return;
}

number nAEQGetDenom      (number &, const coeffs)
{
    return (number) 1;
}

number nAEQGetNumerator      (number &a, const coeffs)
{
    return a;
}

number nAEQGcd           (number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *res=new Q_poly;
    res->Q_poly_gcd(*f,*g);
    return (number) res;
}

number nAEQLcm          (number a, number b, const coeffs)
{
    Q_poly* f=reinterpret_cast<Q_poly*> (a);
    Q_poly* g=reinterpret_cast<Q_poly*> (b);
    Q_poly *gcd=new Q_poly;
    Q_poly *res=new Q_poly;
    Q_poly *s=new Q_poly;
    gcd->Q_poly_gcd(*f,*g);
    res->Q_poly_mult_n(*f,*g);
    res->Q_poly_div_to(*res,*s,*gcd);
    return (number) res;
}

void    nAEQDelete       (number *, const coeffs)
{
    return;
}

number    nAEQSetMap        (number a, const coeffs)
{
        return NULL;
}

void    nAEQInpMult       (number &, number, const coeffs)
{
    return ;
}

void    nAEQCoeffWrite   (const coeffs, BOOLEAN)
{
    return;
}

BOOLEAN nAEQClearContent  (number, const coeffs)
{
    return FALSE;
}

BOOLEAN nAEQClearDenominators  (number, const coeffs)
{
    return FALSE;
}

static char * n_QAECoeffName(const coeffs r)
{
  return (char*)("QAE");
}

static char * n_QAECoeffString(const coeffs r)
{
  return omStrDup("QAE");
}


//INITIALISIERUNG FÜR SINGULAR


BOOLEAN n_QAEInitChar(coeffs r, void *)
{
    // r->is_field,is_domain?
    r->ch=0;
    //r->cfKillChar=ndKillChar;
    //r->nCoeffIsEqual=ndCoeffIsEqual;
    r->cfMult  = nAEQMult;
    r->cfSub   = nAEQSub;
    r->cfAdd   = nAEQAdd;
    r->cfDiv   = nAEQDiv;
    r->cfIntMod= nAEQIntMod;
    r->cfExactDiv= nAEQExactDiv;
    r->cfInit = nAEQInit;
    r->cfSize  = nAEQSize;
    r->cfInt  = nAEQInt;
    r->cfCoeffName = n_QAECoeffName;
    r->cfCoeffString = n_QAECoeffString;
#ifdef HAVE_RINGS
    //r->cfDivComp = NULL; // only for ring stuff
    //r->cfIsUnit = NULL; // only for ring stuff
    //r->cfGetUnit = NULL; // only for ring stuff
    //r->cfExtGcd = NULL; // only for ring stuff
    // r->cfDivBy = NULL; // only for ring stuff
#endif
    r->cfInpNeg   = nAEQNeg;
    r->cfInvers= NULL;
    //r->cfCopy  = ndCopy;
    //r->cfRePart = ndCopy;
    //r->cfImPart = ndReturn0;
    r->cfWriteLong = nAEQWriteLong;
    r->cfRead = nAEQRead;
    //r->cfNormalize=ndNormalize;
    r->cfGreater = nAEQGreater;
    r->cfEqual = nAEQEqual;
    r->cfIsZero = nAEQIsZero;
    r->cfIsOne = nAEQIsOne;
    r->cfIsMOne = nAEQIsOne;
    r->cfGreaterZero = nAEQGreaterZero;
    r->cfPower = nAEQPower; // ZU BEARBEITEN
    r->cfGetDenom = nAEQGetDenom;
    r->cfGetNumerator = nAEQGetNumerator;
    r->cfGcd  = nAEQGcd;
    r->cfLcm  = nAEQLcm; // ZU BEARBEITEN
    r->cfDelete= nAEQDelete;

    r->cfSetMap = nAEQSetMap;

    r->cfInpMult=nAEQInpMult; //????
    r->cfCoeffWrite=nAEQCoeffWrite; //????


    //r->type = n_AE;
    r->has_simple_Alloc=TRUE;
    r->has_simple_Inverse=TRUE;
    return FALSE;
}
#endif
