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

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/mpr_complex.h"

#include "OPAE.h"
#include "AE.h"

#include <string.h>


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
long    nAEInt         (number &a, const coeffs r);
number  nAEMPZ         (number a, const coeffs r); //nachgucken/fragen
number  nAENeg         (number c, const coeffs r);
number  nAECopy        (number a, number b, const coeffs r); // nachgicken
number  nAERePart      (number a, number b, const coeffs r); // nachgicken
number  nAEImPart      (number a, number b, const coeffs r); // nachgicken

void    nAEWriteLong   (number a, const coeffs r);//
void    nAEWriteShort  (number a, const coeffs r);//


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
void    nAEInpMult      (number &a ,number b, const coeffs r);//
void    nAECoeffWrite   (const coeffs r, BOOLEAN details);//

BOOLEAN nAEClearContent  (number a, const coeffs r);//
BOOLEAN nAEClearDenominators  (number a, const coeffs r);//




// DEFINITION DER FUNKTIONEN

number  nAEAdd(number a, number b, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    int_poly* g=reinterpret_cast<int_poly*> (b);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_add_to(*g);
    return (number) res;
}

number  nAEMult(number a, number b, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    int_poly* g=reinterpret_cast<int_poly*> (b);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_mult_n_to(*g);
    return (number) res;
}

number  nAESub(number a, number b, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    int_poly* g=reinterpret_cast<int_poly*> (b);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_sub_to(*g);
    return (number) res;
}


number  nAEDiv(number a, number b, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    int_poly* g=reinterpret_cast<int_poly*> (b);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_div_to(*res,*f,*g);
    return (number) res;
}


number  nAEIntMod(number a, number, const coeffs)
{
    return a;
}

number  nAEExactDiv(number a, number b, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    int_poly* g=reinterpret_cast<int_poly*> (b);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_div_to(*res,*f,*g);
    return (number) res;
}



number nAEInit(long i, const coeffs)
{
    mpz_t m;
    mpz_init_set_ui(m, i);
    int_poly* res=new int_poly;
    res->poly_set(m);
    number res1=reinterpret_cast<number>(res);
    return  res1;
}

number nAEInitMPZ(mpz_t m, const coeffs)
{
    int_poly* res=new int_poly;
    res->poly_set(m);
    number res1=reinterpret_cast<number>(res);
    return  res1;
}


int nAESize (number a, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (a);
    return f->deg;
}

long nAEInt(number &, const coeffs)
{
    return 1;
}


number nAEMPZ(number a, const coeffs)
{
    return a;
}


number nAENeg(number c, const coeffs)
{
    int_poly* f=reinterpret_cast<int_poly*> (c);
    int_poly *res=new int_poly;
    res->poly_set(*f);
    res->poly_neg();
    return (number) res;
}

number nAECopy(number c, const coeffs)
{
    return (number) c;
}

number nAERePart(number c, const coeffs)
{
    return (number) c;
}

number nAEImPart(number c, const coeffs)
{
    return (number) c;
}

void    nAEWriteLong   (number a, const coeffs)
{
    int_poly* f=reinterpret_cast <int_poly*>(a);
    f->poly_print();
    return ;
}

void    nAEWriteShort  (number a, const coeffs)
{
    int_poly* f=reinterpret_cast <int_poly*>(a);
    f->poly_print();
    return ;
}


const char *  nAERead  (const char *, number *, const coeffs)
{
    char* c=new char;
    *c='c';
    return c;
}

number nAENormalize    (number a, number, const coeffs) // ?
{
        return a;
}

BOOLEAN nAEGreater     (number a, number b, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        int_poly* g=reinterpret_cast<int_poly*> (b);
        if (f->deg > g->deg) {return FALSE;}
        else {return TRUE;}
}

BOOLEAN nAEEqual     (number a, number b, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        int_poly* g=reinterpret_cast<int_poly*> (b);
        if (f->is_equal(*g) == 1) {return FALSE;}
        else {return TRUE;}
}

BOOLEAN nAEIsZero      (number a, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        if (f->is_zero() == 1) {return FALSE;}
        else {return TRUE;}
}

BOOLEAN nAEIsOne      (number a, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        if (f->is_one() == 1) {return FALSE;}
        else {return TRUE;}
}

BOOLEAN nAEIsMOne      (number a, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        if (f->is_one() == 1) {return FALSE;}
        else {return TRUE;}
}

