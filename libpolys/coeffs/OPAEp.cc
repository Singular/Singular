/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* Dense Polynomials modulo p
*/
//Schauen was hier überhaupt sinn macht

#include <misc/auxiliary.h>

#include <factory/factory.h>

#include <string.h>
#include <omalloc/omalloc.h>
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <coeffs/mpr_complex.h>
#include <misc/mylimits.h>
#include <coeffs/OPAEp.h>
#include <coeffs/AEp.h>





// DEFINITION DER FUNKTIONEN

number  nAEpAdd(number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_add_to(*g);
    return (number) res;
}

number  nAEpMult(number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_mult_n_to(*g);
    return (number) res;
}

number  nAEpSub(number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_sub_to(*g);
    return (number) res;
}


number  nAEpDiv(number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    p_poly *s=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_div_to(*res,*s,*g);
    return (number) res;
}


number  nAEpIntMod(number a, number, const coeffs)
{
    return a;
}

number  nAEpExactDiv(number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    p_poly *s=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_div_to(*res,*s,*g);
    return (number) res;
}



number nAEpInit(long i, const coeffs)
{
    int j=7;
    mpz_t m;
    mpz_init_set_ui(m, i);
    p_poly* res=new p_poly;
    res->p_poly_set(m, j);
    number res1=reinterpret_cast<number>(res);
    return  res1;
}

number nAEpInitMPZ(mpz_t m, const coeffs)
{
    int j=7;
    p_poly* res=new p_poly;
    res->p_poly_set(m, j);
    number res1=reinterpret_cast<number>(res);
    return  res1;

}

int nAEpSize (number a, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    return f->deg;
}

int nAEpInt(number &, const coeffs)
{
    return 1;
}


number nAEpMPZ(number a, const coeffs)
{
    return a;
}


number nAEpNeg(number c, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (c);
    p_poly *res=new p_poly;
    res->p_poly_set(*f);
    res->p_poly_neg();
    return (number) res;
}

number nAEpCopy(number c, const coeffs)
{
    return c;
}

number nAEpRePart(number c, const coeffs)
{
    return c;
}

number nAEpImPart(number c, const coeffs)
{
    return  c;
}

void    nAEpWriteLong   (number &a, const coeffs)
{
    p_poly* f=reinterpret_cast <p_poly*>(a);
    f->p_poly_print();

    return;
}

void    nAEpWriteShort  (number &a, const coeffs)
{
    p_poly* f=reinterpret_cast <p_poly*>(a);
    f->p_poly_print();
    return ;
}


const char *  nAEpRead  (const char *, number *a, const coeffs)
{
    p_poly& f=reinterpret_cast <p_poly&>(a);
    f.p_poly_insert();
    f.p_poly_print();
    *a=reinterpret_cast <number>(&f);
    char* c=new char;
    *c='c';
    return c;
}

number nAEpNormalize    (number a, number, const coeffs) // ?
{
    return a;
}

BOOLEAN nAEpGreater     (number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    if (f->deg > g->deg) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEpEqual     (number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    if (f->is_equal(*g) == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEpIsZero      (number a, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    if (f->is_zero() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEpIsOne      (number a, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    if (f->is_one() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEpIsMOne      (number a, const coeffs r)
{
    number b=nAEpNeg(a, r);
    p_poly* f=reinterpret_cast<p_poly*> (b);
    if (f->is_one() == 1) {return FALSE;}
    else {return TRUE;}
}

BOOLEAN nAEpGreaterZero     (number a, const coeffs r)
{
    if (nAEpIsZero(a, r) == FALSE) { return TRUE; }
    else { return FALSE; }
}

void    nAEpPower       (number, int, number *, const coeffs)
{
    return;
}

number nAEpGetDenom      (number &, const coeffs)
{
    return (number) 1;
}

number nAEpGetNumerator      (number &a, const coeffs)
{
    return a;
}

number nAEpGcd           (number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *res=new p_poly;
    res->p_poly_gcd(*f,*g);
    return (number) res;
}

number nAEpLcm          (number a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    p_poly *gcd=new p_poly;
    p_poly *res=new p_poly;
    p_poly *s=new p_poly;
    gcd->p_poly_gcd(*f,*g);
    res->p_poly_mult_n(*f,*g);
    res->p_poly_div_to(*res,*s,*gcd);
    return (number) res;
}

void    nAEpDelete       (number *, const coeffs)
{
    return;
}

/*
number    nAEpSetMap        (number a, const coeffs)
{
        return a;
}
*/

void    nAEpInpMult       (number &a, number b, const coeffs)
{
    p_poly* f=reinterpret_cast<p_poly*> (a);
    p_poly* g=reinterpret_cast<p_poly*> (b);
    f->p_poly_mult_n_to(*g);
    a=(number) f;
    return ;
}

void    nAEpCoeffWrite   (const coeffs, BOOLEAN)
{
    return;
}

BOOLEAN nAEpClearContent  (number, const coeffs)
{
    return FALSE;
}

BOOLEAN nAEpClearDenominators  (number, const coeffs)
{
    return FALSE;
}



//INITIALISIERUNG FÜR SINGULAR


BOOLEAN n_pAEInitChar(coeffs r, void *p)
{
    // r->is_field, is_domain
    //Charakteristik abgreifen!
    const int c = (int) (long) p;


    r->ch=c;
    r->cfKillChar=NULL;
    r->nCoeffIsEqual=ndCoeffIsEqual;
    r->cfMult  = nAEpMult;
    r->cfSub   = nAEpSub;
    r->cfAdd   = nAEpAdd;
    r->cfDiv   = nAEpDiv;
    r->cfIntMod= nAEpIntMod;
    r->cfExactDiv= nAEpExactDiv;
    r->cfInit = nAEpInit;
    r->cfSize  = nAEpSize;
    r->cfInt  = nAEpInt;
#ifdef HAVE_RINGS
    //r->cfDivComp = NULL; // only for ring stuff
    //r->cfIsUnit = NULL; // only for ring stuff
    //r->cfGetUnit = NULL; // only for ring stuff
    //r->cfExtGcd = NULL; // only for ring stuff
    // r->cfDivBy = NULL; // only for ring stuff
#endif
    r->cfInpNeg   = nAEpNeg;
    r->cfInvers= NULL;
    //r->cfCopy  = ndCopy;
    //r->cfRePart = ndCopy;
    //r->cfImPart = ndReturn0;
    r->cfWriteLong = nAEpWriteLong;
    r->cfRead = nAEpRead;
    //r->cfNormalize=ndNormalize;
    r->cfGreater = nAEpGreater;
    r->cfEqual = nAEpEqual;
    r->cfIsZero = nAEpIsZero;
    r->cfIsOne = nAEpIsOne;
    r->cfIsMOne = nAEpIsOne;
    r->cfGreaterZero = nAEpGreaterZero;
    r->cfPower = nAEpPower; // ZU BEARBEITEN
    r->cfGetDenom = nAEpGetDenom;
    r->cfGetNumerator = nAEpGetNumerator;
    r->cfGcd  = nAEpGcd;
    r->cfLcm  = nAEpLcm; // ZU BEARBEITEN
    r->cfDelete= nAEpDelete;
    r->cfSetMap = npSetMap;
    r->cfInpMult=nAEpInpMult; //????
    r->cfCoeffWrite=nAEpCoeffWrite; //????


    // the variables:
    r->nNULL = (number) 0;
    //r->type = n_AE;
    r->ch = c;
    r->has_simple_Alloc=TRUE;
    r->has_simple_Inverse=TRUE;
    return FALSE;
}