BOOLEAN nAEGreaterZero     (number a, const coeffs r)
{
        if (nAEIsZero(a, r) == FALSE) { return TRUE; }
        else { return FALSE; }
}

void    nAEPower       (number, int, number *, const coeffs)
{
        return;
}

number nAEGetDenom      (number &, const coeffs)
{
        return (number) 1;
}

number nAEGetNumerator      (number &a, const coeffs)
{
        return a;
}

number nAEGcd           (number a, number b, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        int_poly* g=reinterpret_cast<int_poly*> (b);
        int_poly *res=new int_poly;
        res->poly_gcd(*f,*g);
        return (number) res;
}

number nAELcm          (number a, number b, const coeffs)
{
        int_poly* f=reinterpret_cast<int_poly*> (a);
        int_poly* g=reinterpret_cast<int_poly*> (b);
        int_poly *gcd=new int_poly;
        int_poly *res=new int_poly;
        gcd->poly_gcd(*f,*g);
        res->poly_mult_n(*f,*g);
        res->poly_div_to(*res,*f,*gcd);
        return (number) res;
}

void    nAEDelete       (number *, const coeffs)
{
        return;
}

nMapFunc  nAESetMap (const coeffs src, const coeffs dst)
{
  return NULL;
}

void    nAEInpMult       (number &, number, const coeffs)
{
        return ;
}

void    nAECoeffWrite   (const coeffs, BOOLEAN)
{
        return;
}

BOOLEAN nAEClearContent  (number, const coeffs)
{
        return FALSE;
}

BOOLEAN nAEClearDenominators  (number, const coeffs)
{
        return FALSE;
}

static char * n_AECoeffName(const coeffs r)
{
  return (char*)("AE");
}

static char * nAECoeffString(const coeffs r)
{
  return omStrDup("AE");
}

//INITIALISIERUNG FÜR SINGULAR


BOOLEAN n_AEInitChar(coeffs r, void *)
{
    // r->is_field, r->is_domain?
    r->ch = 0;
    //r->cfKillChar = ndKillChar; /* dummy */
    //r->nCoeffIsEqual=ndCoeffIsEqual;
    r->cfMult  = nAEMult;
    r->cfSub   = nAESub;
    r->cfAdd   = nAEAdd;
    r->cfDiv   = nAEDiv;
    r->cfIntMod= nAEIntMod;
    r->cfExactDiv= nAEExactDiv;
    r->cfInit = nAEInit;
    r->cfSize  = nAESize;
    r->cfInt  = nAEInt;
    r->cfCoeffName = n_AECoeffName;
#ifdef HAVE_RINGS
    //r->cfDivComp = NULL; // only for ring stuff
    //r->cfIsUnit = NULL; // only for ring stuff
    //r->cfGetUnit = NULL; // only for ring stuff
    //r->cfExtGcd = NULL; // only for ring stuff
    // r->cfDivBy = NULL; // only for ring stuff
#endif
    r->cfInpNeg   = nAENeg;
    r->cfInvers= NULL;
    //r->cfCopy  = ndCopy;
    //r->cfRePart = ndCopy;
    //r->cfImPart = ndReturn0;
    r->cfWriteLong = nAEWriteLong;
    r->cfRead = nAERead;
    //r->cfNormalize=ndNormalize;
    r->cfGreater = nAEGreater;
    r->cfEqual = nAEEqual;
    r->cfIsZero = nAEIsZero;
    r->cfIsOne = nAEIsOne;
    r->cfIsMOne = nAEIsOne;
    r->cfGreaterZero = nAEGreaterZero;
    r->cfPower = nAEPower; // ZU BEARBEITEN
    r->cfGetDenom = nAEGetDenom;
    r->cfGetNumerator = nAEGetNumerator;
    r->cfGcd  = nAEGcd;
    r->cfLcm  = nAELcm; // ZU BEARBEITEN
    r->cfDelete= nAEDelete;
    r->cfCoeffString=nAECoeffString;

    r->cfSetMap = nAESetMap;

    r->cfInpMult=nAEInpMult; //????
    r->cfCoeffWrite=nAECoeffWrite; //????


    //r->type = n_AE;
    r->ch = 0;
    r->has_simple_Alloc=TRUE;
    r->has_simple_Inverse=TRUE;
    return FALSE;
}
#endif
