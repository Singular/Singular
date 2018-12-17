/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: Implementation of the Groebner walk
*/

// define if the Buchberger alg should be used
//   to compute a reduced GB of a omega-homogenoues ideal
// default: we use the hilbert driven algorithm.
#define BUCHBERGER_ALG  //we use the improved Buchberger alg.

//#define UPPER_BOUND //for the original "Tran" algorithm
//#define REPRESENTATION_OF_SIGMA //if one perturbs sigma in Tran

//#define TEST_OVERFLOW

#define CHECK_IDEAL_MWALK //to print intermediate results

//#define NEXT_VECTORS_CC
//#define PRINT_VECTORS //to print weight vectors

#define INVEPS_SMALL_IN_FRACTAL  //to choose the small invers of epsilon
#define INVEPS_SMALL_IN_MPERTVECTOR  //to choose the small invers of epsilon
#define INVEPS_SMALL_IN_TRAN  //to choose the small invers of epsilon

#define FIRST_STEP_FRACTAL // to define the first step of the fractal
#define MSTDCC_FRACTAL // apply Buchberger alg to compute a red GB, if tau doesn't stay in the correct cone

//#define TIME_TEST // print the used time of each subroutine
//#define ENDWALKS //print the size of the last omega-homogenoues Groebner basis

/* includes */

#include "kernel/mod2.h"
#include "misc/intvec.h"
#include "Singular/cntrlc.h"
#include "misc/options.h"
#include "omalloc/omalloc.h"
#include "Singular/ipshell.h"
#include "Singular/ipconv.h"
#include "coeffs/ffields.h"
#include "coeffs/coeffs.h"
#include "Singular/subexpr.h"
#include "polys/templates/p_Procs.h"

#include "polys/monomials/maps.h"

/* include Hilbert-function */
#include "kernel/combinatorics/stairc.h"

/** kstd2.cc */
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/khstd.h"

#include "Singular/walk.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "Singular/ipid.h"
#include "Singular/tok.h"
#include "coeffs/numbers.h"
#include "Singular/ipid.h"
#include "polys/monomials/ring.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/matpol.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/GBEngine/syz.h"
#include "Singular/lists.h"
#include "polys/prCopy.h"
#include "polys/monomials/ring.h"
//#include "polys/ext_fields/longalg.h"
#include "polys/clapsing.h"

#include "coeffs/mpr_complex.h"

#include <cmath>
#include "misc/mylimits.h"

int nstep;

extern BOOLEAN ErrorCheck();

extern BOOLEAN pSetm_error;

void Set_Error( BOOLEAN f) { pSetm_error=f; }

BOOLEAN Overflow_Error =  FALSE;

#ifdef TIME_TEST
clock_t xtif, xtstd, xtlift, xtred, xtnw;
clock_t xftostd, xtextra, xftinput, to;
#endif

/****************************
 * utilities for TSet, LSet *
 ****************************/
inline static intset initec (int maxnr)
{
  return (intset)omAlloc(maxnr*sizeof(int));
}

inline static unsigned long* initsevS (int maxnr)
{
  return (unsigned long*)omAlloc0(maxnr*sizeof(unsigned long));
}
inline static int* initS_2_R (int maxnr)
{
  return (int*)omAlloc0(maxnr*sizeof(int));
}

/************************************
 * construct the set s from F u {P} *
 ************************************/
// unused
/*
static void initSSpecialCC (ideal F, ideal Q, ideal P,kStrategy strat)
{
  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else i=setmaxT;

  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
  strat->S_2_R=initS_2_R(i);
  strat->fromQ=NULL;
  strat->Shdl=idInit(i,F->rank);
  strat->S=strat->Shdl->m;

  // - put polys into S -
  if (Q!=NULL)
  {
    strat->fromQ=initec(i);
    memset(strat->fromQ,0,i*sizeof(int));
    for (i=0; i<IDELEMS(Q); i++)
    {
      if (Q->m[i]!=NULL)
      {
        LObject h;
        h.p = pCopy(Q->m[i]);
        //if (TEST_OPT_INTSTRATEGY)
        //{
        //  h.pCleardenom(); // also does a Content
        //}
        //else
        //{
        //  h.pNorm();
        //}
        strat->initEcart(&h);
        if (rHasLocalOrMixedOrdering(currRing))
        {
          deleteHC(&h,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
          h.SetpFDeg();
          strat->enterS(h,pos,strat, strat->tl+1);
          enterT(h, strat);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  //- put polys into S -
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      LObject h;
      h.p = pCopy(F->m[i]);
      if (rHasGlobalOrdering(currRing))
      {
        //h.p=redtailBba(h.p,strat->sl,strat);
        h.p=redtailBba(h.p,strat->sl,strat);
      }
      else
      {
        deleteHC(&h,strat);
      }
      strat->initEcart(&h);
      if (h.p!=NULL)
      {
        if (strat->sl==-1)
          pos =0;
        else
          pos = posInS(strat,strat->sl,h.p,h.ecart);
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,pos,strat, strat->tl+1);
        h.length = pLength(h.p);
        h.SetpFDeg();
        enterT(h,strat);
      }
    }
  }
#ifdef INITSSPECIAL
  for (i=0; i<IDELEMS(P); i++)
  {
    if (P->m[i]!=NULL)
    {
      LObject h;
      h.p=pCopy(P->m[i]);
      strat->initEcart(&h);
      h.length = pLength(h.p);
      if (TEST_OPT_INTSTRATEGY)
      {
        h.pCleardenom();
      }
      else
      {
        h.pNorm();
      }
      if(strat->sl>=0)
      {
        if (rHasGlobalOrdering(currRing))
        {
          h.p=redBba(h.p,strat->sl,strat);
          if (h.p!=NULL)
            h.p=redtailBba(h.p,strat->sl,strat);
        }
        else
        {
          h.p=redMora(h.p,strat->sl,strat);
          strat->initEcart(&h);
        }
        if(h.p!=NULL)
        {
          if (TEST_OPT_INTSTRATEGY)
          {
            h.pCleardenom();
          }
          else
          {
            h.is_normalized = 0;
            h.pNorm();
          }
          h.sev = pGetShortExpVector(h.p);
          h.SetpFDeg();
          pos = posInS(strat->S,strat->sl,h.p,h.ecart);
          enterpairsSpecial(h.p,strat->sl,h.ecart,pos,strat,strat->tl+1);
          strat->enterS(h,pos,strat, strat->tl+1);
          enterT(h,strat);
        }
      }
      else
      {
        h.sev = pGetShortExpVector(h.p);
        h.SetpFDeg();
        strat->enterS(h,0,strat, strat->tl+1);
        enterT(h,strat);
      }
    }
  }
#endif
}
*/

/*****************
 *interreduce F  *
 *****************/
static ideal kInterRedCC(ideal F, ideal Q)
{
  int j;
  kStrategy strat = new skStrategy;
/*
  if (TEST_OPT_PROT)
  {
    writeTime("start InterRed:");
    mflush();
  }
  strat->syzComp     = 0;
*/
  strat->kHEdgeFound = (currRing->ppNoether) != NULL;
  strat->kNoether=pCopy((currRing->ppNoether));
  strat->ak = id_RankFreeModule(F, currRing);
  initBuchMoraCrit(strat);
  strat->NotUsedAxis = (BOOLEAN *)omAlloc((currRing->N+1)*sizeof(BOOLEAN));
  for(j=currRing->N; j>0; j--)
  {
    strat->NotUsedAxis[j] = TRUE;
  }
  strat->enterS      = enterSBba;
  strat->posInT      = posInT0;
  strat->initEcart   = initEcartNormal;
  strat->sl   = -1;
  strat->tl          = -1;
  strat->tmax        = setmaxT;
  strat->T           = initT();
  strat->R           = initR();
  strat->sevT        = initsevT();
  if(rHasLocalOrMixedOrdering(currRing))
  {
    strat->honey = TRUE;
  }

  //initSCC(F,Q,strat);
  initS(F,Q,strat);

  /*
  timetmp=clock();//22.01.02
  initSSpecialCC(F,Q,NULL,strat);
  tininitS=tininitS+clock()-timetmp;//22.01.02
  */
  if(TEST_OPT_REDSB)
  {
    strat->noTailReduction=FALSE;
  }
  updateS(TRUE,strat);

  if(TEST_OPT_REDSB && TEST_OPT_INTSTRATEGY)
  {
    completeReduce(strat);
  }
  if(strat->kHEdge!=NULL) pLmFree(&strat->kHEdge);
  omFreeSize((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omFreeSize((ADDRESS)strat->NotUsedAxis,(currRing->N+1)*sizeof(BOOLEAN));
  omfree(strat->sevT);
  omfree(strat->S_2_R);
  omfree(strat->R);

  if(strat->fromQ)
  {
    for(j=0; j<IDELEMS(strat->Shdl); j++)
    {
      if(strat->fromQ[j])
      {
        pDelete(&strat->Shdl->m[j]);
      }
    }
    omFreeSize((ADDRESS)strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
    strat->fromQ = NULL;
  }
/*
  if (TEST_OPT_PROT)
  {
    writeTime("end Interred:");
    mflush();
  }
*/
  ideal shdl=strat->Shdl;
  idSkipZeroes(shdl);
  delete(strat);

  return shdl;
}

#ifdef TIME_TEST
static void TimeString(clock_t tinput, clock_t tostd, clock_t tif,clock_t tstd,
                       clock_t tlf,clock_t tred, clock_t tnw, int step)
{
  double totm = ((double) (clock() - tinput))/1000000;
  double ostd,mostd, mif, mstd, mlf, mred, mnw, mxif,mxstd,mxlf,mxred,mxnw,tot;
  // double mextra
  Print("\n// total time = %.2f sec", totm);
  Print("\n// tostd = %.2f sec = %.2f", ostd=((double) tostd)/1000000,
        mostd=((((double) tostd)/1000000)/totm)*100);
  Print("\n// tif   = %.2f sec = %.2f", ((double) tif)/1000000,
        mif=((((double) tif)/1000000)/totm)*100);
  Print("\n// std   = %.2f sec = %.2f", ((double) tstd)/1000000,
        mstd=((((double) tstd)/1000000)/totm)*100);
  Print("\n// lift  = %.2f sec = %.2f", ((double) tlf)/1000000,
        mlf=((((double) tlf)/1000000)/totm)*100);
  Print("\n// ired  = %.2f sec = %.2f", ((double) tred)/1000000,
        mred=((((double) tred)/1000000)/totm)*100);
  Print("\n// nextw = %.2f sec = %.2f", ((double) tnw)/1000000,
        mnw=((((double) tnw)/1000000)/totm)*100);
  PrintS("\n Time for the last step:");
  Print("\n// xinfo = %.2f sec = %.2f", ((double) xtif)/1000000,
        mxif=((((double) xtif)/1000000)/totm)*100);
  Print("\n// xstd  = %.2f sec = %.2f", ((double) xtstd)/1000000,
        mxstd=((((double) xtstd)/1000000)/totm)*100);
  Print("\n// xlift = %.2f sec = %.2f", ((double) xtlift)/1000000,
        mxlf=((((double) xtlift)/1000000)/totm)*100);
  Print("\n// xired = %.2f sec = %.2f", ((double) xtred)/1000000,
        mxred=((((double) xtred)/1000000)/totm)*100);
  Print("\n// xnextw= %.2f sec = %.2f", ((double) xtnw)/1000000,
        mxnw=((((double) xtnw)/1000000)/totm)*100);

  tot=mostd+mif+mstd+mlf+mred+mnw+mxif+mxstd+mxlf+mxred+mxnw;
  double res = (double) 100 - tot;
  Print("\n// &%d&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f&%.2f(%.2f)\\ \\",
        step, ostd, totm, mostd,mif,mstd,mlf,mred,mnw,mxif,mxstd,mxlf,mxred,mxnw,tot,res,
        ((((double) xtextra)/1000000)/totm)*100);
}

static void TimeStringFractal(clock_t tinput, clock_t tostd, clock_t tif,clock_t tstd,
                       clock_t textra, clock_t tlf,clock_t tred, clock_t tnw)
{

  double totm = ((double) (clock() - tinput))/1000000;
  double ostd, mostd, mif, mstd, mextra, mlf, mred, mnw, tot, res;
  Print("\n// total time = %.2f sec", totm);
  Print("\n// tostd = %.2f sec = %.2f", ostd=((double) tostd)/1000000,
        mostd=((((double) tostd)/1000000)/totm)*100);
  Print("\n// tif   = %.2f sec = %.2f", ((double) tif)/1000000,
        mif=((((double) tif)/1000000)/totm)*100);
  Print("\n// std   = %.2f sec = %.2f", ((double) tstd)/1000000,
        mstd=((((double) tstd)/1000000)/totm)*100);
  Print("\n// xstd  = %.2f sec = %.2f", ((double) textra)/1000000,
        mextra=((((double) textra)/1000000)/totm)*100);
  Print("\n// lift  = %.2f sec = %.2f", ((double) tlf)/1000000,
        mlf=((((double) tlf)/1000000)/totm)*100);
  Print("\n// ired  = %.2f sec = %.2f", ((double) tred)/1000000,
        mred=((((double) tred)/1000000)/totm)*100);
  Print("\n// nextw = %.2f sec = %.2f", ((double) tnw)/1000000,
        mnw=((((double) tnw)/1000000)/totm)*100);
  tot = mostd+mif+mstd+mextra+mlf+mred+mnw;
  res = (double) 100.00-tot;
  Print("\n// &%.2f &%.2f&%.2f &%.2f &%.2f &%.2f &%.2f &%.2f &%.2f&%.2f&%.2f\\ \\ ",
        ostd,totm,mostd,mif,mstd,mextra,mlf,mred,mnw,tot,res);
}
#endif

#ifdef CHECK_IDEAL_MWALK
static void idString(ideal L, const char* st)
{
  int i, nL = IDELEMS(L);

  Print("\n//  ideal %s =  ", st);
  for(i=0; i<nL-1; i++)
  {
    Print(" %s, ", pString(L->m[i]));
  }
  Print(" %s;", pString(L->m[nL-1]));
}
#endif
/*
#if defined(CHECK_IDEAL_MWALK) || defined(ENDWALKS)
static void headidString(ideal L, char* st)
{
  int i, nL = IDELEMS(L);

  Print("\n//  ideal %s =  ", st);
  for(i=0; i<nL-1; i++)
  {
    Print(" %s, ", pString(pHead(L->m[i])));
  }
  Print(" %s;", pString(pHead(L->m[nL-1])));
}
#endif

#if defined(CHECK_IDEAL_MWALK) || defined(ENDWALKS)
static void idElements(ideal L, char* st)
{
  int i, nL = IDELEMS(L);
  int *K=(int *)omAlloc(nL*sizeof(int));

  Print("\n//  #monoms of %s =  ", st);
  for(i=0; i<nL; i++)
  {
    K[i] = pLength(L->m[i]);
  }
  int j, nsame;
  // int  nk=0;
  for(i=0; i<nL; i++)
  {
    if(K[i]!=0)
    {
      nsame = 1;
      for(j=i+1; j<nL; j++)
      {
        if(K[j]==K[i])
        {
          nsame ++;
          K[j]=0;
        }
      }
      if(nsame == 1)
      {
        Print("%d, ",K[i]);
      }
      else
      {
        Print("%d[%d], ", K[i], nsame);
      }
    }
  }
  omFree(K);
}
#endif
*/

static void ivString(intvec* iv, const char* ch)
{
  int nV = iv->length()-1;
  Print("\n// intvec %s =  ", ch);

  for(int i=0; i<nV; i++)
  {
    Print("%d, ", (*iv)[i]);
  }
  Print("%d;", (*iv)[nV]);
}

#ifdef PRINT_VECTORS
static void MivString(intvec* iva, intvec* ivb, intvec* ivc)
{
  int nV = iva->length()-1;
  int i;
  PrintS("\n//  (");
  for(i=0; i<nV; i++)
  {
    Print("%d, ", (*iva)[i]);
  }
  Print("%d) ==> (", (*iva)[nV]);
  for(i=0; i<nV; i++)
  {
    Print("%d, ", (*ivb)[i]);
  }
  Print("%d) := (", (*ivb)[nV]);

  for(i=0; i<nV; i++)
  {
    Print("%d, ", (*ivc)[i]);
  }
  Print("%d)", (*ivc)[nV]);
}
#endif

/********************************************************************
 * returns gcd of integers a and b                                  *
 ********************************************************************/
static inline long gcd(const long a, const long b)
{
  long r, p0 = a, p1 = b;
  //assume(p0 >= 0 && p1 >= 0);
  if(p0 < 0)
  {
    p0 = -p0;
  }
  if(p1 < 0)
  {
    p1 = -p1;
  }
  while(p1 != 0)
  {
    r = p0 % p1;
    p0 = p1;
    p1 = r;
  }
  return p0;
}

/*****************************************************************************
 * compute the gcd of the entries of the vectors curr_weight and diff_weight *
 *****************************************************************************/
/* unused:
static int simplify_gcd(intvec* curr_weight, intvec* diff_weight)
{
  int j;
  int nRing = currRing->N;
  int gcd_tmp = (*curr_weight)[0];
  for (j=1; j<nRing; j++)
  {
    gcd_tmp = gcd(gcd_tmp, (*curr_weight)[j]);
    if(gcd_tmp == 1)
    {
      break;
    }
  }
  if(gcd_tmp != 1)
  {
    for (j=0; j<nRing; j++)
    {
    gcd_tmp = gcd(gcd_tmp, (*diff_weight)[j]);
    if(gcd_tmp == 1)
      {
        break;
      }
    }
  }
  return gcd_tmp;
}
*/

/*********************************************
 * cancel gcd of integers zaehler and nenner *
 *********************************************/
static void cancel(mpz_t zaehler, mpz_t nenner)
{
//  assume(zaehler >= 0 && nenner > 0);
  mpz_t g;
  mpz_init(g);
  mpz_gcd(g, zaehler, nenner);

  mpz_div(zaehler , zaehler, g);
  mpz_div(nenner ,  nenner, g);

  mpz_clear(g);
}

//unused
#if 0
static int isVectorNeg(intvec* omega)
{
  int i;

  for(i=omega->length(); i>=0; i--)
  {
    if((*omega)[i]<0)
    {
      return 1;
    }
  }
  return 0;
}
#endif

/********************************************************************
 * compute a weight degree of a monomial p w.r.t. a weight_vector   *
 ********************************************************************/
static inline int MLmWeightedDegree(const poly p, intvec* weight)
{
  /* 2147483647 is max. integer representation in SINGULAR */
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  int i, wgrad;

  mpz_t zmul;
  mpz_init(zmul);
  mpz_t zvec;
  mpz_init(zvec);
  mpz_t zsum;
  mpz_init(zsum);

  for (i=currRing->N; i>0; i--)
  {
    mpz_set_si(zvec, (*weight)[i-1]);
    mpz_mul_ui(zmul, zvec, pGetExp(p, i));
    mpz_add(zsum, zsum, zmul);
  }

  wgrad = mpz_get_ui(zsum);

  if(mpz_cmp(zsum, sing_int)>0)
  {
    if(Overflow_Error ==  FALSE)
    {
      PrintLn();
      PrintS("\n// ** OVERFLOW in \"MwalkInitialForm\": ");
      mpz_out_str( stdout, 10, zsum);
      PrintS(" is greater than 2147483647 (max. integer representation)");
      Overflow_Error = TRUE;
    }
  }

  mpz_clear(zmul);
  mpz_clear(zvec);
  mpz_clear(zsum);
  mpz_clear(sing_int);

  return wgrad;
}

/********************************************************************
 * compute a weight degree of a polynomial p w.r.t. a weight_vector *
 ********************************************************************/
static inline int MwalkWeightDegree(poly p, intvec* weight_vector)
{
  assume(weight_vector->length() >= currRing->N);
  int max = 0, maxtemp;

  while(p != NULL)
  {
    maxtemp = MLmWeightedDegree(p, weight_vector);
    pIter(p);

    if (maxtemp > max)
    {
      max = maxtemp;
    }
  }
  return max;
}


/********************************************************************
 * compute a weight degree of a monomial p w.r.t. a weight_vector   *
 ********************************************************************/
static  void  MLmWeightedDegree_gmp(mpz_t result, const poly p, intvec* weight)
{
  /* 2147483647 is max. integer representation in SINGULAR */
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  int i;

  mpz_t zmul;
  mpz_init(zmul);
  mpz_t zvec;
  mpz_init(zvec);
  mpz_t ztmp;
  mpz_init(ztmp);

  for (i=currRing->N; i>0; i--)
  {
    mpz_set_si(zvec, (*weight)[i-1]);
    mpz_mul_ui(zmul, zvec, pGetExp(p, i));
    mpz_add(ztmp, ztmp, zmul);
  }
  mpz_init_set(result, ztmp);
  mpz_clear(ztmp);
  mpz_clear(sing_int);
  mpz_clear(zvec);
  mpz_clear(zmul);
}


/*****************************************************************************
 * return an initial form of the polynom g w.r.t. a weight vector curr_weight *
 *****************************************************************************/
static poly MpolyInitialForm(poly g, intvec* curr_weight)
{
  if(g == NULL)
  {
    return NULL;
  }
  mpz_t max; mpz_init(max);
  mpz_t maxtmp; mpz_init(maxtmp);

  poly hg, in_w_g = NULL;

  while(g != NULL)
  {
    hg = g;
    pIter(g);
    MLmWeightedDegree_gmp(maxtmp, hg, curr_weight);

    if(mpz_cmp(maxtmp, max)>0)
    {
      mpz_set(max, maxtmp);
      if (in_w_g!=NULL) pDelete(&in_w_g);
      in_w_g = pHead(hg);
    }
    else
    {
      if(mpz_cmp(maxtmp, max)==0)
      {
        in_w_g = pAdd(in_w_g, pHead(hg));
      }
    }
  }
  mpz_clear(maxtmp);
  mpz_clear(max);
  return in_w_g;
}

/************************************************************************
 * compute the initial form of an ideal <G> w.r.t. a weight vector iva  *
 ************************************************************************/
ideal MwalkInitialForm(ideal G, intvec* ivw)
{
  BOOLEAN nError =  Overflow_Error;
  Overflow_Error = FALSE;

  int i, nG = IDELEMS(G);
  ideal Gomega = idInit(nG, 1);

  for(i=nG-1; i>=0; i--)
  {
    Gomega->m[i] = MpolyInitialForm(G->m[i], ivw);
  }
  if(Overflow_Error == FALSE)
  {
    Overflow_Error = nError;
  }
  return Gomega;
}

/************************************************************************
 * test whether the weight vector iv is in the cone of the ideal G      *
 *     i.e. test whether in(in_w(g)) = in(g) for all g in G             *
 ************************************************************************/

static int test_w_in_ConeCC(ideal G, intvec* iv)
{
  if(G->m[0] == NULL)
  {
    PrintS("//** the result may be WRONG, i.e. 0!!\n");
    return 0;
  }

  BOOLEAN nError =  Overflow_Error;
  Overflow_Error = FALSE;

  int i, nG = IDELEMS(G);
  poly mi, gi;

  for(i=nG-1; i>=0; i--)
  {
    mi = MpolyInitialForm(G->m[i], iv);
    //Print("\n **// test_w_in_ConeCC: lm(initial)= %s \n",pString(mi));
    gi = G->m[i];
    //Print("\n **// test_w_in_ConeCC: lm(ideal)= %s \n",pString(gi));
    if(mi == NULL)
    {
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = nError;
      }
      return 0;
    }
    if(!pLmEqual(mi, gi))
    {
      pDelete(&mi);
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = nError;
      }
      return 0;
    }
    pDelete(&mi);
  }

  if(Overflow_Error == FALSE)
  {
    Overflow_Error = nError;
  }
  return 1;
}

/***************************************************
 * compute a least common multiple of two integers *
 ***************************************************/
static inline long Mlcm(long &i1, long &i2)
{
  long temp = gcd(i1, i2);
  return ((i1 / temp)* i2);
}


/***************************************************
 * return  the dot product of two intvecs a and b  *
 ***************************************************/
static inline long  MivDotProduct(intvec* a, intvec* b)
{
  assume( a->length() ==  b->length());
  int i, n = a->length();
  long result = 0;

  for(i=n-1; i>=0; i--)
    {
    result += (*a)[i] * (*b)[i];
    }
  return result;
}

/*****************************************************
 * Substract two given intvecs componentwise         *
 *****************************************************/
static intvec* MivSub(intvec* a, intvec* b)
{
  assume( a->length() ==  b->length());
  int i, n = a->length();
  intvec* result = new intvec(n);

  for(i=n-1; i>=0; i--)
  {
    (*result)[i] = (*a)[i] - (*b)[i];
  }
  return result;
}

/*****************************************************
 * return the "intvec" lead exponent of a polynomial *
 *****************************************************/
static intvec* MExpPol(poly f)
{
  int i, nR = currRing->N;
  intvec* result = new intvec(nR);

  for(i=nR-1; i>=0; i--)
  {
    (*result)[i] = pGetExp(f,i+1);
  }
  return result;
}

/*****************************************************
 * Compare two given intvecs and return 1, if they   *
 * are the same, otherwise 0                         *
 *****************************************************/
int MivSame(intvec* u , intvec* v)
{
  assume(u->length() == v->length());

  int i, niv = u->length();

  for (i=0; i<niv; i++)
  {
    if ((*u)[i] != (*v)[i])
    {
      return 0;
    }
  }
  return 1;
}

/******************************************************
 * Compare 3 given intvecs and return 0, if the first *
 * and the second are the same. Return 1, if the      *
 * the second and the third are the same, otherwise 2 *
 ******************************************************/
int M3ivSame(intvec* temp, intvec* u , intvec* v)
{
  assume(temp->length() == u->length() && u->length() == v->length());

  if((MivSame(temp, u)) == 1)
  {
    return 0;
  }
  if((MivSame(temp, v)) == 1)
  {
    return 1;
  }
  return 2;
}

/*****************************************************
 * compute a Groebner basis of an ideal              *
 *****************************************************/
static ideal MstdCC(ideal G)
{
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);
  si_opt_1|=(Sy_bit(OPT_REDTAIL)|Sy_bit(OPT_REDSB));
  ideal G1 = kStd(G, NULL, testHomog, NULL);
  SI_RESTORE_OPT(save1,save2);

  idSkipZeroes(G1);
  return G1;
}

/*****************************************************
 * compute a Groebner basis of an homogeneous ideal  *
 *****************************************************/
static ideal MstdhomCC(ideal G)
{
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);
  si_opt_1|=(Sy_bit(OPT_REDTAIL)|Sy_bit(OPT_REDSB));
  ideal G1 = kStd(G, NULL, isHomog, NULL);
  SI_RESTORE_OPT(save1,save2);

  idSkipZeroes(G1);
  return G1;
}


/*****************************************************************************
* create a weight matrix order as intvec of an extra weight vector (a(iv),lp)*
******************************************************************************/
intvec* MivMatrixOrder(intvec* iv)
{
  int i, nR = iv->length();

  intvec* ivm = new intvec(nR*nR);

  for(i=0; i<nR; i++)
  {
    (*ivm)[i] = (*iv)[i];
  }
  for(i=1; i<nR; i++)
  {
    (*ivm)[i*nR+i-1] = 1;
  }
  return ivm;
}

/*********************************************************************************
* create a weight matrix order as intvec of an extra weight vector (a(iv),M(iw)) *
**********************************************************************************/
intvec* MivMatrixOrderRefine(intvec* iv, intvec* iw)
{
  assume((iv->length())*(iv->length()) == iw->length());
  int i,j, nR = iv->length();

  intvec* ivm = new intvec(nR*nR);

  for(i=0; i<nR; i++)
  {
    (*ivm)[i] = (*iv)[i];
  }
  for(i=1; i<nR; i++)
  {
    for(j=0; j<nR; j++)
    {
      (*ivm)[j+i*nR] = (*iw)[j+i*nR];
    }
  }
  return ivm;
}

/*******************************
 * return intvec = (1, ..., 1) *
 *******************************/
intvec* Mivdp(int nR)
{
  int i;
  intvec* ivm = new intvec(nR);

  for(i=nR-1; i>=0; i--)
  {
    (*ivm)[i] = 1;
  }
  return ivm;
}

/**********************************
 * return intvvec = (1,0, ..., 0) *
 **********************************/
intvec* Mivlp(int nR)
{
  intvec* ivm = new intvec(nR);
  (*ivm)[0] = 1;

  return ivm;
}

//unused
/*****************************************************************************
 * print the max total degree and the max coefficient of G                   *
 *****************************************************************************/
/*
static void checkComplexity(ideal G, char* cG)
{
  int nV = currRing->N;
  int nG = IDELEMS(G);
  intvec* ivUnit = Mivdp(nV);
  int i, tmpdeg, maxdeg=0;
  number tmpcoeff , maxcoeff=nInit(0);
  poly p;
  for(i=nG-1; i>=0; i--)
  {
    tmpdeg = MwalkWeightDegree(G->m[i], ivUnit);
    if(tmpdeg > maxdeg )
    {
      maxdeg = tmpdeg;
    }
  }

  for(i=nG-1; i>=0; i--)
  {
    p = pCopy(G->m[i]);
    while(p != NULL)
    {
      //tmpcoeff = pGetCoeff(pHead(p));
      tmpcoeff = pGetCoeff(p);
      if(nGreater(tmpcoeff,maxcoeff))
      {
         maxcoeff = nCopy(tmpcoeff);
      }
      pIter(p);
    }
    pDelete(&p);
  }
  p = pNSet(maxcoeff);
  char* pStr = pString(p);
  delete ivUnit;
  Print("// max total degree of %s = %d\n",cG, maxdeg);
  Print("// max coefficient of %s  = %s", cG, pStr);//ing(p));
  Print(" which consists of %d digits", (int)strlen(pStr));
  PrintLn();
}
*/

/*****************************************************************************
* If target_ord = intmat(A1, ..., An) then calculate the perturbation        *
* vectors                                                                    *
*   tau_p_dep = inveps^(p_deg-1)*A1 + inveps^(p_deg-2)*A2 +... + A_p_deg     *
* where                                                                      *
*      inveps > totaldegree(G)*(max(A2)+...+max(A_p_deg))                    *
* intmat target_ord is an integer order matrix of the monomial ordering of   *
* basering.                                                                  *
* This programm computes a perturbated vector with a p_deg perturbation      *
* degree which smaller than the numbers of variables                         *
******************************************************************************/
intvec* MPertVectors(ideal G, intvec* ivtarget, int pdeg)
{
  // ivtarget is a matrix order of a degree reverse lex. order
  int nV = currRing->N;
  //assume(pdeg <= nV && pdeg >= 0);

  int i, j, nG = IDELEMS(G);
  intvec* v_null =  new intvec(nV);

  // Check that the perturbed degree is valid
  if(pdeg > nV || pdeg <= 0)
  {
    WerrorS("//** The perturbed degree is wrong!!");
    return v_null;
  }
  delete v_null;

  if(pdeg == 1)
  {
    return ivtarget;
  }
  mpz_t *pert_vector = (mpz_t*)omAlloc(nV*sizeof(mpz_t));
  mpz_t *pert_vector1 = (mpz_t*)omAlloc(nV*sizeof(mpz_t));

  for(i=0; i<nV; i++)
  {
    mpz_init_set_si(pert_vector[i], (*ivtarget)[i]);
    mpz_init_set_si(pert_vector1[i], (*ivtarget)[i]);
  }
  // Calculate max1 = Max(A2)+Max(A3)+...+Max(Apdeg),
  // where the Ai are the i-te rows of the matrix target_ord.
  int ntemp, maxAi, maxA=0;
  for(i=1; i<pdeg; i++)
  {
    maxAi = (*ivtarget)[i*nV];
    if(maxAi<0)
    {
      maxAi = -maxAi;
    }
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp < 0)
      {
        ntemp = -ntemp;
      }
      if(ntemp > maxAi)
      {
        maxAi = ntemp;
      }
    }
    maxA += maxAi;
  }

  // Calculate inveps = 1/eps, where 1/eps > totaldeg(p)*max1 for all p in G.

  intvec* ivUnit = Mivdp(nV);

  mpz_t tot_deg; mpz_init(tot_deg);
  mpz_t maxdeg; mpz_init(maxdeg);
  mpz_t inveps; mpz_init(inveps);


  for(i=nG-1; i>=0; i--)
  {
     mpz_set_ui(maxdeg, MwalkWeightDegree(G->m[i], ivUnit));
     if (mpz_cmp(maxdeg,  tot_deg) > 0 )
     {
       mpz_set(tot_deg, maxdeg);
     }
  }

  delete ivUnit;
  mpz_mul_ui(inveps, tot_deg, maxA);
  mpz_add_ui(inveps, inveps, 1);


  // takes  "small" inveps
#ifdef INVEPS_SMALL_IN_MPERTVECTOR
  if(mpz_cmp_ui(inveps, pdeg)>0 && pdeg > 3)
  {
    //  Print("\n// choose the\"small\" inverse epsilon := %d / %d = ", mpz_get_si(inveps), pdeg);
    mpz_fdiv_q_ui(inveps, inveps, pdeg);
    // mpz_out_str(stdout, 10, inveps);
  }
#else
  // PrintS("\n// the \"big\" inverse epsilon: ");
  mpz_out_str(stdout, 10, inveps);
#endif

  // pert(A1) = inveps^(pdeg-1)*A1 + inveps^(pdeg-2)*A2+...+A_pdeg,
  // pert_vector := A1
  for( i=1; i < pdeg; i++ )
  {
    for(j=0; j<nV; j++)
    {
      mpz_mul(pert_vector[j], pert_vector[j], inveps);
      if((*ivtarget)[i*nV+j]<0)
      {
        mpz_sub_ui(pert_vector[j], pert_vector[j],-(*ivtarget)[i*nV+j]);
      }
      else
      {
        mpz_add_ui(pert_vector[j], pert_vector[j],(*ivtarget)[i*nV+j]);
      }
    }
  }

  // 2147483647 is max. integer representation in SINGULAR
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  mpz_t check_int;
  mpz_init_set_ui(check_int,  100000);

  mpz_t ztemp;
  mpz_init(ztemp);
  mpz_set(ztemp, pert_vector[0]);
  for(i=1; i<nV; i++)
  {
    mpz_gcd(ztemp, ztemp, pert_vector[i]);
    if(mpz_cmp_si(ztemp, 1)  == 0)
    {
      break;
    }
  }
  if(mpz_cmp_si(ztemp, 1) != 0)
  {
    for(i=0; i<nV; i++)
    {
      mpz_divexact(pert_vector[i], pert_vector[i], ztemp);
    }
  }

  for(i=0; i<nV; i++)
  {
    if(mpz_cmp(pert_vector[i], check_int)>=0)
    {
      for(j=0; j<nV; j++)
      {
        mpz_fdiv_q_ui(pert_vector1[j], pert_vector[j], 100);
      }
    }
  }

  intvec* result = new intvec(nV);

  int ntrue=0;

  for(i=0; i<nV; i++)
  {
    (*result)[i] = mpz_get_si(pert_vector1[i]);
    if(mpz_cmp(pert_vector1[i], sing_int)>=0)
    {
      ntrue++;
    }
  }
  if(ntrue > 0 || test_w_in_ConeCC(G,result)==0)
  {
    ntrue=0;
    for(i=0; i<nV; i++)
    {
      (*result)[i] = mpz_get_si(pert_vector[i]);
      if(mpz_cmp(pert_vector[i], sing_int)>=0)
      {
        ntrue++;
        if(Overflow_Error == FALSE)
        {
          Overflow_Error = TRUE;
          PrintS("\n// ** OVERFLOW in \"MPertvectors\": ");
          mpz_out_str( stdout, 10, pert_vector[i]);
          PrintS(" is greater than 2147483647 (max. integer representation)");
          Print("\n//  So vector[%d] := %d is wrong!!", i+1, (*result)[i]);
        }
      }
    }

    if(Overflow_Error == TRUE)
    {
      ivString(result, "pert_vector");
      Print("\n// %d element(s) of it is overflow!!", ntrue);
    }
  }

  mpz_clear(ztemp);
  mpz_clear(sing_int);
  mpz_clear(check_int);
  omFree(pert_vector);
  omFree(pert_vector1);
  mpz_clear(tot_deg);
  mpz_clear(maxdeg);
  mpz_clear(inveps);

  rComplete(currRing);
  for(j=0; j<IDELEMS(G); j++)
  {
    poly p=G->m[j];
    while(p!=NULL)
    {
      p_Setm(p,currRing); pIter(p);
    }
  }
  return result;
}

/*****************************************************************************
 * The following procedure returns                                           *
 *     Pert(A1) = 1/eps^(pdeg-1)*A_1 + 1/eps^(pdeg-2)*A_2+...+A_pdeg,        *
 * where the A_i are the i-th rows of the matrix target_ord and              *
 *     1/eps > deg(p)*(max(A_2) + max(A_3)+...+max(A_pdeg))                  *
 *****************************************************************************/
intvec* MPertVectorslp(ideal G, intvec* ivtarget, int pdeg)
{
  // ivtarget is a matrix order of the lex. order
  int nV = currRing->N;
  //assume(pdeg <= nV && pdeg >= 0);

  int i, j, nG = IDELEMS(G);
  intvec* pert_vector =  new intvec(nV);

  //Checking that the perturbated degree is valid
  if(pdeg > nV || pdeg <= 0)
  {
    WerrorS("//** The perturbed degree is wrong!!");
    return pert_vector;
  }
  for(i=0; i<nV; i++)
  {
    (*pert_vector)[i]=(*ivtarget)[i];
  }
  if(pdeg == 1)
  {
    return pert_vector;
  }
  // Calculate max1 = Max(A2)+Max(A3)+...+Max(Apdeg),
  // where the Ai are the i-te rows of the matrix target_ord.
  int ntemp, maxAi, maxA=0;
  for(i=1; i<pdeg; i++)
  {
    maxAi = (*ivtarget)[i*nV];
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp > maxAi)
      {
        maxAi = ntemp;
      }
    }
    maxA += maxAi;
  }

  // Calculate inveps := 1/eps, where 1/eps > deg(p)*max1 for all p in G.
  int inveps, tot_deg = 0, maxdeg;

  intvec* ivUnit = Mivdp(nV);//19.02
  for(i=nG-1; i>=0; i--)
  {
    // maxdeg = pTotaldegree(G->m[i], currRing); //it's wrong for ex1,2,rose
    maxdeg = MwalkWeightDegree(G->m[i], ivUnit);
    if (maxdeg > tot_deg )
    {
      tot_deg = maxdeg;
    }
  }
  delete ivUnit;

  inveps = (tot_deg * maxA) + 1;

#ifdef INVEPS_SMALL_IN_FRACTAL
  //  Print("\n// choose the\"small\" inverse epsilon := %d / %d = ", inveps, pdeg);
  if(inveps > pdeg && pdeg > 3)
  {
    inveps = inveps / pdeg;
  }
  // Print(" %d", inveps);
#else
  PrintS("\n// the \"big\" inverse epsilon %d", inveps);
#endif

  // Pert(A1) = inveps^(pdeg-1)*A1 + inveps^(pdeg-2)*A2+...+A_pdeg
  for ( i=1; i < pdeg; i++ )
  {
    for(j=0; j<nV; j++)
    {
      (*pert_vector)[j] = inveps*((*pert_vector)[j]) + (*ivtarget)[i*nV+j];
    }
  }

  int temp = (*pert_vector)[0];
  for(i=1; i<nV; i++)
  {
    temp = gcd(temp, (*pert_vector)[i]);
    if(temp == 1)
    {
      break;
    }
  }
  if(temp != 1)
  {
    for(i=0; i<nV; i++)
    {
      (*pert_vector)[i] = (*pert_vector)[i] / temp;
    }
  }

  intvec* result = pert_vector;
  delete pert_vector;
  return result;
}

/*****************************************************************************
 * define a lexicographic order matrix as intvec                             *
 *****************************************************************************/
intvec* MivMatrixOrderlp(int nV)
{
  int i;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
  {
    (*ivM)[i*nV + i] = 1;
  }
  return(ivM);
}


/*****************************************************************************
 * define a reverse lexicographic order (dp) matrix as intvec                *
 *****************************************************************************/
intvec* MivMatrixOrderdp(int nV)
{
  int i;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
  {
    (*ivM)[i] = 1;
  }
  for(i=1; i<nV; i++)
  {
    (*ivM)[(i+1)*nV - i] = -1;
  }
  return(ivM);
}

/*****************************************************************************
 * creates an intvec of the monomial order Wp(ivstart)                       *
 *****************************************************************************/
intvec* MivWeightOrderlp(intvec* ivstart)
{
  int i;
  int nV = ivstart->length();
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
  {
    (*ivM)[i] = (*ivstart)[i];
  }
  for(i=1; i<nV; i++)
  {
    (*ivM)[i*nV + i-1] = 1;
  }
  return(ivM);
}

/*****************************************************************************
 * creates an intvec of the monomial order dp(ivstart)                       *
 *****************************************************************************/
intvec* MivWeightOrderdp(intvec* ivstart)
{
  int i;
  int nV = ivstart->length();
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
  {
    (*ivM)[i] = (*ivstart)[i];
  }
  for(i=0; i<nV; i++)
  {
    (*ivM)[nV+i] = 1;
  }
  for(i=2; i<nV; i++)
  {
    (*ivM)[(i+1)*nV - i] = -1;
  }
  return(ivM);
}

//unused
/*
static intvec* MatrixOrderdp(int nV)
{
  int i;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
  {
    (*ivM)[i] = 1;
  }
  for(i=1; i<nV; i++)
  {
    (*ivM)[(i+1)*nV - i] = -1;
  }
  return(ivM);
}
*/

intvec* MivUnit(int nV)
{
  int i;
  intvec* ivM = new intvec(nV);
  for(i=nV-1; i>=0; i--)
  {
    (*ivM)[i] = 1;
  }
  return(ivM);
}


/************************************************************************
*  compute a perturbed weight vector of a matrix order w.r.t. an ideal  *
*************************************************************************/
int Xnlev;
intvec* Mfpertvector(ideal G, intvec* ivtarget)
{
  int i, j, nG = IDELEMS(G);
  int nV = currRing->N;
  int niv = nV*nV;


  // Calculate maxA = Max(A2) + Max(A3) + ... + Max(AnV),
  // where the Ai are the i-te rows of the matrix 'targer_ord'.
  int ntemp, maxAi, maxA=0;
  for(i=1; i<nV; i++)
  {
    maxAi = (*ivtarget)[i*nV];
    if(maxAi<0)
    {
      maxAi = -maxAi;
    }
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp < 0)
      {
        ntemp = -ntemp;
      }
      if(ntemp > maxAi)
      {
        maxAi = ntemp;
      }
    }
    maxA = maxA + maxAi;
  }
  intvec* ivUnit = Mivdp(nV);

  // Calculate inveps = 1/eps, where 1/eps > deg(p)*maxA for all p in G.
  mpz_t tot_deg; mpz_init(tot_deg);
  mpz_t maxdeg; mpz_init(maxdeg);
  mpz_t inveps; mpz_init(inveps);


  for(i=nG-1; i>=0; i--)
  {
    mpz_set_ui(maxdeg, MwalkWeightDegree(G->m[i], ivUnit));
    if (mpz_cmp(maxdeg,  tot_deg) > 0 )
    {
      mpz_set(tot_deg, maxdeg);
    }
  }

  delete ivUnit;
  //inveps = (tot_deg * maxA) + 1;
  mpz_mul_ui(inveps, tot_deg, maxA);
  mpz_add_ui(inveps, inveps, 1);

  // takes  "small" inveps
#ifdef INVEPS_SMALL_IN_FRACTAL
  if(mpz_cmp_ui(inveps, nV)>0 && nV > 3)
  {
    mpz_cdiv_q_ui(inveps, inveps, nV);
  }
  // choose the small inverse epsilon
#endif

  // PrintLn();  mpz_out_str(stdout, 10, inveps);

  // Calculate the perturbed target orders:
  mpz_t *ivtemp=(mpz_t *)omAlloc(nV*sizeof(mpz_t));
  mpz_t *pert_vector=(mpz_t *)omAlloc(niv*sizeof(mpz_t));

  for(i=0; i < nV; i++)
  {
    mpz_init_set_si(ivtemp[i], (*ivtarget)[i]);
    mpz_init_set_si(pert_vector[i], (*ivtarget)[i]);
  }

  mpz_t ztmp; mpz_init(ztmp);
  // BOOLEAN isneg = FALSE;

  for(i=1; i<nV; i++)
  {
    for(j=0; j<nV; j++)
    {
      mpz_mul(ztmp, inveps, ivtemp[j]);
      if((*ivtarget)[i*nV+j]<0)
      {
        mpz_sub_ui(ivtemp[j], ztmp, -(*ivtarget)[i*nV+j]);
      }
      else
      {
        mpz_add_ui(ivtemp[j], ztmp,(*ivtarget)[i*nV+j]);
      }
    }

    for(j=0; j<nV; j++)
    {
      mpz_init_set(pert_vector[i*nV+j],ivtemp[j]);
    }
  }

  // 2147483647 is max. integer representation in SINGULAR
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  intvec* result = new intvec(niv);
  BOOLEAN nflow = FALSE;

  // computes gcd
  mpz_set(ztmp, pert_vector[0]);
  for(i=0; i<niv; i++)
  {
    mpz_gcd(ztmp, ztmp, pert_vector[i]);
    if(mpz_cmp_si(ztmp, 1)==0)
    {
      break;
    }
  }

  for(i=0; i<niv; i++)
  {
    mpz_divexact(pert_vector[i], pert_vector[i], ztmp);
    (* result)[i] = mpz_get_si(pert_vector[i]);
  }

  CHECK_OVERFLOW:

  for(i=0; i<niv; i++)
  {
    if(mpz_cmp(pert_vector[i], sing_int)>0)
    {
      if(nflow == FALSE)
      {
        Xnlev = i / nV;
        nflow = TRUE;
        Overflow_Error = TRUE;
        Print("\n// Xlev = %d and the %d-th element is", Xnlev,  i+1);
        PrintS("\n// ** OVERFLOW in \"Mfpertvector\": ");
        mpz_out_str( stdout, 10, pert_vector[i]);
        PrintS(" is greater than 2147483647 (max. integer representation)");
        Print("\n//  So vector[%d] := %d is wrong!!", i+1, (*result)[i]);
      }
    }
  }
  if(Overflow_Error == TRUE)
  {
    ivString(result, "new_vector");
  }
  omFree(pert_vector);
  omFree(ivtemp);
  mpz_clear(ztmp);
  mpz_clear(tot_deg);
  mpz_clear(maxdeg);
  mpz_clear(inveps);
  mpz_clear(sing_int);

  rComplete(currRing);
  for(j=0; j<IDELEMS(G); j++)
  {
    poly p=G->m[j];
    while(p!=NULL)
    {
      p_Setm(p,currRing);
      pIter(p);
    }
  }
  return result;
}

/****************************************************************
 * Multiplication of two ideals element by element              *
 * i.e. Let be A := (a_i) and B := (b_i), return C := (a_i*b_i) *
 *  destroy A, keeps B                                          *
 ****************************************************************/
static ideal MidMult(ideal A, ideal B)
{
  int mA = IDELEMS(A), mB = IDELEMS(B);

  if(A==NULL || B==NULL)
  {
    return NULL;
  }
  if(mB < mA)
  {
    mA = mB;
  }
  ideal result = idInit(mA, 1);

  int i, k=0;
  for(i=0; i<mA; i++)
    {
      result->m[k] = pMult(A->m[i], pCopy(B->m[i]));
      A->m[i]=NULL;
      if (result->m[k]!=NULL)
      {
        k++;
      }
    }

  idDelete(&A);
  idSkipZeroes(result);
  return result;
}

/*********************************************************************
 * G is a red. Groebner basis w.r.t. <_1                             *
 * Gomega is an initial form ideal of <G> w.r.t. a weight vector w   *
 * M is a subideal of <Gomega> and M selft is a red. Groebner basis  *
 *    of the ideal <Gomega> w.r.t. <_w                               *
 * Let m_i = h1.gw1 + ... + hs.gws for each m_i in M; gwi in Gomega  *
 * return F with n(F) = n(M) and f_i = h1.g1 + ... + hs.gs for each i*
 ********************************************************************/
static ideal MLifttwoIdeal(ideal Gw, ideal M, ideal G)
{
  ideal Mtmp = idLift(Gw, M, NULL, FALSE, TRUE, TRUE, NULL);

  // If Gw is a GB, then isSB = TRUE, otherwise FALSE
  // So, it is better, if one tests whether Gw is a GB
  // in ideals.cc:
  // idLift (ideal mod, ideal submod,ideal * rest, BOOLEAN goodShape,
  //           BOOLEAN isSB,BOOLEAN divide,matrix * unit)

  // Let be Mtmp = {m1,...,ms}, where mi=sum hij.in_gj, for all i=1,...,s
  // We compute F = {f1,...,fs}, where fi=sum hij.gj
  int i, j, nM = IDELEMS(Mtmp);
  ideal idpol, idLG;
  ideal F = idInit(nM, 1);

  for(i=0; i<nM; i++)
  {
     idpol = idVec2Ideal(Mtmp->m[i]);
     idLG = MidMult(idpol, G);
     idpol = NULL;
     F->m[i] = NULL;
     for(j=IDELEMS(idLG)-1; j>=0; j--)
     {
       F->m[i] = pAdd(F->m[i], idLG->m[j]);
       idLG->m[j]=NULL;
     }
     idDelete(&idLG);
  }
  idDelete(&Mtmp);
  return F;
}

//unused
/*
static void checkidealCC(ideal G, char* Ch)
{
  int i,nmon=0,ntmp;
  int nG = IDELEMS(G);
  int n = nG-1;
  Print("\n// ** Ideal %s besteht aus %d Polynomen mit ", Ch, nG);

  for(i=0; i<nG; i++)
  {
    ntmp =  pLength(G->m[i]);
    nmon += ntmp;

    if(i != n)
    {
      Print("%d, ", ntmp);
    }
    else
    {
      Print(" bzw. %d ", ntmp);
    }
  }
  PrintS(" Monomen.\n");
  Print("// ** %s besitzt %d Monome.", Ch, nmon);
  PrintLn();
}
*/

//unused
/*
static void HeadidString(ideal L, char* st)
{
  int i, nL = IDELEMS(L)-1;

  Print("//  The head terms of the ideal %s = ", st);
  for(i=0; i<nL; i++)
  {
    Print(" %s, ", pString(pHead(L->m[i])));
  }
  Print(" %s;\n", pString(pHead(L->m[nL])));
}

*/
static inline int MivComp(intvec* iva, intvec* ivb)
{
  assume(iva->length() == ivb->length());
  int i;
  for(i=iva->length()-1; i>=0; i--)
  {
    if((*iva)[i] - (*ivb)[i] != 0)
    {
      return 0;
    }
  }
  return 1;
}

/**********************************************
 * Look for the smallest absolut value in vec *
 **********************************************/
static int MivAbsMax(intvec* vec)
{
  int i,k;
  if((*vec)[0] < 0)
  {
    k = -(*vec)[0];
  }
  else
  {
    k = (*vec)[0];
  }
  for(i=1; i < (vec->length()); i++)
  {
    if((*vec)[i] < 0)
    {
      if(-(*vec)[i] > k)
      {
        k = -(*vec)[i];
      }
    }
    else
    {
      if((*vec)[i] > k)
      {
        k = (*vec)[i];
      }
    }
  }
  return k;
}


/**************************************************************
 * Look for the position of the smallest absolut value in vec *
 **************************************************************/
static int MivAbsMaxArg(intvec* vec)
{
  int k = MivAbsMax(vec);
  int i=0;
  while(1)
  {
    if((*vec)[i] == k || (*vec)[i] == -k)
    {
      break;
    }
    i++;
  }
  return i;
}


/**********************************************************************
 * Compute a next weight vector between curr_weight and target_weight *
 * with respect to an ideal <G>.                                      *
**********************************************************************/
/*
static intvec* MwalkNextWeightCC(intvec* curr_weight, intvec* target_weight,
                                 ideal G)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  assume(currRing != NULL && curr_weight != NULL &&
         target_weight != NULL && G != NULL);

  int nRing = currRing->N;
  int checkRed, j, nG = IDELEMS(G);
  intvec* ivtemp;

  mpz_t t_zaehler, t_nenner;
  mpz_init(t_zaehler);
  mpz_init(t_nenner);

  mpz_t s_zaehler, s_nenner, temp, MwWd;
  mpz_init(s_zaehler);
  mpz_init(s_nenner);
  mpz_init(temp);
  mpz_init(MwWd);

  mpz_t sing_int;
  mpz_init(sing_int);
  mpz_set_ui(sing_int,  2147483647);

  mpz_t sing_int_half;
  mpz_init(sing_int_half);
  mpz_set_ui(sing_int_half,  3*(1073741824/2));

  mpz_t deg_w0_p1, deg_d0_p1;
  mpz_init(deg_w0_p1);
  mpz_init(deg_d0_p1);

  mpz_t sztn, sntz;
  mpz_init(sztn);
  mpz_init(sntz);

  mpz_t t_null;
  mpz_init(t_null);

  mpz_t ggt;
  mpz_init(ggt);

  mpz_t dcw;
  mpz_init(dcw);

  int gcd_tmp;
  intvec* diff_weight = MivSub(target_weight, curr_weight);

  intvec* diff_weight1 = MivSub(target_weight, curr_weight);
  poly g;

  for (j=0; j<nG; j++)
  {
    g = G->m[j];
    if (g != NULL)
    {
      ivtemp = MExpPol(g);
      mpz_set_si(deg_w0_p1, MivDotProduct(ivtemp, curr_weight));
      mpz_set_si(deg_d0_p1, MivDotProduct(ivtemp, diff_weight));
      delete ivtemp;

      pIter(g);
      while (g != NULL)
      {
        ivtemp = MExpPol(g);
        mpz_set_si(MwWd, MivDotProduct(ivtemp, curr_weight));
        mpz_sub(s_zaehler, deg_w0_p1, MwWd);
        if(mpz_cmp(s_zaehler, t_null) != 0)
        {
          mpz_set_si(MwWd, MivDotProduct(ivtemp, diff_weight));
          mpz_sub(s_nenner, MwWd, deg_d0_p1);
          // check for 0 < s <= 1
          if( (mpz_cmp(s_zaehler,t_null) > 0 &&
               mpz_cmp(s_nenner, s_zaehler)>=0) ||
              (mpz_cmp(s_zaehler, t_null) < 0 &&
               mpz_cmp(s_nenner, s_zaehler)<=0))
          {
            // make both positive
            if (mpz_cmp(s_zaehler, t_null) < 0)
            {
              mpz_neg(s_zaehler, s_zaehler);
              mpz_neg(s_nenner, s_nenner);
            }

            //compute a simple fraction of s
            cancel(s_zaehler, s_nenner);

            if(mpz_cmp(t_nenner, t_null) != 0)
            {
              mpz_mul(sztn, s_zaehler, t_nenner);
              mpz_mul(sntz, s_nenner, t_zaehler);

              if(mpz_cmp(sztn,sntz) < 0)
              {
                mpz_add(t_nenner, t_null, s_nenner);
                mpz_add(t_zaehler,t_null, s_zaehler);
              }
            }
            else
            {
              mpz_add(t_nenner, t_null, s_nenner);
              mpz_add(t_zaehler,t_null, s_zaehler);
            }
          }
        }
        pIter(g);
        delete ivtemp;
      }
    }
  }
  //Print("\n// Alloc Size = %d \n", nRing*sizeof(mpz_t));
  mpz_t *vec=(mpz_t*)omAlloc(nRing*sizeof(mpz_t));


  // there is no 0<t<1 and define the next weight vector that is equal
  // to the current weight vector
  if(mpz_cmp(t_nenner, t_null) == 0)
  {
#ifndef SING_NDEBUG
    PrintS("\n//MwalkNextWeightCC: t_nenner=0\n");
#endif
    delete diff_weight;
    diff_weight = ivCopy(curr_weight);//take memory
    goto FINISH;
  }

  // define the target vector as the next weight vector, if t = 1
  if(mpz_cmp_si(t_nenner, 1)==0 && mpz_cmp_si(t_zaehler,1)==0)
  {
    delete diff_weight;
    diff_weight = ivCopy(target_weight); //this takes memory
    goto FINISH;
  }

   checkRed = 0;

  SIMPLIFY_GCD:

  // simplify the vectors curr_weight and diff_weight (C-int)
  gcd_tmp = (*curr_weight)[0];

  for (j=1; j<nRing; j++)
  {
    gcd_tmp = gcd(gcd_tmp, (*curr_weight)[j]);
    if(gcd_tmp == 1)
    {
      break;
    }
  }
  if(gcd_tmp != 1)
  {
    for (j=0; j<nRing; j++)
    {
      gcd_tmp = gcd(gcd_tmp, (*diff_weight)[j]);
      if(gcd_tmp == 1)
      {
        break;
      }
    }
  }
  if(gcd_tmp != 1)
  {
    for (j=0; j<nRing; j++)
    {
      (*curr_weight)[j] =  (*curr_weight)[j]/gcd_tmp;
      (*diff_weight)[j] =  (*diff_weight)[j]/gcd_tmp;
    }
  }
  if(checkRed > 0)
  {
    for (j=0; j<nRing; j++)
    {
      mpz_set_si(vec[j], (*diff_weight)[j]);
    }
    goto TEST_OVERFLOW;
  }

#ifdef  NEXT_VECTORS_CC
  Print("\n// gcd of the weight vectors (current and target) = %d", gcd_tmp);
  ivString(curr_weight, "new cw");
  ivString(diff_weight, "new dw");

  PrintS("\n// t_zaehler: ");  mpz_out_str( stdout, 10, t_zaehler);
  PrintS(", t_nenner: ");  mpz_out_str( stdout, 10, t_nenner);
#endif

// construct a new weight vector and check whether vec[j] is overflow,
// i.e. vec[j] > 2^31.
// If vec[j] doesn't overflow, define a weight vector. Otherwise,
// report that overflow appears. In the second case, test whether the
// the correctness of the new vector plays an important role

  for (j=0; j<nRing; j++)
  {
    mpz_set_si(dcw, (*curr_weight)[j]);
    mpz_mul(s_nenner, t_nenner, dcw);

    if( (*diff_weight)[j]>0)
    {
      mpz_mul_ui(s_zaehler, t_zaehler, (*diff_weight)[j]);
    }
    else
    {
      mpz_mul_ui(s_zaehler, t_zaehler, -(*diff_weight)[j]);
      mpz_neg(s_zaehler, s_zaehler);
    }
    mpz_add(sntz, s_nenner, s_zaehler);
    mpz_init_set(vec[j], sntz);

#ifdef NEXT_VECTORS_CC
    Print("\n//   j = %d ==> ", j);
    PrintS("(");
    mpz_out_str( stdout, 10, t_nenner);
    Print(" * %d)", (*curr_weight)[j]);
    PrintS(" + ("); mpz_out_str( stdout, 10, t_zaehler);
    Print(" * %d) =  ",  (*diff_weight)[j]);
    mpz_out_str( stdout, 10, s_nenner);
    PrintS(" + ");
    mpz_out_str( stdout, 10, s_zaehler);
    PrintS(" = "); mpz_out_str( stdout, 10, sntz);
    Print(" ==> vector[%d]: ", j); mpz_out_str(stdout, 10, vec[j]);
#endif

    if(j==0)
    {
      mpz_set(ggt, sntz);
    }
    else
    {
      if(mpz_cmp_si(ggt,1) != 0)
      {
        mpz_gcd(ggt, ggt, sntz);
      }
    }
  }
  // reduce the vector with the gcd
  if(mpz_cmp_si(ggt,1) != 0)
  {
    for (j=0; j<nRing; j++)
    {
      mpz_divexact(vec[j], vec[j], ggt);
    }
  }
#ifdef  NEXT_VECTORS_CC
  PrintS("\n// gcd of elements of the vector: ");
  mpz_out_str( stdout, 10, ggt);
#endif

  for(j=0; j<nRing; j++)
  {
    if(mpz_cmp(vec[j], sing_int_half) >= 0)
    {
      goto REDUCTION;
    }
  }
  checkRed = 1;
  for (j=0; j<nRing; j++)
    {
      (*diff_weight)[j] = mpz_get_si(vec[j]);
    }
  goto SIMPLIFY_GCD;

  REDUCTION:
  checkRed = 1;
  for (j=0; j<nRing; j++)
  {
    (*diff_weight1)[j] = mpz_get_si(vec[j]);
  }
  while(test_w_in_ConeCC(G,diff_weight1))
  {
    for(j=0; j<nRing; j++)
    {
      (*diff_weight)[j] = (*diff_weight1)[j];
      mpz_set_si(vec[j], (*diff_weight)[j]);
    }
    for(j=0; j<nRing; j++)
    {
      (*diff_weight1)[j] = floor(0.1*(*diff_weight)[j] + 0.5);
    }
  }
  if(MivAbsMax(diff_weight)>10000)
  {
    for(j=0; j<nRing; j++)
    {
      (*diff_weight1)[j] = (*diff_weight)[j];
    }
    j = 0;
    while(test_w_in_ConeCC(G,diff_weight1))
    {
      (*diff_weight)[j] = (*diff_weight1)[j];
      mpz_set_si(vec[j], (*diff_weight)[j]);
      j = MivAbsMaxArg(diff_weight1);
      (*diff_weight1)[j] = floor(0.1*(*diff_weight1)[j] + 0.5);
    }
    goto SIMPLIFY_GCD;
  }

 TEST_OVERFLOW:

  for (j=0; j<nRing; j++)
  {
    if(mpz_cmp(vec[j], sing_int)>=0)
    {
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = TRUE;
        PrintS("\n// ** OVERFLOW in \"MwalkNextWeightCC\": ");
        mpz_out_str( stdout, 10, vec[j]);
        PrintS(" is greater than 2147483647 (max. integer representation)\n");
        //Print("//  So vector[%d] := %d is wrong!!\n",j+1, vec[j]);// vec[j] is mpz_t
      }
    }
  }

 FINISH:
   delete diff_weight1;
   mpz_clear(t_zaehler);
   mpz_clear(t_nenner);
   mpz_clear(s_zaehler);
   mpz_clear(s_nenner);
   mpz_clear(sntz);
   mpz_clear(sztn);
   mpz_clear(temp);
   mpz_clear(MwWd);
   mpz_clear(deg_w0_p1);
   mpz_clear(deg_d0_p1);
   mpz_clear(ggt);
   omFree(vec);
   mpz_clear(sing_int_half);
   mpz_clear(sing_int);
   mpz_clear(dcw);
   mpz_clear(t_null);

  if(Overflow_Error == FALSE)
  {
    Overflow_Error = nError;
  }
  rComplete(currRing);
  for(j=0; j<IDELEMS(G); j++)
  {
    poly p=G->m[j];
    while(p!=NULL)
    {
      p_Setm(p,currRing);
      pIter(p);
    }
  }
return diff_weight;
}
*/
/**********************************************************************
 * Compute a next weight vector between curr_weight and target_weight *
 * with respect to an ideal <G>.                                      *
**********************************************************************/
static intvec* MwalkNextWeightCC(intvec* curr_weight, intvec* target_weight,
                                 ideal G)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  assume(currRing != NULL && curr_weight != NULL &&
         target_weight != NULL && G != NULL);

  int nRing = currRing->N;
  int j, nG = IDELEMS(G);
  intvec* ivtemp;

  mpz_t t_zaehler, t_nenner;
  mpz_init(t_zaehler);
  mpz_init(t_nenner);

  mpz_t s_zaehler, s_nenner, temp, MwWd;
  mpz_init(s_zaehler);
  mpz_init(s_nenner);
  mpz_init(temp);
  mpz_init(MwWd);

  mpz_t sing_int;
  mpz_init(sing_int);
  mpz_set_ui(sing_int,  2147483647);

  mpz_t sing_int_half;
  mpz_init(sing_int_half);
  mpz_set_ui(sing_int_half,  3*(1073741824/2));

  mpz_t deg_w0_p1, deg_d0_p1;
  mpz_init(deg_w0_p1);
  mpz_init(deg_d0_p1);

  mpz_t sztn, sntz;
  mpz_init(sztn);
  mpz_init(sntz);

  mpz_t t_null;
  mpz_init(t_null);

  mpz_t ggt;
  mpz_init(ggt);

  mpz_t dcw;
  mpz_init(dcw);

  int gcd_tmp;
  //intvec* diff_weight = MivSub(target_weight, curr_weight);

  intvec* diff_weight1 = new intvec(nRing); //MivSub(target_weight, curr_weight);
  poly g;

  // reduce the size of the entries of the current weight vector
  if(TEST_OPT_REDSB)
  {
    for (j=0; j<nRing; j++)
    {
      (*diff_weight1)[j] = (*curr_weight)[j];
    }
    while(MivAbsMax(diff_weight1)>10000 && test_w_in_ConeCC(G,diff_weight1)==1)
    {
      for(j=0; j<nRing; j++)
      {
        (*curr_weight)[j] = (*diff_weight1)[j];
      }
      for(j=0; j<nRing; j++)
      {
        (*diff_weight1)[j] = ((*diff_weight1)[j] + 5) / 10;
      }
    }

    if(MivAbsMax(curr_weight)>100000)
    {
      for(j=0; j<nRing; j++)
      {
        (*diff_weight1)[j] = (*curr_weight)[j];
      }
      j = 0;
      while(test_w_in_ConeCC(G,diff_weight1)==1 && MivAbsMax(diff_weight1)>1000)
      {
        (*curr_weight)[j] = (*diff_weight1)[j];
        j = MivAbsMaxArg(diff_weight1);
        (*diff_weight1)[j] = ((*diff_weight1)[j] + 5) / 10;
      }
    }

  }
  intvec* diff_weight = MivSub(target_weight, curr_weight);

  // compute a suitable next weight vector
  for (j=0; j<nG; j++)
  {
    g = G->m[j];
    if (g != NULL)
    {
      ivtemp = MExpPol(g);
      mpz_set_si(deg_w0_p1, MivDotProduct(ivtemp, curr_weight));
      mpz_set_si(deg_d0_p1, MivDotProduct(ivtemp, diff_weight));
      delete ivtemp;

      pIter(g);
      while (g != NULL)
      {
        ivtemp = MExpPol(g);
        mpz_set_si(MwWd, MivDotProduct(ivtemp, curr_weight));
        mpz_sub(s_zaehler, deg_w0_p1, MwWd);
        if(mpz_cmp(s_zaehler, t_null) != 0)
        {
          mpz_set_si(MwWd, MivDotProduct(ivtemp, diff_weight));
          mpz_sub(s_nenner, MwWd, deg_d0_p1);
          // check for 0 < s <= 1
          if( (mpz_cmp(s_zaehler,t_null) > 0 &&
               mpz_cmp(s_nenner, s_zaehler)>=0) ||
              (mpz_cmp(s_zaehler, t_null) < 0 &&
               mpz_cmp(s_nenner, s_zaehler)<=0))
          {
            // make both positive
            if (mpz_cmp(s_zaehler, t_null) < 0)
            {
              mpz_neg(s_zaehler, s_zaehler);
              mpz_neg(s_nenner, s_nenner);
            }

            //compute a simple fraction of s
            cancel(s_zaehler, s_nenner);

            if(mpz_cmp(t_nenner, t_null) != 0)
            {
              mpz_mul(sztn, s_zaehler, t_nenner);
              mpz_mul(sntz, s_nenner, t_zaehler);

              if(mpz_cmp(sztn,sntz) < 0)
              {
                mpz_add(t_nenner, t_null, s_nenner);
                mpz_add(t_zaehler,t_null, s_zaehler);
              }
            }
            else
            {
              mpz_add(t_nenner, t_null, s_nenner);
              mpz_add(t_zaehler,t_null, s_zaehler);
            }
          }
        }
        pIter(g);
        delete ivtemp;
      }
    }
  }
  //Print("\n// Alloc Size = %d \n", nRing*sizeof(mpz_t));
  mpz_t *vec=(mpz_t*)omAlloc(nRing*sizeof(mpz_t));


  // there is no 0<t<1 and define the next weight vector that is equal
  // to the current weight vector
  if(mpz_cmp(t_nenner, t_null) == 0)
  {
#ifndef SING_NDEBUG
    PrintS("\n//MwalkNextWeightCC: t_nenner=0\n");
#endif
    delete diff_weight;
    diff_weight = ivCopy(curr_weight);//take memory
    goto FINISH;
  }

  // define the target vector as the next weight vector, if t = 1
  if(mpz_cmp_si(t_nenner, 1)==0 && mpz_cmp_si(t_zaehler,1)==0)
  {
    delete diff_weight;
    diff_weight = ivCopy(target_weight); //this takes memory
    goto FINISH;
  }

  SIMPLIFY_GCD:

  // simplify the vectors curr_weight and diff_weight (C-int)
  gcd_tmp = (*curr_weight)[0];

  for (j=1; j<nRing; j++)
  {
    gcd_tmp = gcd(gcd_tmp, (*curr_weight)[j]);
    if(gcd_tmp == 1)
    {
      break;
    }
  }
  if(gcd_tmp != 1)
  {
    for (j=0; j<nRing; j++)
    {
      gcd_tmp = gcd(gcd_tmp, (*diff_weight)[j]);
      if(gcd_tmp == 1)
      {
        break;
      }
    }
  }
  if(gcd_tmp != 1)
  {
    for (j=0; j<nRing; j++)
    {
      (*curr_weight)[j] =  (*curr_weight)[j]/gcd_tmp;
      (*diff_weight)[j] =  (*diff_weight)[j]/gcd_tmp;
    }
  }

#ifdef  NEXT_VECTORS_CC
  Print("\n// gcd of the weight vectors (current and target) = %d", gcd_tmp);
  ivString(curr_weight, "new cw");
  ivString(diff_weight, "new dw");

  PrintS("\n// t_zaehler: ");  mpz_out_str( stdout, 10, t_zaehler);
  PrintS(", t_nenner: ");  mpz_out_str( stdout, 10, t_nenner);
#endif

// construct a new weight vector and check whether vec[j] is overflow, i.e. vec[j] > 2^31.
// If vec[j] doesn't overflow, define a weight vector. Otherwise, report that overflow
// appears. In the second case, test whether the the correctness of the new vector plays
// an important role

  for (j=0; j<nRing; j++)
  {
    mpz_set_si(dcw, (*curr_weight)[j]);
    mpz_mul(s_nenner, t_nenner, dcw);

    if( (*diff_weight)[j]>0)
    {
      mpz_mul_ui(s_zaehler, t_zaehler, (*diff_weight)[j]);
    }
    else
    {
      mpz_mul_ui(s_zaehler, t_zaehler, -(*diff_weight)[j]);
      mpz_neg(s_zaehler, s_zaehler);
    }
    mpz_add(sntz, s_nenner, s_zaehler);
    mpz_init_set(vec[j], sntz);

#ifdef NEXT_VECTORS_CC
    Print("\n//   j = %d ==> ", j);
    PrintS("(");
    mpz_out_str( stdout, 10, t_nenner);
    Print(" * %d)", (*curr_weight)[j]);
    PrintS(" + ("); mpz_out_str( stdout, 10, t_zaehler);
    Print(" * %d) =  ",  (*diff_weight)[j]);
    mpz_out_str( stdout, 10, s_nenner);
    PrintS(" + ");
    mpz_out_str( stdout, 10, s_zaehler);
    PrintS(" = "); mpz_out_str( stdout, 10, sntz);
    Print(" ==> vector[%d]: ", j); mpz_out_str(stdout, 10, vec[j]);
#endif

    if(j==0)
    {
      mpz_set(ggt, sntz);
    }
    else
    {
      if(mpz_cmp_si(ggt,1) != 0)
      {
        mpz_gcd(ggt, ggt, sntz);
      }
    }
  }
  // reduce the vector with the gcd
  if(mpz_cmp_si(ggt,1) != 0)
  {
    for (j=0; j<nRing; j++)
    {
      mpz_divexact(vec[j], vec[j], ggt);
    }
  }
#ifdef  NEXT_VECTORS_CC
  PrintS("\n// gcd of elements of the vector: ");
  mpz_out_str( stdout, 10, ggt);
#endif

  for (j=0; j<nRing; j++)
  {
    (*diff_weight)[j] = mpz_get_si(vec[j]);
  }

 TEST_OVERFLOW:

  for (j=0; j<nRing; j++)
  {
    if(mpz_cmp(vec[j], sing_int)>=0)
    {
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = TRUE;
        PrintS("\n// ** OVERFLOW in \"MwalkNextWeightCC\": ");
        mpz_out_str( stdout, 10, vec[j]);
        PrintS(" is greater than 2147483647 (max. integer representation)\n");
        //Print("//  So vector[%d] := %d is wrong!!\n",j+1, vec[j]);// vec[j] is mpz_t
      }
    }
  }

 FINISH:
   delete diff_weight1;
   mpz_clear(t_zaehler);
   mpz_clear(t_nenner);
   mpz_clear(s_zaehler);
   mpz_clear(s_nenner);
   mpz_clear(sntz);
   mpz_clear(sztn);
   mpz_clear(temp);
   mpz_clear(MwWd);
   mpz_clear(deg_w0_p1);
   mpz_clear(deg_d0_p1);
   mpz_clear(ggt);
   omFree(vec);
   mpz_clear(sing_int_half);
   mpz_clear(sing_int);
   mpz_clear(dcw);
   mpz_clear(t_null);

  if(Overflow_Error == FALSE)
  {
    Overflow_Error = nError;
  }
  rComplete(currRing);
  for(j=0; j<IDELEMS(G); j++)
  {
    poly p=G->m[j];
    while(p!=NULL)
    {
      p_Setm(p,currRing);
      pIter(p);
    }
  }
return diff_weight;
}


/**********************************************************************
* Compute an intermediate weight vector from iva to ivb w.r.t.        *
* the reduced Groebner basis G.                                       *
* Return NULL, if it is equal to iva or iva = avb.                    *
**********************************************************************/
intvec* MkInterRedNextWeight(intvec* iva, intvec* ivb, ideal G)
{
  intvec* tmp = new intvec(iva->length());
  intvec* result;

  if(G == NULL)
  {
    return tmp;
  }
  if(MivComp(iva, ivb) == 1)
  {
    return tmp;
  }
  result = MwalkNextWeightCC(iva, ivb, G);

  if(MivComp(result, iva) == 1)
  {
    delete result;
    return tmp;
  }

  delete tmp;
  return result;
}

/********************************************************************
 * define and execute a new ring which order is (a(vb),a(va),lp,C)  *
 * ******************************************************************/
/*static ring VMrHomogeneous(intvec* va, intvec* vb)
{

  if ((currRing->ppNoether)!=NULL)
  {
    pDelete(&(currRing->ppNoether));
  }
  if (((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING)) ||
      ((sLastPrinted.rtyp==LIST_CMD)&&(lRingDependend((lists)sLastPrinted.data))))
  {
    sLastPrinted.CleanUp();
  }

  ring r = (ring) omAlloc0Bin(sip_sring_bin);
  int i, nv = currRing->N;

  r->cf  = currRing->cf;
  r->N   = currRing->N;
  int nb = 4;


  //names
  char* Q; // In order to avoid the corrupted memory, do not change.
  r->names = (char **) omAlloc0(nv * sizeof(char_ptr));
  for(i=0; i<nv; i++)
  {
    Q = currRing->names[i];
    r->names[i]  = omStrDup(Q);
  }

  //weights: entries for 3 blocks: NULL Made:???
  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  r->wvhdl[0] = (int*) omAlloc(nv*sizeof(int));
  r->wvhdl[1] = (int*) omAlloc((nv)*sizeof(int));

  for(i=0; i<nv; i++)
  {
    r->wvhdl[1][i] = (*vb)[i];
    r->wvhdl[0][i] = (*va)[i];
  }
  r->wvhdl[0][nv] = (*va)[nv];

  // order: (1..1),a,lp,C
  r->order = (int *) omAlloc(nb * sizeof(int *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = nv;

 // ringorder a for the second block: var 2..nv
  r->order[1]  = ringorder_a;
  r->block0[1] = 1;
  r->block1[1] = nv;

  // ringorder lp for the third block: var 2..nv
  r->order[2]  = ringorder_lp;
  r->block0[2] = 1;
  r->block1[2] = nv;

  // ringorder C for the 4th block
  // it is very important within "idLift",
  // especially, by ring syz_ring=rCurrRingAssure_SyzComp();
  // therefore, nb  must be (nBlocks(currRing)  + 1)
  r->order[3]  = ringorder_C;

  // polynomial ring
  r->OrdSgn    = 1;

  // complete ring intializations

  rComplete(r);
  return r;
  //rChangeCurrRing(r);
}
*/

/**************************************************************
 * define and execute a new ring which order is (a(va),lp,C)  *
 * ************************************************************/
static ring VMrDefault(intvec* va)
{

  ring r = rCopy0(currRing,FALSE,FALSE);
  int i, nv = currRing->N;

  int nb = 4;

  /*weights: entries for 3 blocks: NULL Made:???*/
  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  r->wvhdl[0] = (int*) omAlloc(nv*sizeof(int));
  for(i=0; i<nv; i++)
    r->wvhdl[0][i] = (*va)[i];

  /* order: a,lp,C,0 */
  r->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = nv;

  // ringorder lp for the second block: var 1..nv
  r->order[1]  = ringorder_lp;
  r->block0[1] = 1;
  r->block1[1] = nv;

  // ringorder C for the third block
  // it is very important within "idLift",
  // especially, by ring syz_ring=rCurrRingAssure_SyzComp();
  // therefore, nb  must be (nBlocks(currRing)  + 1)
  r->order[2]  = ringorder_C;

  // the last block: everything is 0
  r->order[3]  = (rRingOrder_t)0;

  // polynomial ring
  r->OrdSgn    = 1;

  // complete ring intializations

  rComplete(r);
  return r;
  //rChangeCurrRing(r);
}

/****************************************************************
 * define and execute a new ring with ordering (a(va),Wp(vb),C) *
 * **************************************************************/
static ring VMrRefine(intvec* va, intvec* vb)
{

  ring r = rCopy0(currRing,FALSE,FALSE);
  int i, nv = currRing->N;

  int nb = 5;

  //weights: entries for 3 blocks: NULL Made:???
  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  r->wvhdl[0] = (int*) omAlloc(nv*sizeof(int));
  r->wvhdl[1] = (int*) omAlloc(nv*sizeof(int));

  for(i=0; i<nv; i++)
  {
    r->wvhdl[0][i] = (*vb)[i];
    r->wvhdl[1][i] = (*va)[i];
  }

  // order: (1..1),a,lp,C
  r->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = nv;

 // ringorder Wp for the second block: var 1..nv
  r->order[1]  = ringorder_a;
  r->block0[1] = 1;
  r->block1[1] = nv;

  // ringorder lp for the third block: var 1..nv
  r->order[2]  = ringorder_lp;
  r->block0[2] = 1;
  r->block1[2] = nv;

  // ringorder C for the 4th block
  // it is very important within "idLift",
  // especially, by ring syz_ring=rCurrRingAssure_SyzComp();
  // therefore, nb  must be (nBlocks(currRing)  + 1)
  r->order[3]  = ringorder_C;

  // the last block: everything is 0
  r->order[4]  = (rRingOrder_t)0;

  // complete ring intializations

  rComplete(r);

  //rChangeCurrRing(r);
  return r;
}

/*****************************************************
 * define and execute a new ring with ordering (M,C) *
 *****************************************************/
static ring VMatrDefault(intvec* va)
{

  ring r = rCopy0(currRing,FALSE,FALSE);
  int i, nv = currRing->N;

  int nb = 4;

  /*weights: entries for 3 blocks: NULL Made:???*/
  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  r->wvhdl[0] = (int*) omAlloc(nv*nv*sizeof(int));
  r->wvhdl[1] =NULL; // (int*) omAlloc(nv*sizeof(int));
  r->wvhdl[2]=NULL;
  r->wvhdl[3]=NULL;
  for(i=0; i<nv*nv; i++)
    r->wvhdl[0][i] = (*va)[i];

  /* order: a,lp,C,0 */
  r->order = (rRingOrder_t*) omAlloc(nb * sizeof(rRingOrder_t*));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  r->order[0]  = ringorder_M;
  r->block0[0] = 1;
  r->block1[0] = nv;

  // ringorder C for the second block
  r->order[1]  = ringorder_C;
  r->block0[1] = 1;
  r->block1[1] = nv;

// ringorder C for the third block: var 1..nv
  r->order[2]  = ringorder_C;
  r->block0[2] = 1;
  r->block1[2] = nv;

  // the last block: everything is 0
  r->order[3]  = (rRingOrder_t)0;

  // complete ring intializations

  rComplete(r);

  //rChangeCurrRing(r);
  return r;
}

/***********************************************************
 * define and execute a new ring with ordering (a(vb),M,C) *
 ***********************************************************/
static ring VMatrRefine(intvec* va, intvec* vb)
{

  ring r = rCopy0(currRing,FALSE,FALSE);
  int i, nv = currRing->N;
  int nvs = nv*nv;

  int nb = 4;

  /*weights: entries for 3 blocks: NULL Made:???*/
  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  r->wvhdl[0] = (int*) omAlloc(nv*sizeof(int));
  r->wvhdl[1] = (int*) omAlloc(nvs*sizeof(int));
  r->wvhdl[2]=NULL;
  r->wvhdl[3]=NULL;
  for(i=0; i<nvs; i++)
  {
    r->wvhdl[1][i] = (*va)[i];
  }
  for(i=0; i<nv; i++)
  {
    r->wvhdl[0][i] = (*vb)[i];
  }
  /* order: a,lp,C,0 */
  r->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = nv;

  // ringorder M for the second block: var 1..nv
  r->order[1]  = ringorder_M;
  r->block0[1] = 1;
  r->block1[1] = nv;

  // ringorder C for the third block: var 1..nv
  r->order[2]  = ringorder_C;
  r->block0[2] = 1;
  r->block1[2] = nv;

  // the last block: everything is 0
  r->order[3]  = (rRingOrder_t)0;

  // complete ring intializations

  rComplete(r);

  //rChangeCurrRing(r);
  return r;
}

/**********************************************************************
* define and execute a new ring which order is  a lexicographic order *
***********************************************************************/
static void VMrDefaultlp(void)
{
  ring r = rCopy0(currRing,FALSE,FALSE);
  int nv = currRing->N;

  int nb = rBlocks(currRing) + 1;

  /*weights: entries for 3 blocks: NULL Made:???*/

  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));

  /* order: lp,C,0 */
  r->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  /* ringorder lp for the first block: var 1..nv */
  r->order[0]  = ringorder_lp;
  r->block0[0] = 1;
  r->block1[0] = nv;

  /* ringorder C for the second block */
  r->order[1]  = ringorder_C;

  /* the last block: everything is 0 */
  r->order[2]  = (rRingOrder_t)0;

  /*polynomial ring*/
  r->OrdSgn    = 1;

  /* complete ring intializations */

  rComplete(r);

  rChangeCurrRing(r);
}

/***************************************************
* define a ring with parameters und change to it   *
* DefRingPar and DefRingParlp corrupt still memory *
****************************************************/
static void DefRingPar(intvec* va)
{
  int nv = currRing->N;
  int nb = rBlocks(currRing) + 1;

  ring res=rCopy0(currRing,FALSE,FALSE);

  /*weights: entries for 3 blocks: NULL Made:???*/
  res->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));
  res->wvhdl[0] = (int*) omAlloc(nv*sizeof(int));
  for(int i=0; i<nv; i++)
    res->wvhdl[0][i] = (*va)[i];

  /* order: a,lp,C,0 */

  res->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  res->block0 = (int *)omAlloc0(nb * sizeof(int *));
  res->block1 = (int *)omAlloc0(nb * sizeof(int *));

  // ringorder a for the first block: var 1..nv
  res->order[0]  = ringorder_a;
  res->block0[0] = 1;
  res->block1[0] = nv;

  // ringorder lp for the second block: var 1..nv
  res->order[1]  = ringorder_lp;
  res->block0[1] = 1;
  res->block1[1] = nv;

  // ringorder C for the third block
  // it is very important within "idLift",
  // especially, by ring syz_ring=rCurrRingAssure_SyzComp();
  // therefore, nb  must be (nBlocks(currRing)  + 1)
  res->order[2]  = ringorder_C;

  // the last block: everything is 0
  res->order[3]  = (rRingOrder_t)0;

  // polynomial ring
  res->OrdSgn    = 1;


  // complete ring intializations
  rComplete(res);

  // execute the created ring
  rChangeCurrRing(res);
}

static void DefRingParlp(void)
{
  int nv = currRing->N;

  ring r=rCopy0(currRing,FALSE,FALSE);

  int nb = rBlocks(currRing) + 1;

  /*weights: entries for 3 blocks: NULL Made:???*/

  r->wvhdl = (int **)omAlloc0(nb * sizeof(int_ptr));

  /* order: lp,C,0 */
  r->order = (rRingOrder_t *) omAlloc(nb * sizeof(rRingOrder_t *));
  r->block0 = (int *)omAlloc0(nb * sizeof(int *));
  r->block1 = (int *)omAlloc0(nb * sizeof(int *));

  /* ringorder lp for the first block: var 1..nv */
  r->order[0]  = ringorder_lp;
  r->block0[0] = 1;
  r->block1[0] = nv;

  /* ringorder C for the second block */
  r->order[1]  = ringorder_C;

  /* the last block: everything is 0 */
  r->order[2]  = (rRingOrder_t)0;

  /*polynomial ring*/
  r->OrdSgn    = 1;


//   if (rParameter(currRing)!=NULL)
//   {
//     r->cf->extRing->qideal->m[0]=p_Copy(currRing->cf->extRing->qideal->m[0], currRing->cf->extRing);
//     int l=rPar(currRing);
//     r->cf->extRing->names=(char **)omAlloc(l*sizeof(char_ptr));
//
//     for(int i=l-1;i>=0;i--)
//     {
//       rParameter(r)[i]=omStrDup(rParameter(currRing)[i]);
//     }
//   }

  // complete ring intializations

  rComplete(r);

  // execute the created ring
  rChangeCurrRing(r);
}

/*************************************************************
 * check whether one or more components of a vector are zero *
 *************************************************************/
/* unused:
static int isNolVector(intvec* hilb)
{
  int i;
  for(i=hilb->length()-1; i>=0; i--)
  {
    if((* hilb)[i]==0)
    {
      return 1;
    }
  }
  return 0;
}
*/

/*************************************************************
 * check whether one or more components of a vector are <= 0 *
 *************************************************************/
static int isNegNolVector(intvec* hilb)
{
  int i;
  for(i=hilb->length()-1; i>=0; i--)
  {
    if((* hilb)[i]<=0)
    {
      return 1;
    }
  }
  return 0;
}

/**************************************************************************
* Gomega is the initial ideal of G w. r. t. the current weight vector     *
* curr_weight. Check whether curr_weight lies on a border of the Groebner *
* cone, i. e. check whether a monomial is divisible by a leading monomial *
***************************************************************************/
static ideal middleOfCone(ideal G, ideal Gomega)
{
  BOOLEAN middle = FALSE;
  int i,j,N = IDELEMS(Gomega);
  poly p,lm,factor1,factor2;

  ideal Go = idCopy(G);

  // check whether leading monomials of G and Gomega coincide
  // and return NULL if not
  for(i=0; i<N; i++)
  {
    if(!pIsConstant(pSub(pCopy(Gomega->m[i]),pCopy(pHead(G->m[i])))))
    {
      idDelete(&Go);
      return NULL;
    }
  }
  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      if(i!=j)
      {
        p = pCopy(Gomega->m[i]);
        lm = pCopy(Gomega->m[j]);
        pIter(p);
        while(p!=NULL)
        {
          if(pDivisibleBy(lm,p))
          {
            if(middle == FALSE)
            {
              middle = TRUE;
            }
            factor1 = singclap_pdivide(pHead(p),lm,currRing);
            factor2 = pMult(pCopy(factor1),pCopy(Go->m[j]));
            pDelete(&factor1);
            Go->m[i] = pAdd((Go->m[i]),pNeg(pCopy(factor2)));
            pDelete(&factor2);
          }
          pIter(p);
        }
        pDelete(&lm);
        pDelete(&p);
      }
    }
  }

  if(middle == TRUE)
  {
    return Go;
  }
  idDelete(&Go);
  return NULL;
}

/******************************  Februar 2002  ****************************
 * G is a Groebner basis w.r.t. (a(curr_weight),lp) and                   *
 * we compute a GB of <G> w.r.t. the lex. order by the perturbation walk  *
 * its perturbation degree is tp_deg                                      *
 * We call the following subfunction LastGB, if                           *
 * the computed intermediate weight vector or                             *
 * if the perturbed target weight vector does NOT lie n the correct cone  *
 **************************************************************************/

static ideal LastGB(ideal G, intvec* curr_weight,int tp_deg)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  int i, nV = currRing->N;
  int nwalk=0, endwalks=0, nnwinC=1;
  int nlast = 0;
  ideal Gomega, M, F, Gomega1, Gomega2, M1,F1,result,ssG;
  ring newRing, oldRing, TargetRing;
  intvec* iv_M_lp;
  intvec* target_weight;
  intvec* iv_lp = Mivlp(nV); //define (1,0,...,0)
  intvec* pert_target_vector;
  intvec* ivNull = new intvec(nV);
  intvec* extra_curr_weight = new intvec(nV);
  intvec* next_weight;

#ifndef  BUCHBERGER_ALG
  intvec* hilb_func;
#endif

  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;

  ring EXXRing = currRing;

  // compute a pertubed weight vector of the target weight vector
  if(tp_deg > 1 && tp_deg <= nV)
  {
    //..25.03.03 VMrDefaultlp();//    VMrDefault(target_weight);
    if (rParameter (currRing) != NULL)
    {
      DefRingParlp();
    }
    else
    {
      VMrDefaultlp();
    }
    TargetRing = currRing;
    ssG = idrMoveR(G,EXXRing,currRing);
    iv_M_lp = MivMatrixOrderlp(nV);
    //target_weight = MPertVectorslp(ssG, iv_M_lp, tp_deg);
    target_weight = MPertVectors(ssG, iv_M_lp, tp_deg);
    delete iv_M_lp;
    pert_target_vector = target_weight;

    rChangeCurrRing(EXXRing);
    G = idrMoveR(ssG, TargetRing,currRing);
  }
  else
  {
    target_weight = Mivlp(nV);
  }
  //Print("\n// ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));

  while(1)
  {
    nwalk++;
    nstep++;
#ifdef TIME_TEST
    to=clock();
#endif
   // compute a next weight vector
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif

#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    if(Overflow_Error == TRUE)
    {
      newRing = currRing;
      nnwinC = 0;
      if(tp_deg == 1)
      {
        nlast = 1;
      }
      delete next_weight;

      //idElements(G, "G");
      //Print("\n// ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));

      break;
    }

    if(MivComp(next_weight, ivNull) == 1)
    {
      //Print("\n// ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));
      newRing = currRing;
      delete next_weight;
      break;
    }

    if(MivComp(next_weight, target_weight) == 1)
      endwalks = 1;

    for(i=nV-1; i>=0; i--)
    {
      (*extra_curr_weight)[i] = (*curr_weight)[i];
    }
    /* 06.11.01 NOT Changed */
    for(i=nV-1; i>=0; i--)
    {
      (*curr_weight)[i] = (*next_weight)[i];
    }
    oldRing = currRing;
#ifdef TIME_TEST
    to=clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif

#ifdef ENDWALKS
    if(endwalks == 1)
    {
      Print("\n// ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));
/*
      idElements(Gomega, "Gw");
      headidString(Gomega, "Gw");
*/
    }
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif // BUCHBERGER_ALG

    /* define a new ring that its ordering is "(a(curr_weight),lp) */
    //..25.03.03 VMrDefault(curr_weight);
    if (rParameter (currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a reduced Groebner basis of <Gomega> w.r.t. "newRing" */
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif // BUCHBERGER_ALG
#ifdef TIME_TEST
    xtstd=xtstd+clock()-to;
#endif
    /* change the ring to oldRing */
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a reduced Groebner basis of <G> w.r.t. "newRing" */
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif

    idDelete(&M1);
    idDelete(&G);

    /* change the ring to newRing */
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* reduce the Groebner basis <G> w.r.t. new ring */
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);

    if(endwalks == 1)
    {
      //Print("\n// ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));
      break;
    }

    delete next_weight;
  }//while

  delete ivNull;

  if(tp_deg != 1)
  {
    //..25.03.03 VMrDefaultlp();//define and execute the ring "lp"
    if (rParameter (currRing) != NULL)
    {
      DefRingParlp();
    }
    else
    {
      VMrDefaultlp();
    }
    F1 = idrMoveR(G, newRing,currRing);

    if(nnwinC == 0 || test_w_in_ConeCC(F1, pert_target_vector) != 1)
    {
      oldRing = currRing;
      rChangeCurrRing(newRing);
      G = idrMoveR(F1, oldRing,currRing);
      Print("\n// takes %d steps and calls the recursion of level %d:",
             nwalk, tp_deg-1);

      F1 = LastGB(G,curr_weight, tp_deg-1);
    }

    TargetRing = currRing;
    rChangeCurrRing(EXXRing);
    result = idrMoveR(F1, TargetRing,currRing);
  }
  else
  {
    if(nlast == 1)
    {
      //OMEGA_OVERFLOW_LASTGB:
      /*
      if(MivSame(curr_weight, iv_lp) == 1)
        if (rParameter(currRing) != NULL)
          DefRingParlp();
        else
          VMrDefaultlp();
      else
        if (rParameter(currRing) != NULL)
          DefRingPar(curr_weight);
        else
          VMrDefault(curr_weight);
      */

        //..25.03.03 VMrDefaultlp();//define and execute the ring "lp"
        if (rParameter (currRing) != NULL)
        {
          DefRingParlp();
        }
        else
        {
          VMrDefaultlp();
        }

      F1 = idrMoveR(G, newRing,currRing);
      //Print("\n// Apply \"std\" in ring r%d_%d = %s;\n", tp_deg, nwalk, rString(currRing));

      G = MstdCC(F1);
      idDelete(&F1);
      newRing = currRing;
    }

    rChangeCurrRing(EXXRing);
    result = idrMoveR(G, newRing,currRing);
  }
  delete target_weight;
  delete last_omega;
  delete iv_lp;

  if(Overflow_Error == FALSE)
  {
    Overflow_Error = nError;
  }
  return(result);
}

/**********************************************************
 * check whether a polynomial of G has least 4 monomials  *
 **********************************************************/
static int lengthpoly(ideal G)
{
  int i;
  for(i=IDELEMS(G)-1; i>=0; i--)
  {
    if((G->m[i]!=NULL) /* len >=0 */
       && (G->m[i]->next!=NULL) /* len >=1 */
       && (G->m[i]->next->next!=NULL) /* len >=2 */
       && (G->m[i]->next->next->next!=NULL) /* len >=3 */
       && (G->m[i]->next->next->next->next!=NULL) /* len >=4*/ )
    {
      return 1;
    }
  }
  return 0;
}

/*****************************************
 * return maximal polynomial length of G *
 *****************************************/
static int maxlengthpoly(ideal G)
{
  int i,k,length=0;
  for(i=IDELEMS(G)-1; i>=0; i--)
  {
    k = pLength(G->m[i]);
    if(k>length)
    {
      length = k;
    }
  }
  return length;
}

/*********************************************************
 * check whether a polynomial of G has least 2 monomials *
**********************************************************/
static int islengthpoly2(ideal G)
{
  int i;
  for(i=IDELEMS(G)-1; i>=0; i--)
  {
    if((G->m[i]!=NULL) /* len >=0 */
       && (G->m[i]->next!=NULL) /* len >=1 */
       && (G->m[i]->next->next!=NULL)) /* len >=2 */
    {
      return 1;
    }
  }
  return 0;
}



/* Implementation of the improved Groebner walk algorithm which is written
   by Quoc-Nam Tran (2000).
   One perturbs the original target weight vector, only if
   the next intermediate weight vector is equal to the current target weight
   vector. This must be repeated until the wanted reduced Groebner basis
   to reach.
   If the numbers of variables is big enough, the representation of the origin
   weight vector may be very big. Therefore, it is possible the intermediate
   weight vector doesn't stay in the correct Groebner cone.
   In this case we have just a reduced Groebner basis of the given ideal
   with respect to another monomial order. Then we have to compute
   a wanted reduced Groebner basis of it with respect to the given order.
   At the following subroutine we use the improved Buchberger algorithm or
   the changed perturbation walk algorithm with a decrased degree.
 */

/***************************************
 * return the initial term of an ideal *
 ***************************************/
static ideal idHeadCC(ideal h)
{
  int i, nH =IDELEMS(h);

  ideal m = idInit(nH,h->rank);

  for (i=nH-1;i>=0; i--)
  {
    if (h->m[i]!=NULL)
    {
      m->m[i]=pHead(h->m[i]);
    }
  }
  return m;
}

/**********************************************
 * check whether two head-ideals are the same *
 **********************************************/
static inline int test_G_GB_walk(ideal H0, ideal H1)
{
  int i, nG = IDELEMS(H0);

  if(nG != IDELEMS(H1))
  {
    return 0;
  }
  for(i=nG-1; i>=0; i--)
  {
/*
    poly t;
    if((t=pSub(pCopy(H0->m[i]), pCopy(H1->m[i]))) != NULL)
    {
      pDelete(&t);
      return 0;
    }
    pDelete(&t);
*/
    if(!pEqualPolys(H0->m[i],H1->m[i]))
    {
      return 0;
    }
  }
  return 1;
}

//unused
/*****************************************************
 * find the maximal total degree of polynomials in G *
 *****************************************************/
/*
static int Trandegreebound(ideal G)
{
  int i, nG = IDELEMS(G);
  // int np=1;
  int nV = currRing->N;
  int degtmp, result = 0;
  intvec* ivUnit = Mivdp(nV);

  for(i=nG-1; i>=0; i--)
  {
    // find the maximal total degree of the polynomial G[i]
    degtmp = MwalkWeightDegree(G->m[i], ivUnit);
    if(degtmp > result)
    {
      result = degtmp;
    }
  }
  delete ivUnit;
  return result;
}
*/

//unused
/************************************************************************
 * perturb the weight vector iva w.r.t. the ideal G.                    *
 *  the monomial order of the current ring is the w_1 weight lex. order *
 *  define w := d^(n-1)w_1+ d^(n-2)w_2, ...+ dw_(n-1)+ w_n              *
 *  where d := 1 + max{totdeg(g):g in G}*m, or                          *
 *  d := (2*maxdeg*maxdeg + (nV+1)*maxdeg)*m;                           *
 ************************************************************************/
#if 0
static intvec* TranPertVector(ideal G, intvec* iva)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  int i, j;
  // int nG = IDELEMS(G);
  int nV = currRing->N;

  // define the sequence which expresses the current monomial ordering
  // w_1 = iva; w_2 = (1,0,..,0); w_n = (0,...,0,1,0)
  intvec* ivMat = MivMatrixOrder(iva);

  int  mtmp, m=(*iva)[0];

  for(i=ivMat->length(); i>=0; i--)
  {
    mtmp = (*ivMat)[i];
    if(mtmp <0)
    {
      mtmp = -mtmp;
    }
    if(mtmp > m)
    {
      m = mtmp;
    }
  }

  // define the maximal total degree of polynomials of G
  mpz_t ndeg;
  mpz_init(ndeg);

 // 12 Juli 03
#ifndef UPPER_BOUND
  mpz_set_si(ndeg, Trandegreebound(G)+1);
#else
  mpz_t ztmp;
  mpz_init(ztmp);

  mpz_t maxdeg;
  mpz_init_set_si(maxdeg, Trandegreebound(G));

  //ndeg = (2*maxdeg*maxdeg + (nV+1)*maxdeg)*m;//Kalkbrenner (1999)
  mpz_pow_ui(ztmp, maxdeg, 2);
  mpz_mul_ui(ztmp, ztmp, 2);
  mpz_mul_ui(maxdeg, maxdeg, nV+1);
  mpz_add(ndeg, ztmp, maxdeg);
  mpz_mul_ui(ndeg, ndeg, m);

  mpz_clear(ztmp);

  //PrintS("\n// with the new upper degree bound (2d^2+(n+1)d)*m ");
  //Print("\n//         where d = %d, n = %d and bound = %d", maxdeg, nV, ndeg);
#endif //UPPER_BOUND

#ifdef INVEPS_SMALL_IN_TRAN
  if(mpz_cmp_ui(ndeg, nV)>0 && nV > 3)
  {
    mpz_cdiv_q_ui(ndeg, ndeg, nV);
  }
 //PrintS("\n// choose the \"small\" inverse epsilon:");
 //mpz_out_str(stdout, 10, ndeg);
#endif
  mpz_t deg_tmp;
  mpz_init_set(deg_tmp, ndeg);

  mpz_t *ivres=( mpz_t *) omAlloc(nV*sizeof(mpz_t));
  mpz_init_set_ui(ivres[nV-1],1);

  for(i=nV-2; i>=0; i--)
  {
    mpz_init_set(ivres[i], deg_tmp);
    mpz_mul(deg_tmp, deg_tmp, ndeg);
  }

  mpz_t *ivtmp=(mpz_t *)omAlloc(nV*sizeof(mpz_t));
  for(i=0; i<nV; i++)
  {
    mpz_init(ivtmp[i]);
  }
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  intvec* repr_vector = new intvec(nV);

  // define ivtmp := ndeg^(n-1).w_1 + ndeg^(n-2).w_2 + ... + w_n
  for(i=0; i<nV; i++)
  {
    for(j=0; j<nV; j++)
    {
      if( (*ivMat)[i*nV+j] >= 0 )
      {
        mpz_mul_ui(ivres[i], ivres[i], (*ivMat)[i*nV+j]);
      }
      else
      {
        mpz_mul_ui(ivres[i], ivres[i], -(*ivMat)[i*nV+j]);
        mpz_neg(ivres[i], ivres[i]);
      }
      mpz_add(ivtmp[j], ivtmp[j], ivres[i]);
    }
  }
  delete ivMat;

  int ntrue=0;
  for(i=0; i<nV; i++)
  {
    (*repr_vector)[i] = mpz_get_si(ivtmp[i]);
    if(mpz_cmp(ivtmp[i], sing_int)>=0)
    {
      ntrue++;
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = TRUE;

        PrintS("\n// ** OVERFLOW in \"Repr.Vector\": ");
        mpz_out_str( stdout, 10, ivtmp[i]);
        PrintS(" is greater than 2147483647 (max. integer representation)");
        Print("\n//  So vector[%d] := %d is wrong!!\n",i+1,(*repr_vector)[i]);
      }
    }
  }
  if(Overflow_Error == TRUE)
  {
    ivString(repr_vector, "repvector");
    Print("\n// %d element(s) of it are overflow!!", ntrue);
  }

  if(Overflow_Error == FALSE)
    Overflow_Error=nError;

  omFree(ivres);
  omFree(ivtmp);

  mpz_clear(sing_int);
  mpz_clear(deg_tmp);
  mpz_clear(ndeg);

  return repr_vector;
}
#endif

//unused
#if 0
static intvec* TranPertVector_lp(ideal G)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;
  // int j, nG = IDELEMS(G);
  int i;
  int nV = currRing->N;

  // define the maximal total degree of polynomials of G
  mpz_t ndeg;
  mpz_init(ndeg);

 // 12 Juli 03
#ifndef UPPER_BOUND
  mpz_set_si(ndeg, Trandegreebound(G)+1);
#else
  mpz_t ztmp;
  mpz_init(ztmp);

  mpz_t maxdeg;
  mpz_init_set_si(maxdeg, Trandegreebound(G));

  //ndeg = (2*maxdeg*maxdeg + (nV+1)*maxdeg);//Kalkbrenner (1999)
  mpz_pow_ui(ztmp, maxdeg, 2);
  mpz_mul_ui(ztmp, ztmp, 2);
  mpz_mul_ui(maxdeg, maxdeg, nV+1);
  mpz_add(ndeg, ztmp, maxdeg);
  // PrintS("\n// with the new upper degree bound (2d^2+(n+1)d)*m ");
  // Print("\n//         where d = %d, n = %d and bound = %d",
  // mpz_get_si(maxdeg), nV, mpz_get_si(ndeg));

 mpz_clear(ztmp);

#endif

#ifdef INVEPS_SMALL_IN_TRAN
 if(mpz_cmp_ui(ndeg, nV)>0 && nV > 3)
    mpz_cdiv_q_ui(ndeg, ndeg, nV);

 //PrintS("\n// choose the \"small\" inverse epsilon:");
 // mpz_out_str(stdout, 10, ndeg);
#endif

  mpz_t deg_tmp;
  mpz_init_set(deg_tmp, ndeg);

  mpz_t *ivres=(mpz_t *)omAlloc(nV*sizeof(mpz_t));
  mpz_init_set_ui(ivres[nV-1], 1);

  for(i=nV-2; i>=0; i--)
  {
    mpz_init_set(ivres[i], deg_tmp);
    mpz_mul(deg_tmp, deg_tmp, ndeg);
  }

  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  intvec* repr_vector = new intvec(nV);
  int ntrue=0;
  for(i=0; i<nV; i++)
  {
    (*repr_vector)[i] = mpz_get_si(ivres[i]);

    if(mpz_cmp(ivres[i], sing_int)>=0)
    {
      ntrue++;
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = TRUE;
        PrintS("\n// ** OVERFLOW in \"Repr.Vector\": ");
        mpz_out_str( stdout, 10, ivres[i]);
        PrintS(" is greater than 2147483647 (max. integer representation)");
        Print("\n//  So vector[%d] := %d is wrong!!\n",i+1,(*repr_vector)[i]);
      }
    }
  }
  if(Overflow_Error == TRUE)
  {
    ivString(repr_vector, "repvector");
    Print("\n// %d element(s) of it are overflow!!", ntrue);
  }
  if(Overflow_Error == FALSE)
    Overflow_Error = nError;

  omFree(ivres);

  mpz_clear(ndeg);
  mpz_clear(sing_int);

  return repr_vector;
}
#endif

//unused
#if 0
static intvec* RepresentationMatrix_Dp(ideal G, intvec* M)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  int i, j;
  int nV = currRing->N;

  intvec* ivUnit = Mivdp(nV);
  int degtmp, maxdeg = 0;

  for(i=IDELEMS(G)-1; i>=0; i--)
  {
    // find the maximal total degree of the polynomial G[i]
    degtmp = MwalkWeightDegree(G->m[i], ivUnit);
    if(degtmp > maxdeg)
      maxdeg = degtmp;
  }

  mpz_t ztmp;
  mpz_init_set_si(ztmp, maxdeg);
  mpz_t *ivres=(mpz_t *)omAlloc(nV*sizeof(mpz_t));
  mpz_init_set_ui(ivres[nV-1], 1); // (*ivres)[nV-1] = 1;

  for(i=nV-2; i>=0; i--)
  {
    mpz_init_set(ivres[i], ztmp); //(*ivres)[i] = ztmp;
    mpz_mul_ui(ztmp, ztmp, maxdeg); //ztmp *=maxdeg;
  }

  mpz_t *ivtmp=(mpz_t*)omAlloc(nV*sizeof(mpz_t));
  for(i=0; i<nV; i++)
    mpz_init(ivtmp[i]);

  // define ivtmp := ndeg^(n-1).w_1 + ndeg^(n-2).w_2 + ... + w_n
  for(i=0; i<nV; i++)
    for(j=0; j<nV; j++)
    {
      if((*M)[i*nV+j] < 0)
      {
        mpz_mul_ui(ztmp, ivres[i], -(*M)[i*nV+j]);
        mpz_neg(ztmp, ztmp);
      }
      else
        mpz_mul_ui(ztmp, ivres[i], (*M)[i*nV+j]);

      mpz_add(ivtmp[j], ivtmp[j], ztmp);
    }
  delete ivres;
  mpz_t sing_int;
  mpz_init_set_ui(sing_int,  2147483647);

  int ntrue=0;
  intvec* repvector = new intvec(nV);
  for(i=0; i<nV; i++)
  {
    (*repvector)[i] = mpz_get_si(ivtmp[i]);
    if(mpz_cmp(ivtmp[i], sing_int)>0)
    {
      ntrue++;
      if(Overflow_Error == FALSE)
      {
        Overflow_Error = TRUE;
        PrintS("\n// ** OVERFLOW in \"Repr.Matrix\": ");
        mpz_out_str( stdout, 10, ivtmp[i]);
        PrintS(" is greater than 2147483647 (max. integer representation)");
        Print("\n//  So vector[%d] := %d is wrong!!\n",i+1,(*repvector)[i]);
      }
    }
  }
  if(Overflow_Error == TRUE)
  {
    ivString(repvector, "repvector");
    Print("\n// %d element(s) of it are overflow!!", ntrue);
  }

  if(Overflow_Error == FALSE)
    Overflow_Error = nError;

  mpz_clear(sing_int);
  mpz_clear(ztmp);
  omFree(ivtmp);
  omFree(ivres);
  return repvector;
}
#endif

/*****************************************************************************
 * The following subroutine is the implementation of our first improved      *
 * Groebner walk algorithm, i.e. the first altervative algorithm.            *
 * First we use the Grobner walk algorithm and then we call the changed      *
 * perturbation walk algorithm with decreased degree, if an intermediate     *
 * weight vector is equal to the current target weight vector.               *
 * This call will be only repeated until we get the wanted reduced Groebner  *
 * basis or n times, where n is the numbers of variables.                    *
 *****************************************************************************/

// npwinc = 0, if curr_weight doesn't stay in the correct Groebner cone
static ideal Rec_LastGB(ideal G, intvec* curr_weight,
                        intvec* orig_target_weight, int tp_deg, int npwinc)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;
  // BOOLEAN nOverflow_Error = FALSE;

#ifdef TIME_TEST
  clock_t tproc=0;
  clock_t tinput = clock();
#endif

  int i,  nV = currRing->N;
  int nwalk=0, endwalks=0, nnwinC=1;
  int nlast = 0;
  ideal Gomega, M, F, Gomega1, Gomega2, M1,F1,result,ssG;
  ring newRing, oldRing, TargetRing;
  intvec* iv_M_lp;
  intvec* target_weight;
  intvec* ivNull = new intvec(nV); //define (0,...,0)
  ring EXXRing = currRing;
  //int NEG=0; //19 juni 03
  intvec* next_weight;
#ifndef  BUCHBERGER_ALG
  //08 Juli 03
  intvec* hilb_func;
#endif
  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  BOOLEAN isGB = FALSE;

  // compute a pertubed weight vector of the target weight vector
  if(tp_deg > 1 && tp_deg <= nV)
  {
    ideal H0 = idHeadCC(G);

    if (rParameter (currRing) != NULL)
    {
      DefRingParlp();
    }
    else
    {
      VMrDefaultlp();
    }
    TargetRing = currRing;
    ssG = idrMoveR(G,EXXRing,currRing);

    ideal H0_tmp = idrMoveR(H0,EXXRing,currRing);
    ideal H1 = idHeadCC(ssG);

    // Apply Lemma 2.2 in Collart et. al (1997) to check whether cone(k-1) is equal to cone(k)
    if(test_G_GB_walk(H0_tmp,H1)==1)
    {
      idDelete(&H0_tmp);
      idDelete(&H1);
      G = ssG;
      ssG = NULL;
      newRing = currRing;
      delete ivNull;

      if(npwinc != 0)
      {
        goto LastGB_Finish;
      }
      else
      {
        isGB = TRUE;
        goto KSTD_Finish;
      }
    }
    idDelete(&H0_tmp);
    idDelete(&H1);

    iv_M_lp = MivMatrixOrderlp(nV);
    target_weight  = MPertVectors(ssG, iv_M_lp, tp_deg);
    delete iv_M_lp;
    //PrintS("\n// Input is not GB!!");
    rChangeCurrRing(EXXRing);
    G = idrMoveR(ssG, TargetRing,currRing);

    if(Overflow_Error == TRUE)
    {
      //nOverflow_Error = Overflow_Error;
      //NEG = 1;
      newRing = currRing;
      goto JUNI_STD;
    }
  }

  while(1)
  {
    nwalk ++;
    nstep++;

    if(nwalk==1)
    {
      goto FIRST_STEP;
    }
#ifdef TIME_TEST
    to=clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    // defiNe a new ring that its ordering is "(a(curr_weight),lp)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif // BUCHBERGER_ALG
#ifdef TIME_TEST
    xtstd=xtstd+clock()-to;
#endif
    // change the ring to oldRing
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
     to=clock();
#endif
    // compute a reduced Groebner basis of <G> w.r.t. "newRing" by the lifting process
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif
    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // reduce the Groebner basis <G> w.r.t. new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);

    if(endwalks == 1)
    {
      break;
    }
  FIRST_STEP:
#ifdef TIME_TEST
    to=clock();
#endif
    Overflow_Error = FALSE;
    // compute a next weight vector
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif
    if(Overflow_Error == TRUE)
    {
      //PrintS("\n// ** The next vector does NOT stay in Cone!!\n");
#ifdef TEST_OVERFLOW
      goto  LastGB_Finish;
#endif

      nnwinC = 0;
      if(tp_deg == nV)
      {
        nlast = 1;
      }
      delete next_weight;
      break;
    }

    if(MivComp(next_weight, ivNull) == 1)
    {
      //newRing = currRing;
      delete next_weight;
      break;
    }

    if(MivComp(next_weight, target_weight) == 1)
    {
      if(tp_deg == nV)
      {
        endwalks = 1;
      }
      else
      {
        // REC_LAST_GB_ALT2:
        //nOverflow_Error = Overflow_Error;
#ifdef TIME_TEST
        tproc=tproc+clock()-tinput;
#endif

        /*Print("\n// takes %d steps and calls \"Rec_LastGB\" (%d):",
        nwalk, tp_deg+1);
        */
        G = Rec_LastGB(G,curr_weight, orig_target_weight, tp_deg+1,nnwinC);
        newRing = currRing;
        delete next_weight;
        break;
      }
    }

    for(i=nV-1; i>=0; i--)
    {
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }//while

  delete ivNull;

  if(tp_deg != nV)
  {
    newRing = currRing;

    if (rParameter(currRing) != NULL)
    {
      DefRingParlp();
    }
    else
    {
      VMrDefaultlp();
    }
    F1 = idrMoveR(G, newRing,currRing);

    if(nnwinC == 0 || test_w_in_ConeCC(F1, target_weight) != 1 )
    {
      // nOverflow_Error = Overflow_Error;
      //Print("\n//  takes %d steps and calls \"Rec_LastGB (%d):", tp_deg+1);
#ifdef TIME_TEST
      tproc=tproc+clock()-tinput;
#endif
      F1 = Rec_LastGB(F1,curr_weight, orig_target_weight, tp_deg+1,nnwinC);
    }
    delete target_weight;

    TargetRing = currRing;
    rChangeCurrRing(EXXRing);
    result = idrMoveR(F1, TargetRing,currRing);
  }
  else
  {
    if(nlast == 1)
    {
      JUNI_STD:

      newRing = currRing;
      if (rParameter(currRing) != NULL)
      {
        DefRingParlp();
      }
      else
      {
        VMrDefaultlp();
      }
      KSTD_Finish:
      if(isGB == FALSE)
      {
        F1 = idrMoveR(G, newRing,currRing);
      }
      else
      {
        F1 = G;
      }
#ifdef TIME_TEST
      to=clock();
#endif
      // Print("\n// apply the Buchberger's alg in ring = %s",rString(currRing));
      // idElements(F1, "F1");
      G = MstdCC(F1);
#ifdef TIME_TEST
      xtextra=xtextra+clock()-to;
#endif


      idDelete(&F1);
      newRing = currRing;
    }

    LastGB_Finish:
    rChangeCurrRing(EXXRing);
    result = idrMoveR(G, newRing,currRing);
  }

  if(Overflow_Error == FALSE)
    {
    Overflow_Error=nError;
    }
#ifdef TIME_TEST
   //Print("\n// \"Rec_LastGB\" (%d) took %d steps and %.2f sec.Overflow_Error (%d)", tp_deg, nwalk, ((double) tproc)/1000000, nOverflow_Error);
#endif
  return(result);
}

/* The following subroutine is the implementation of our second improved
   Groebner walk algorithm, i.e. the second altervative algorithm.
   First we use the Grobner walk algorithm and then we call the changed
   perturbation walk algorithm with increased degree, if an intermediate
   weight vector is equal to the current target weight vector.
   This call will be only repeated until we get the wanted reduced Groebner
   basis or n times, where n is the numbers of variables.
*/

/******************************
 * walk + recursive LastGB    *
 ******************************/
ideal MAltwalk2(ideal Go, intvec* curr_weight, intvec* target_weight)
{
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //BOOLEAN nOverflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
#ifdef TIME_TEST
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0; xtextra=0;
  xftinput = clock();
  clock_t tostd, tproc;
#endif
  nstep = 0;
  int i, nV = currRing->N;
  int nwalk=0, endwalks=0;
  // int nhilb = 1;
  ideal Gomega, M, F, Gomega1, Gomega2, M1, F1, G;
  //ideal  G1;
  //ring endRing;
  ring newRing, oldRing;
  intvec* ivNull = new intvec(nV);
  intvec* next_weight;
  //intvec* extra_curr_weight = new intvec(nV);
  //intvec* hilb_func;
  intvec* exivlp = Mivlp(nV);
  ring XXRing = currRing;

  //Print("\n// ring r_input = %s;", rString(currRing));
#ifdef TIME_TEST
  to = clock();
#endif
  /* compute the reduced Groebner basis of the given ideal w.r.t.
     a "fast" monomial order, e.g. degree reverse lex. order (dp) */
  G = MstdCC(Go);
#ifdef TIME_TEST
  tostd=clock()-to;

  Print("\n// Computation of the first std took = %.2f sec",
        ((double) tostd)/1000000);
#endif
  if(currRing->order[0] == ringorder_a)
  {
    goto NEXT_VECTOR;
  }
  while(1)
  {
    nwalk ++;
    nstep ++;
#ifdef TIME_TEST
    to = clock();
#endif
    /* compute an initial form ideal of <G> w.r.t. "curr_vector" */
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif
/*
    if(Overflow_Error == TRUE)
    {
      for(i=nV-1; i>=0; i--)
        (*curr_weight)[i] = (*extra_curr_weight)[i];
      delete extra_curr_weight;
      goto LAST_GB_ALT2;
    }
*/
    oldRing = currRing;

    /* define a new ring that its ordering is "(a(curr_weight),lp) */
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);
#ifdef TIME_TEST
    to = clock();
#endif
    /* compute a reduced Groebner basis of <Gomega> w.r.t. "newRing" */
    M = MstdhomCC(Gomega1);
#ifdef TIME_TEST
    xtstd=xtstd+clock()-to;
#endif
    /* change the ring to oldRing */
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);
#ifdef TIME_TEST
    to = clock();
#endif
    /* compute the reduced Groebner basis of <G> w.r.t. "newRing"
       by the liftig process */
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif
    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    /* change the ring to newRing */
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);
#ifdef TIME_TEST
    to = clock();
#endif
    /* reduce the Groebner basis <G> w.r.t. newRing */
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);

    if(endwalks == 1)
      break;

  NEXT_VECTOR:
#ifdef TIME_TEST
    to = clock();
#endif
    /* compute a next weight vector */
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    if(Overflow_Error == TRUE)
    {
      /*
        ivString(next_weight, "omega");
        PrintS("\n// ** The weight vector does NOT stay in Cone!!\n");
      */
#ifdef TEST_OVERFLOW
      goto  TEST_OVERFLOW_OI;
#endif

      newRing = currRing;
      if (rParameter(currRing) != NULL)
      {
        DefRingPar(target_weight);
      }
      else
      {
        rChangeCurrRing(VMrDefault(target_weight)); // Aenderung
      }
      F1 = idrMoveR(G, newRing,currRing);
      G = MstdCC(F1);
      idDelete(&F1);
      newRing = currRing;
      break;
    }

    if(MivComp(next_weight, ivNull) == 1)
    {
      newRing = currRing;
      delete next_weight;
      break;
    }

    if(MivComp(next_weight, target_weight) == 1)
    {
      if(MivSame(target_weight, exivlp)==1)
      {
     // LAST_GB_ALT2:
        //nOverflow_Error = Overflow_Error;
#ifdef TIME_TEST
        tproc = clock()-xftinput;
#endif
        //Print("\n// takes %d steps and calls the recursion of level 2:",  nwalk);
        /* call the changed perturbation walk algorithm with degree 2 */
        G = Rec_LastGB(G, curr_weight, target_weight, 2,1);
        newRing = currRing;
        delete next_weight;
        break;
      }
      endwalks = 1;
    }

    for(i=nV-1; i>=0; i--)
    {
      //(*extra_curr_weight)[i] = (*curr_weight)[i];
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }
#ifdef TEST_OVERFLOW
 TEST_OVERFLOW_OI:
#endif
  rChangeCurrRing(XXRing);
  G = idrMoveR(G, newRing,currRing);
  delete ivNull;
  delete exivlp;

#ifdef TIME_TEST
  /*Print("\n// \"Main procedure\"  took %d steps dnd %.2f sec. Overflow_Error (%d)",
        nwalk, ((double) tproc)/1000000, nOverflow_Error);
*/
  TimeStringFractal(xftinput, tostd, xtif, xtstd, xtextra,xtlift, xtred,xtnw);

  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// Overflow_Error? (%d)", nOverflow_Error);
  //Print("\n// Awalk2 took %d steps!!", nstep);
#endif

  return(G);
}


/**************************************
 * perturb the matrix order of  "lex" *
 **************************************/
static intvec* NewVectorlp(ideal I)
{
  int nV = currRing->N;
  intvec* iv_wlp =  MivMatrixOrderlp(nV);
  intvec* result = Mfpertvector(I, iv_wlp);
  delete iv_wlp;
  return result;
}

int ngleich;
intvec* Xsigma;
intvec* Xtau;
int xn;
intvec* Xivinput;
intvec* Xivlp;


/********************************
 * compute a next weight vector *
 ********************************/
static intvec* MWalkRandomNextWeight(ideal G, intvec* orig_M, intvec* target_weight,
       int weight_rad, int pert_deg)
{
  assume(currRing != NULL && orig_M != NULL &&
         target_weight != NULL && G->m[0] != NULL);

  //BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  BOOLEAN found_random_weight = FALSE;
  int i,nV = currRing->N;
  intvec* curr_weight = new intvec(nV);

  for(i=0; i<nV; i++)
  {
    (*curr_weight)[i] = (*orig_M)[i];
  }

  int k=0,weight_norm;
  intvec* next_weight;
  intvec* next_weight1 = MkInterRedNextWeight(curr_weight,target_weight,G);
  intvec* next_weight2 = new intvec(nV);
  intvec* result = new intvec(nV);
  intvec* curr_weight1;
  ideal G_test, G_test1, G_test2;

  //try to find a random next weight vector "next_weight2"
  if(weight_rad > 0)
  {
    while(k<10)
    {
      weight_norm = 0;
      while(weight_norm == 0)
      {
        for(i=0; i<nV; i++)
        {
          (*next_weight2)[i] = rand() % 60000 - 30000;
          weight_norm = weight_norm + (*next_weight2)[i]*(*next_weight2)[i];
        }
        weight_norm = 1 + static_cast<int>(sqrt(double(weight_norm)));
      }
      for(i=0; i<nV; i++)
      {
        if((*next_weight2)[i] < 0)
        {
          (*next_weight2)[i] = 1 + (*curr_weight)[i] + weight_rad*(*next_weight2)[i]/weight_norm;
        }
        else
        {
          (*next_weight2)[i] = (*curr_weight)[i] + weight_rad*(*next_weight2)[i]/weight_norm;
        }
      }
      if(test_w_in_ConeCC(G,next_weight2) == 1)
      {
        if(maxlengthpoly(MwalkInitialForm(G,next_weight2))<2)
        {
          next_weight2 = MkInterRedNextWeight(next_weight2,target_weight,G);
        }
        G_test2 = MwalkInitialForm(G, next_weight2);
        found_random_weight = TRUE;
        break;
      }
      k++;
    }
  }

  // compute "perturbed" next weight vector
  if(pert_deg > 1)
  {
    curr_weight1 = MPertVectors(G,orig_M,pert_deg);
    next_weight = MkInterRedNextWeight(curr_weight1,target_weight,G);
    delete curr_weight1;
  }
  else
  {
    next_weight = MkInterRedNextWeight(curr_weight,target_weight,G);
  }
  if(MivSame(curr_weight,next_weight)==1 || Overflow_Error == TRUE)
  {
    Overflow_Error = FALSE;
    delete next_weight;
    next_weight = MkInterRedNextWeight(curr_weight,target_weight,G);
  }
  G_test=MwalkInitialForm(G,next_weight);
  G_test1=MwalkInitialForm(G,next_weight1);

  // compare next weights
  if(Overflow_Error == FALSE)
  {
    if(found_random_weight == TRUE)
    {
    // random next weight vector found
      if(G_test1->m[0] != NULL && maxlengthpoly(G_test1) < maxlengthpoly(G_test))
      {
        if(G_test2->m[0] != NULL && maxlengthpoly(G_test2) < maxlengthpoly(G_test1))
        {
          for(i=0; i<nV; i++)
          {
            (*result)[i] = (*next_weight2)[i];
          }
        }
        else
        {
          for(i=0; i<nV; i++)
          {
            (*result)[i] = (*next_weight1)[i];
          }
        }
      }
      else
      {
        if(G_test2->m[0] != NULL && maxlengthpoly(G_test2) < maxlengthpoly(G_test))
        {
          for(i=0; i<nV; i++)
          {
            (*result)[i] = (*next_weight2)[i];
          }
        }
        else
        {
          for(i=0; i<nV; i++)
          {
            (*result)[i] = (*next_weight)[i];
          }
        }
      }
    }
    else
    {
      // no random next weight vector found
      if(G_test1->m[0] != NULL && maxlengthpoly(G_test1) < maxlengthpoly(G_test))
      {
       for(i=0; i<nV; i++)
        {
          (*result)[i] = (*next_weight1)[i];
        }
      }
      else
      {
        for(i=0; i<nV; i++)
        {
          (*result)[i] = (*next_weight)[i];
        }
      }
    }
  }
  else
  {
    Overflow_Error = FALSE;
    if(found_random_weight == TRUE)
    {
      if(G_test2->m[0] != NULL && maxlengthpoly(G_test2) < maxlengthpoly(G_test))
      {
        for(i=1; i<nV; i++)
        {
          (*result)[i] = (*next_weight2)[i];
        }
      }
      else
      {
        for(i=0; i<nV; i++)
        {
          (*result)[i] = (*next_weight)[i];
        }
      }
    }
    else
    {
      for(i=0; i<nV; i++)
      {
        (*result)[i] = (*next_weight)[i];
      }
    }
  }

  delete next_weight;
  delete next_weight2;
  idDelete(&G_test);
  idDelete(&G_test1);
  if(found_random_weight == TRUE)
  {
    idDelete(&G_test2);
  }
  if(test_w_in_ConeCC(G, result) == 1 && MivSame(curr_weight,result)==0)
  {
    delete curr_weight;
    delete next_weight1;
    return result;
  }
  else
  {
    delete curr_weight;
    delete result;
    return next_weight1;
  }
}


/***************************************************************************
 * The procedur REC_GB_Mwalk computes a GB for <G> w.r.t. the weight order *
 * otw, where G is a reduced GB w.r.t. the weight order cw.                *
 * The new procedure Mwalk calls REC_GB.                                   *
 ***************************************************************************/
static ideal REC_GB_Mwalk(ideal G, intvec* curr_weight, intvec* orig_target_weight,
                          int tp_deg, int npwinc)
{
  BOOLEAN nError = Overflow_Error;
  Overflow_Error = FALSE;

  int i,  nV = currRing->N;
  int nwalk=0, endwalks=0, nnwinC=1, nlast = 0;
  ideal Gomega, M, F, Gomega1, Gomega2, M1,F1,result,ssG;
  ring newRing, oldRing, TargetRing;
  intvec* target_weight;
  intvec* ivNull = new intvec(nV);
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;
#endif
  BOOLEAN isGB = FALSE;

  ring EXXRing = currRing;

  // compute a pertubed weight vector of the target weight vector
  if(tp_deg > 1 && tp_deg <= nV)
  {
    ideal H0 = idHeadCC(G);
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(orig_target_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(orig_target_weight));
    }
    TargetRing = currRing;
    ssG = idrMoveR(G,EXXRing,currRing);

    ideal H0_tmp = idrMoveR(H0,EXXRing,currRing);
    ideal H1 = idHeadCC(ssG);
    id_Delete(&H0,EXXRing);

    if(test_G_GB_walk(H0_tmp,H1)==1)
    {
      //Print("\n//REC_GB_Mwalk: input in %d-th recursive is a GB!\n",tp_deg);
      idDelete(&H0_tmp);
      idDelete(&H1);
      G = ssG;
      ssG = NULL;
      newRing = currRing;
      delete ivNull;
      if(npwinc == 0)
      {
        isGB = TRUE;
        goto KSTD_Finish;
      }
      else
      {
        goto LastGB_Finish;
      }
    }
    idDelete(&H0_tmp);
    idDelete(&H1);

    target_weight  = MPertVectors(ssG, MivMatrixOrder(orig_target_weight), tp_deg);

    rChangeCurrRing(EXXRing);
    G = idrMoveR(ssG, TargetRing,currRing);
  }

  while(1)
  {
    nwalk ++;
    nstep++;
    if(nwalk == 1)
    {
      goto NEXT_STEP;
    }
    //Print("\n//REC_GB_Mwalk: Entering the %d-th step in the %d-th recursive:\n",nwalk,tp_deg);
#ifdef TIME_TEST
    to = clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif = xtif + clock()-to;
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif

    oldRing = currRing;

    // define a new ring with ordering "(a(curr_weight),lp)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
    to = clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif
#ifdef TIME_TEST
    xtstd = xtstd + clock() - to;
#endif

    // change the ring to oldRing
    rChangeCurrRing(oldRing);

    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
    to = clock();
#endif
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift = xtlift + clock() -to;
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);


    // change the ring to newRing
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);

#ifdef TIME_TEST
    to = clock();
#endif
    // reduce the Groebner basis <G> w.r.t. new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred = xtred + clock() -to;
#endif

    idDelete(&F1);

    if(endwalks == 1)
    {
      break;
    }
  NEXT_STEP:
#ifdef TIME_TEST
    to = clock();
#endif
    // compute a next weight vector
    intvec* next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);


#ifdef TIME_TEST
    xtnw = xtnw + clock() - to;
#endif

#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    if(Overflow_Error == TRUE)
    {
      //PrintS("\n//REC_GB_Mwalk: The computed vector does NOT stay in the correct cone!!\n");
      nnwinC = 0;
      if(tp_deg == nV)
      {
        nlast = 1;
      }
      delete next_weight;
      break;
    }
    if(MivComp(next_weight, ivNull) == 1)
    {
      newRing = currRing;
      delete next_weight;
      break;
    }

    if(MivComp(next_weight, target_weight) == 1)
    {
      if(tp_deg == nV)
      {
        endwalks = 1;
      }
      else
      {
        G = REC_GB_Mwalk(G,curr_weight, orig_target_weight, tp_deg+1,nnwinC);
        newRing = currRing;
        delete next_weight;
        break;
      }
    }

    for(i=nV-1; i>=0; i--)
    {
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }

  delete ivNull;

  if(tp_deg != nV)
  {
    newRing = currRing;

    if (rParameter(currRing) != NULL)
    {
      DefRingPar(orig_target_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(orig_target_weight));
    }
    F1 = idrMoveR(G, newRing,currRing);

    if(nnwinC == 0)
    {
      F1 = REC_GB_Mwalk(F1,curr_weight, orig_target_weight, tp_deg+1,nnwinC);
    }
    else
    {
      if(test_w_in_ConeCC(F1, target_weight) != 1)
      {
        F1 = REC_GB_Mwalk(F1,curr_weight, orig_target_weight,tp_deg+1,nnwinC);
      }
    }
    delete target_weight;

    TargetRing = currRing;
    rChangeCurrRing(EXXRing);
    result = idrMoveR(F1, TargetRing,currRing);
  }
  else
  {
    if(nlast == 1)
    {
      if (rParameter(currRing) != NULL)
      {
        DefRingPar(orig_target_weight);
      }
      else
      {
        rChangeCurrRing(VMrDefault(orig_target_weight));
      }
    KSTD_Finish:
      if(isGB == FALSE)
      {
        F1 = idrMoveR(G, newRing,currRing);
      }
      else
      {
        F1 = G;
      }
#ifdef TIME_TEST
      to=clock();
#endif
      // apply Buchberger alg to compute a red. GB of F1
      G = MstdCC(F1);
#ifdef TIME_TEST
      xtextra=clock()-to;
#endif
      idDelete(&F1);
      newRing = currRing;
    }

  LastGB_Finish:
    rChangeCurrRing(EXXRing);
    result = idrMoveR(G, newRing,currRing);
  }

  if(Overflow_Error == FALSE)
    {
    Overflow_Error = nError;
    }
#ifndef BUCHBERGER_ALG
  delete last_omega;
#endif
  return(result);
}


// THE NEW GROEBNER WALK ALGORITHM
// Groebnerwalk with a recursive "second" alternative GW, called REC_GB_Mwalk that only computes the last reduced GB
ideal MwalkAlt(ideal Go, intvec* curr_weight, intvec* target_weight)
{
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());

#ifdef TIME_TEST
  clock_t tinput, tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0;
  tinput = clock();
  clock_t tim;
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0;
#endif
  nstep=0;
  int i;
  int nV = currRing->N;
  int nwalk=0;
  int endwalks=0;

  ideal Gomega, M, F, Gomega1, Gomega2, M1, F1, G;

  ring newRing, oldRing;
  intvec* ivNull = new intvec(nV);
  intvec* exivlp = Mivlp(nV);
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  intvec* tmp_weight = new intvec(nV);
  for(i=nV-1; i>=0; i--)
    (*tmp_weight)[i] = (*curr_weight)[i];

   // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  ring XXRing = currRing;

#ifdef TIME_TEST
  to = clock();
#endif
  // the monomial ordering of this current ring would be "dp"
  G = MstdCC(Go);
#ifdef TIME_TEST
  tostd = clock()-to;
#endif

  if(currRing->order[0] == ringorder_a)
    goto NEXT_VECTOR;

  while(1)
  {
    nwalk ++;
    nstep ++;
#ifdef TIME_TEST
    to = clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    tif = tif + clock()-to;
#endif
    oldRing = currRing;

    if(endwalks == 1)
    {
      /* compute a reduced Groebner basis of Gomega w.r.t. >>_cw by
         the recursive changed perturbation walk alg. */
#ifdef TIME_TEST
      tim = clock();
#endif
#ifdef CHECK_IDEAL_MWALK
        Print("\n// **** Groebnerwalk took %d steps and ", nwalk);
        PrintS("\n// **** call the rec. Pert. Walk to compute a red GB of:");
        idString(Gomega, "Gomega");
#endif

      if(MivSame(exivlp, target_weight)==1)
        M = REC_GB_Mwalk(idCopy(Gomega), tmp_weight, curr_weight, 2,1);
      else
        goto NORMAL_GW;
#ifdef TIME_TEST
        Print("\n//  time for the last std(Gw)  = %.2f sec",
        ((double) (clock()-tim)/1000000));
#endif
/*
#ifdef CHECK_IDEAL_MWALK
      idElements(Gomega, "G_omega");
      headidString(Gomega, "Gw");
      idElements(M, "M");
      //headidString(M, "M");
#endif
*/
#ifdef TIME_TEST
      to = clock();
#endif
      F = MLifttwoIdeal(Gomega, M, G);
#ifdef TIME_TEST
      xtlift = xtlift + clock() - to;
#endif

      idDelete(&Gomega);
      idDelete(&M);
      idDelete(&G);

      oldRing = currRing;

      // create a new ring newRing
       if (rParameter(currRing) != NULL)
       {
         DefRingPar(curr_weight);
       }
       else
       {
         rChangeCurrRing(VMrDefault(curr_weight));
       }
      newRing = currRing;
      F1 = idrMoveR(F, oldRing,currRing);
    }
    else
    {
    NORMAL_GW:
#ifndef  BUCHBERGER_ALG
      if(isNolVector(curr_weight) == 0)
      {
        hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
      }
      else
      {
        hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
      }
#endif // BUCHBERGER_ALG

      // define a new ring that its ordering is "(a(curr_weight),lp)
      if (rParameter(currRing) != NULL)
      {
        DefRingPar(curr_weight);
      }
      else
      {
        rChangeCurrRing(VMrDefault(curr_weight));
      }
      newRing = currRing;
      Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
      to = clock();
#endif
      // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
      M = MstdhomCC(Gomega1);
#else
      M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
      delete hilb_func;
#endif
#ifdef TIME_TEST
      tstd = tstd + clock() - to;
#endif

      // change the ring to oldRing
      rChangeCurrRing(oldRing);
      M1 =  idrMoveR(M, newRing,currRing);
      Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
      to = clock();
#endif
      // compute a representation of the generators of submod (M) with respect
      // to those of mod (Gomega).
      // Gomega is a reduced Groebner basis w.r.t. the current ring.
      F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
      tlift = tlift + clock() - to;
#endif

      idDelete(&M1);
      idDelete(&Gomega2);
      idDelete(&G);

      // change the ring to newRing
      rChangeCurrRing(newRing);
      F1 = idrMoveR(F, oldRing,currRing);
    }

#ifdef TIME_TEST
    to = clock();
#endif
    // reduce the Groebner basis <G> w.r.t. new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    if(endwalks != 1)
    {
      tred = tred + clock() - to;
    }
    else
    {
      xtred = xtred + clock() - to;
    }
#endif
    idDelete(&F1);
    if(endwalks == 1)
    {
      break;
    }
  NEXT_VECTOR:
#ifdef TIME_TEST
    to = clock();
#endif
    // compute a next weight vector
    intvec* next_weight = MkInterRedNextWeight(curr_weight,target_weight,G);
#ifdef TIME_TEST
    tnw = tnw + clock() - to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    //if(test_w_in_ConeCC(G, next_weight) != 1)
    if(Overflow_Error == TRUE)
    {
      newRing = currRing;
      PrintS("\n// ** The computed vector does NOT stay in Cone!!\n");

      if (rParameter(currRing) != NULL)
      {
        DefRingPar(target_weight);
      }
      else
      {
        rChangeCurrRing(VMrDefault(target_weight));
      }
      F1 = idrMoveR(G, newRing,currRing);
      G = MstdCC(F1);
      idDelete(&F1);

      newRing = currRing;
      break;
    }

    if(MivComp(next_weight, ivNull) == 1)
    {
      newRing = currRing;
      delete next_weight;
      break;
    }
    if(MivComp(next_weight, target_weight) == 1)
    {
      endwalks = 1;
    }
    for(i=nV-1; i>=0; i--)
    {
      (*tmp_weight)[i] = (*curr_weight)[i];
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }
  rChangeCurrRing(XXRing);
  G = idrMoveR(G, newRing,currRing);

  delete tmp_weight;
  delete ivNull;
  delete exivlp;

#ifdef TIME_TEST
  TimeString(tinput, tostd, tif, tstd, tlift, tred, tnw, nstep);

  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
#endif
  return(G);
}

/*******************************
 * THE GROEBNER WALK ALGORITHM *
 *******************************/
ideal Mwalk(ideal Go, intvec* orig_M, intvec* target_M,
            ring baseRing, int reduction, int printout)
{
  // save current options
  BITSET save1 = si_opt_1;
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //BOOLEAN endwalks = FALSE;
#ifdef TIME_TEST
  clock_t tinput, tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0;
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0;
  tinput = clock();
  clock_t tim;
#endif
  nstep=0;
  int i,nwalk;
  int nV = baseRing->N;

  ideal Gomega, M, F, FF, Gomega1, Gomega2, M1;
  ring newRing;
  ring XXRing = baseRing;
  ring targetRing;
  intvec* ivNull = new intvec(nV);
  intvec* curr_weight = new intvec(nV);
  intvec* target_weight = new intvec(nV);
  intvec* exivlp = Mivlp(nV);
/*
  intvec* tmp_weight = new intvec(nV);
  for(i=0; i<nV; i++)
  {
    (*tmp_weight)[i] = (*orig_M)[i];
  }
*/
  for(i=0; i<nV; i++)
  {
    (*curr_weight)[i] = (*orig_M)[i];
    (*target_weight)[i] = (*target_M)[i];
  }
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
   // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;
#endif
  rComplete(currRing);
#ifdef CHECK_IDEAL_MWALK
  if(printout > 2)
  {
    idString(Go,"//** Mwalk: Go");
  }
#endif

  if(target_M->length() == nV)
  {
   // define the target ring
    targetRing = VMrDefault(target_weight);
  }
  else
  {
    targetRing = VMatrDefault(target_M);
  }
  if(orig_M->length() == nV)
  {
    // define a new ring with ordering "(a(curr_weight),lp)
    //newRing = VMrDefault(curr_weight);
    newRing=VMrRefine(target_weight, curr_weight);
  }
  else
  {
    newRing = VMatrRefine(target_M,curr_weight); //newRing = VMatrDefault(orig_M);
  }
  rChangeCurrRing(newRing);
  if(printout > 2)
  {
    Print("\n//** Mrwalk: Current ring r = %s;\n", rString(currRing));
  }
#ifdef TIME_TEST
  to = clock();
#endif
  ideal G = MstdCC(idrMoveR(Go,baseRing,currRing));
#ifdef TIME_TEST
  tostd = clock()-to;
#endif

  baseRing = currRing;
  nwalk = 0;

  while(1)
  {
    nwalk ++;
    nstep ++;
    //compute an initial form ideal of <G> w.r.t. "curr_vector"
#ifdef TIME_TEST
    to = clock();
#endif
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    tif = tif + clock()-to;
#endif

#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** Mwalk: Gomega");
    }
#endif

    if(reduction == 0)
    {
      FF = middleOfCone(G,Gomega);
      if(FF != NULL)
      {
      PrintS("middle of Cone");
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        goto NEXT_VECTOR;
      }
    }

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif

    if(nwalk == 1)
    {
      if(orig_M->length() == nV)
      {
        // define a new ring with ordering "(a(curr_weight),lp)
        //newRing = VMrDefault(curr_weight);
        newRing=VMrRefine(target_weight, curr_weight);
      }
      else
      {
        newRing = VMatrRefine(target_M,curr_weight);//newRing = VMatrDefault(orig_M);
      }
    }
    else
    {
     if(target_M->length() == nV)
     {
       //define a new ring with ordering "(a(curr_weight),lp)"
       //newRing = VMrDefault(curr_weight);
       newRing=VMrRefine(target_weight, curr_weight);
     }
     else
     {
       //define a new ring with matrix ordering
       newRing = VMatrRefine(target_M,curr_weight);
     }
    }
    rChangeCurrRing(newRing);
    if(printout > 2)
    {
      Print("\n// Current ring r = %s;\n", rString(currRing));
    }
    Gomega1 = idrMoveR(Gomega, baseRing,currRing);
    idDelete(&Gomega);
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef TIME_TEST
    to = clock();
#endif
#ifndef  BUCHBERGER_ALG
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#else
    M = kStd(Gomega1,NULL,testHomog,NULL,NULL,0,0,NULL);
#endif
#ifdef TIME_TEST
    tstd = tstd + clock() - to;
#endif
    idSkipZeroes(M);
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(M, "//** Mwalk: M");
    }
#endif
    //change the ring to baseRing
    rChangeCurrRing(baseRing);
    M1 =  idrMoveR(M, newRing,currRing);
    idDelete(&M);
    Gomega2 = idrMoveR(Gomega1, newRing,currRing);
    idDelete(&Gomega1);
#ifdef TIME_TEST
    to = clock();
#endif
    // compute a representation of the generators of submod (M) with respect to those of mod (Gomega),
    // where Gomega is a reduced Groebner basis w.r.t. the current ring
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    tlift = tlift + clock() - to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F, "//** Mwalk: F");
    }
#endif
    idDelete(&Gomega2);
    idDelete(&M1);

    rChangeCurrRing(newRing); // change the ring to newRing
    G = idrMoveR(F,baseRing,currRing);
    idDelete(&F);
    idSkipZeroes(G);

#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(G, "//** Mwalk: G");
    }
#endif

    rChangeCurrRing(targetRing);
    G = idrMoveR(G,newRing,currRing);
    // test whether target cone is reached
    if(reduction !=0 && test_w_in_ConeCC(G,curr_weight) == 1)
    {
      baseRing = currRing;
      break;
      //endwalks = TRUE;
    }

    rChangeCurrRing(newRing);
    G = idrMoveR(G,targetRing,currRing);
    baseRing = currRing;

    NEXT_VECTOR:
#ifdef TIME_TEST
    to = clock();
#endif
    intvec* next_weight = MwalkNextWeightCC(curr_weight,target_weight,G);
#ifdef TIME_TEST
    tnw = tnw + clock() - to;
#endif
#ifdef PRINT_VECTORS
    if(printout > 0)
    {
      MivString(curr_weight, target_weight, next_weight);
    }
#endif
    if(reduction ==0)
    {
      if(MivComp(curr_weight,next_weight)==1)
      {
        break;
      }
    }
    if(MivComp(target_weight,curr_weight) == 1)
    {
      break;
    }

    for(i=nV-1; i>=0; i--)
    {
      //(*tmp_weight)[i] = (*curr_weight)[i];
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }
  rChangeCurrRing(XXRing);
  ideal result = idrMoveR(G,baseRing,currRing);
  idDelete(&Go);
  idDelete(&G);
  //delete tmp_weight;
  delete ivNull;
  delete exivlp;
#ifndef BUCHBERGER_ALG
  delete last_omega;
#endif
#ifdef TIME_TEST
  TimeString(tinput, tostd, tif, tstd, tlift, tred, tnw, nstep);
  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
#endif
  if(printout > 0)
  {
    Print("\n//** Mwalk: Groebner Walk took %d steps.\n", nstep);
  }
  si_opt_1 = save1; //set original options
  return(result);
}

// THE RANDOM WALK ALGORITHM
ideal Mrwalk(ideal Go, intvec* orig_M, intvec* target_M, int weight_rad, int pert_deg,
             int reduction, int printout)
{
  BITSET save1 = si_opt_1; // save current options
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }

  Set_Error(FALSE);
  Overflow_Error = FALSE;
#ifdef TIME_TEST
  clock_t tinput, tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0;
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0;
  tinput = clock();
  clock_t tim;
#endif
  nstep=0;
  int i,nwalk;//polylength;
  int nV = currRing->N;

  //check that weight radius is valid
  if(weight_rad < 0)
  {
    WerrorS("Invalid radius.\n");
    return NULL;
  }

  //check that perturbation degree is valid
  if(pert_deg > nV || pert_deg < 1)
  {
    WerrorS("Invalid perturbation degree.\n");
    return NULL;
  }

  ideal Gomega, M, F,FF, Gomega1, Gomega2, M1;
  ring newRing;
  ring targetRing;
  ring baseRing = currRing;
  ring XXRing = currRing;
  intvec* iv_M;
  intvec* ivNull = new intvec(nV);
  intvec* curr_weight = new intvec(nV);
  intvec* target_weight = new intvec(nV);
  intvec* next_weight= new intvec(nV);

  for(i=0; i<nV; i++)
  {
    (*curr_weight)[i] = (*orig_M)[i];
    (*target_weight)[i] = (*target_M)[i];
  }

#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
   // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;
#endif
  rComplete(currRing);

  if(target_M->length() == nV)
  {
    targetRing = VMrDefault(target_weight); // define the target ring
  }
  else
  {
    targetRing = VMatrDefault(target_M);
  }
  if(orig_M->length() == nV)
  {
    //newRing = VMrDefault(curr_weight); // define a new ring with ordering "(a(curr_weight),lp)
    newRing=VMrRefine(target_weight, curr_weight);
  }
  else
  {
    newRing = VMatrRefine(target_M,curr_weight);//newRing = VMatrDefault(orig_M);
  }
  rChangeCurrRing(newRing);
#ifdef TIME_TEST
  to = clock();
#endif
  ideal G = MstdCC(idrMoveR(Go,baseRing,currRing));
#ifdef TIME_TEST
  tostd = clock()-to;
#endif
  baseRing = currRing;
  nwalk = 0;

#ifdef TIME_TEST
  to = clock();
#endif
  Gomega = MwalkInitialForm(G, curr_weight); // compute an initial form ideal of <G> w.r.t. "curr_vector"
#ifdef TIME_TEST
  tif = tif + clock()-to; //time for computing initial form ideal
#endif

  while(1)
  {
    nwalk ++;
    nstep ++;
#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** Mrwalk: Gomega");
    }
#endif
    if(reduction == 0)
    {
      FF = middleOfCone(G,Gomega);
      if(FF != NULL)
      {
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        goto NEXT_VECTOR;
      }
    }
#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif
    if(nwalk == 1)
    {
      if(orig_M->length() == nV)
      {
        /*newRing = VMrDefault(curr_weight); // define a new ring with ordering "(a(curr_weight),lp)*/
        newRing=VMrRefine(target_weight, curr_weight);
      }
      else
      {
        newRing = VMatrRefine(target_M,curr_weight);//newRing = VMatrDefault(orig_M);
      }
    }
    else
    {
     if(target_M->length() == nV)
     {
       /*newRing = VMrDefault(curr_weight); // define a new ring with ordering "(a(curr_weight),lp)*/
       newRing=VMrRefine(target_weight, curr_weight);
     }
     else
     {
       newRing = VMatrRefine(target_M,curr_weight);
     }
    }
    rChangeCurrRing(newRing);
    Gomega1 = idrMoveR(Gomega, baseRing,currRing);
    idDelete(&Gomega);
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef TIME_TEST
    to = clock();
#endif
#ifndef BUCHBERGER_ALG
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#else
    M = kStd(Gomega1,NULL,testHomog,NULL,NULL,0,0,NULL);
#endif
#ifdef TIME_TEST
    tstd = tstd + clock() - to;
#endif
    idSkipZeroes(M);
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(M, "//** Mrwalk: M");
    }
#endif
    //change the ring to baseRing
    rChangeCurrRing(baseRing);
    M1 =  idrMoveR(M, newRing,currRing);
    idDelete(&M);
    Gomega2 = idrMoveR(Gomega1, newRing,currRing);
    idDelete(&Gomega1);
#ifdef TIME_TEST
    to = clock();
#endif
    // compute a representation of the generators of submod (M) with respect to those of mod (Gomega),
    // where Gomega is a reduced Groebner basis w.r.t. the current ring
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    tlift = tlift + clock() - to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F,"//** Mrwalk: F");
    }
#endif
    idDelete(&Gomega2);
    idDelete(&M1);
    rChangeCurrRing(newRing); // change the ring to newRing
    G = idrMoveR(F,baseRing,currRing);
    idDelete(&F);
    baseRing = currRing;
#ifdef TIME_TEST
    to = clock();
    tstd = tstd + clock() - to;
#endif
    idSkipZeroes(G);
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(G,"//** Mrwalk: G");
    }
#endif

    rChangeCurrRing(targetRing);
    G = idrMoveR(G,newRing,currRing);

    // test whether target cone is reached
    if(reduction !=0 && test_w_in_ConeCC(G,curr_weight) == 1)
    {
      baseRing = currRing;
      break;
    }

    rChangeCurrRing(newRing);
    G = idrMoveR(G,targetRing,currRing);
    baseRing = currRing;

    NEXT_VECTOR:
#ifdef TIME_TEST
    to = clock();
#endif
    next_weight = MwalkNextWeightCC(curr_weight,target_weight,G);
#ifdef TIME_TEST
    tnw = tnw + clock() - to;
#endif

#ifdef TIME_TEST
    to = clock();
#endif
    Gomega = MwalkInitialForm(G, next_weight); // compute an initial form ideal of <G> w.r.t. "curr_vector"
#ifdef TIME_TEST
    tif = tif + clock()-to; //time for computing initial form ideal
#endif

    //lengthpoly(Gomega) = 1 if there is a polynomial in Gomega with at least 3 monomials and 0 otherwise
    //polylength = lengthpoly(Gomega);
    if(lengthpoly(Gomega) > 0)
    {
      //there is a polynomial in Gomega with at least 3 monomials,
      //low-dimensional facet of the cone
      delete next_weight;
      if(target_M->length() == nV)
      {
        //iv_M = MivMatrixOrder(curr_weight);
        iv_M = MivMatrixOrderRefine(curr_weight,target_M);
      }
      else
      {
        iv_M = MivMatrixOrderRefine(curr_weight,target_M);
      }
#ifdef TIME_TEST
      to = clock();
#endif
      next_weight = MWalkRandomNextWeight(G, iv_M, target_weight, weight_rad, pert_deg);
#ifdef TIME_TEST
      tnw = tnw + clock() - to;
#endif
      idDelete(&Gomega);
#ifdef TIME_TEST
      to = clock();
#endif
      Gomega = MwalkInitialForm(G, next_weight);
#ifdef TIME_TEST
      tif = tif + clock()-to; //time for computing initial form ideal
#endif
      delete iv_M;
    }

    // test whether target weight vector is reached
    if(MivComp(next_weight, ivNull) == 1 || MivComp(target_weight,curr_weight) == 1)
    {
      baseRing = currRing;
      delete next_weight;
      break;
    }
    if(reduction ==0)
    {
      if(MivComp(curr_weight,next_weight)==1)
      {
        break;
      }
    }
#ifdef PRINT_VECTORS
    if(printout > 0)
    {
      MivString(curr_weight, target_weight, next_weight);
    }
#endif

    for(i=nV-1; i>=0; i--)
    {
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }
  baseRing = currRing;
  rChangeCurrRing(XXRing);
  ideal result = idrMoveR(G,baseRing,currRing);
  idDelete(&G);
  delete ivNull;
#ifndef BUCHBERGER_ALG
  delete last_omega;
#endif
  if(printout > 0)
  {
    Print("\n//** Mrwalk: Groebner Walk took %d steps.\n", nstep);
  }
#ifdef TIME_TEST
  TimeString(tinput, tostd, tif, tstd, tlift, tred, tnw, nstep);
  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
#endif
  si_opt_1 = save1; //set original options
  return(result);
}

/**************************************************************/
/*     Implementation of the perturbation walk algorithm      */
/**************************************************************/
/* If the perturbed target weight vector or an intermediate weight vector
   doesn't stay in the correct Groebner cone, we have only
   a reduced Groebner basis for the given ideal with respect to
   a monomial order which differs to the given order.
   Then we have to compute the wanted  reduced Groebner basis for it.
   For this, we can use
   1) the improved Buchberger algorithm or
   2) the changed perturbation walk algorithm with a decreased degree.
*/
// if nP = 0 use kStd, else call LastGB
ideal Mpwalk(ideal Go, int op_deg, int tp_deg,intvec* curr_weight,
             intvec* target_weight, int nP, int reduction, int printout)
{
  BITSET save1 = si_opt_1; // save current options
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }
  Set_Error(FALSE  );
  Overflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
#ifdef TIME_TEST
  clock_t  tinput, tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0;
  xtextra=0;
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0;
  tinput = clock();

  clock_t tim;
#endif
  nstep = 0;
  int i, ntwC=1, ntestw=1,  nV = currRing->N;

  //check that perturbation degree is valid
  if(op_deg < 1 || tp_deg < 1 || op_deg > nV || tp_deg > nV)
  {
    WerrorS("Invalid perturbation degree.\n");
    return NULL;
  }

  BOOLEAN endwalks = FALSE;
  ideal Gomega, M, F, FF, G, Gomega1, Gomega2, M1,F1,Eresult,ssG;
  ring newRing, oldRing, TargetRing;
  intvec* iv_M_dp;
  intvec* iv_M_lp;
  intvec* exivlp = Mivlp(nV);
  intvec* orig_target = target_weight;
  intvec* pert_target_vector = target_weight;
  intvec* ivNull = new intvec(nV);
  intvec* iv_dp = MivUnit(nV);// define (1,1,...,1)
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  intvec* next_weight;

  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  ring XXRing = currRing;
#ifdef TIME_TEST
  to = clock();
#endif
  // perturbs the original vector
  if(MivComp(curr_weight, iv_dp) == 1) //rOrdStr(currRing) := "dp"
  {
    G = MstdCC(Go);
#ifdef TIME_TEST
    tostd = clock()-to;
#endif
    if(op_deg != 1){
      iv_M_dp = MivMatrixOrderdp(nV);
      //ivString(iv_M_dp, "iv_M_dp");
      curr_weight = MPertVectors(G, iv_M_dp, op_deg);
    }
  }
  else
  {
    //define ring order := (a(curr_weight),lp);
/*
    if (rParameter(currRing) != NULL)
      DefRingPar(curr_weight);
    else
      rChangeCurrRing(VMrDefault(curr_weight));
*/
    rChangeCurrRing(VMrRefine(target_weight,curr_weight));

    G = idrMoveR(Go, XXRing,currRing);
    G = MstdCC(G);
#ifdef TIME_TEST
    tostd = clock()-to;
#endif
    if(op_deg != 1){
      iv_M_dp = MivMatrixOrder(curr_weight);
      curr_weight = MPertVectors(G, iv_M_dp, op_deg);
    }
  }
  delete iv_dp;
  if(op_deg != 1) delete iv_M_dp;

  ring HelpRing = currRing;

  // perturbs the target weight vector
  if(tp_deg > 1 && tp_deg <= nV)
  {
/*
    if (rParameter(currRing) != NULL)
      DefRingPar(target_weight);
    else
      rChangeCurrRing(VMrDefault(target_weight));
*/
    rChangeCurrRing(VMrRefine(target_weight,curr_weight));

    TargetRing = currRing;
    ssG = idrMoveR(G,HelpRing,currRing);
    if(MivSame(target_weight, exivlp) == 1)
    {
      iv_M_lp = MivMatrixOrderlp(nV);
      target_weight = MPertVectors(ssG, iv_M_lp, tp_deg);
    }
    else
    {
      iv_M_lp = MivMatrixOrder(target_weight);
      target_weight = MPertVectors(ssG, iv_M_lp, tp_deg);
    }
    delete iv_M_lp;
    pert_target_vector = target_weight;
    rChangeCurrRing(HelpRing);
    G = idrMoveR(ssG, TargetRing,currRing);
  }
  if(printout > 0)
  {
    Print("\n//** Mpwalk: Perturbation Walk of degree (%d,%d):",op_deg,tp_deg);
#ifdef PRINT_VECTORS
    ivString(curr_weight, "//** Mpwalk: new current weight");
    ivString(target_weight, "//** Mpwalk: new target weight");
#endif
  }
  while(1)
  {
    nstep ++;
#ifdef TIME_TEST
    to = clock();
#endif
    // compute an initial form ideal of <G> w.r.t. the weight vector
    // "curr_weight"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    tif = tif + clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** Mpwalk: Gomega");
    }
#endif
    if(reduction == 0 && nstep > 1)
    {
      FF = middleOfCone(G,Gomega);
      if(FF != NULL)
      {
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        goto NEXT_VECTOR;
      }
    }

#ifdef ENDWALKS
    if(endwalks == TRUE)
    {
      if(printout > 0)
      {
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
      }
      //idElements(G, "G");
      //headidString(G, "G");
    }
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    // define a new ring with ordering "(a(curr_weight),lp)
/*
    if (rParameter(currRing) != NULL)
      DefRingPar(curr_weight);
    else
      rChangeCurrRing(VMrDefault(curr_weight));
*/
    rChangeCurrRing(VMrRefine(target_weight,curr_weight));

    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef ENDWALKS
    if(endwalks==TRUE)
    {
      if(printout > 0)
      {
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
        //idElements(Gomega1, "Gw");
        //headidString(Gomega1, "headGw");
        PrintS("\n// compute a rGB of Gw:\n");
      }
#ifndef  BUCHBERGER_ALG
      ivString(hilb_func, "w");
#endif
    }
#endif
#ifdef TIME_TEST
    tim = clock();
    to = clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif

    if(endwalks == TRUE)
    {
#ifdef TIME_TEST
      xtstd = xtstd+clock()-to;
#endif
#ifdef ENDWALKS
#ifdef TIME_TEST
      if(printout > 1)
      {
        Print("\n// time for the last std(Gw)  = %.2f sec\n",
            ((double) clock())/1000000 -((double)tim) /1000000);
      }
#endif
#endif
    }
    else
    {
#ifdef TIME_TEST
      tstd=tstd+clock()-to;
#endif
    }
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(M,"//** Mpwalk: M");
    }
#endif
    // change the ring to oldRing
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a representation of the generators of submod (M)
       with respect to those of mod (Gomega).
       Gomega is a reduced Groebner basis w.r.t. the current ring */
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    if(endwalks == FALSE)
      tlift = tlift+clock()-to;
    else
      xtlift=clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F,"//** Mpwalk: F");
    }
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    if(reduction == 0)
    {
      G = idrMoveR(F,oldRing,currRing);
    }
    else
    {
      F1 = idrMoveR(F, oldRing,currRing);
      if(printout > 2)
      {
        PrintS("\n //** Mpwalk: reduce the Groebner basis.\n");
      }
#ifdef TIME_TEST
      to=clock();
#endif
      G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
      if(endwalks == FALSE)
        tred = tred+clock()-to;
      else
        xtred=clock()-to;
#endif
      idDelete(&F1);
    }
    if(endwalks == TRUE)
      break;

    NEXT_VECTOR:
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a next weight vector
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    tnw=tnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    if(printout > 0)
    {
      MivString(curr_weight, target_weight, next_weight);
    }
#endif

    if(Overflow_Error == TRUE)
    {
      ntwC = 0;
      //ntestomega = 1;
      //Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
      //idElements(G, "G");
      delete next_weight;
      goto FINISH_160302;
    }
    if(MivComp(next_weight, ivNull) == 1){
      newRing = currRing;
      delete next_weight;
      //Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
      break;
    }
    if(MivComp(next_weight, target_weight) == 1)
      endwalks = TRUE;

    for(i=nV-1; i>=0; i--)
      (*curr_weight)[i] = (*next_weight)[i];

    delete next_weight;
  }//end of while-loop

  if(tp_deg != 1)
  {
  FINISH_160302:
    if(MivSame(orig_target, exivlp) == 1) {
    /*  if (rParameter(currRing) != NULL)
        DefRingParlp();
      else
        VMrDefaultlp();
    else
      if (rParameter(currRing) != NULL)
        DefRingPar(orig_target);
      else*/
        rChangeCurrRing(VMrDefault(orig_target));
    }
    TargetRing=currRing;
    F1 = idrMoveR(G, newRing,currRing);
/*
#ifdef CHECK_IDEAL_MWALK
      headidString(G, "G");
#endif
*/

    // check whether the pertubed target vector stays in the correct cone
    if(ntwC != 0){
      ntestw = test_w_in_ConeCC(F1, pert_target_vector);
    }

    if( ntestw != 1 || ntwC == 0)
    {
      if(ntestw != 1 && printout >2)
      {
        ivString(pert_target_vector, "tau");
        PrintS("\n// ** perturbed target vector doesn't stay in cone!!");
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
        //idElements(F1, "G");
      }
      // LastGB is "better" than the kStd subroutine
#ifdef TIME_TEST
      to=clock();
#endif
      ideal eF1;
      if(nP == 0 || tp_deg == 1 || MivSame(orig_target, exivlp) != 1){
        // PrintS("\n// ** calls \"std\" to compute a GB");
        eF1 = MstdCC(F1);
        idDelete(&F1);
      }
      else {
        // PrintS("\n// ** calls \"LastGB\" to compute a GB");
        rChangeCurrRing(newRing);
        ideal F2 = idrMoveR(F1, TargetRing,currRing);
        eF1 = LastGB(F2, curr_weight, tp_deg-1);
        F2=NULL;
      }
#ifdef TIME_TEST
      xtextra=clock()-to;
#endif
      ring exTargetRing = currRing;

      rChangeCurrRing(XXRing);
      Eresult = idrMoveR(eF1, exTargetRing,currRing);
    }
    else{
      rChangeCurrRing(XXRing);
      Eresult = idrMoveR(F1, TargetRing,currRing);
    }
  }
  else {
    rChangeCurrRing(XXRing);
    Eresult = idrMoveR(G, newRing,currRing);
  }
  si_opt_1 = save1; //set original options, e. g. option(RedSB)
  delete ivNull;
  if(tp_deg != 1)
    delete target_weight;

  if(op_deg != 1 )
    delete curr_weight;

  delete exivlp;
  delete last_omega;

#ifdef TIME_TEST
  TimeStringFractal(tinput, tostd, tif+xtif, tstd+xtstd,0, tlift+xtlift, tred+xtred,
             tnw+xtnw);

  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// It took %d steps and Overflow_Error? (%d)\n", nstep,  Overflow_Error);
#endif
  if(printout > 0)
  {
    Print("\n//** Mpwalk: Perturbation Walk took %d steps.\n", nstep);
  }
  return(Eresult);
}

/*******************************************************
 * THE PERTURBATION WALK ALGORITHM WITH RANDOM ELEMENT *
 *******************************************************/
ideal Mprwalk(ideal Go, intvec* orig_M, intvec* target_M, int weight_rad,
              int op_deg, int tp_deg, int nP, int reduction, int printout)
{
  BITSET save1 = si_opt_1; // save current options
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
#ifdef TIME_TEST
  clock_t  tinput, tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0;
  xtextra=0;
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0;
  tinput = clock();

  clock_t tim;
#endif
  nstep = 0;
  int i, ntwC=1, ntestw=1, nV = currRing->N; //polylength

  //check that weight radius is valid
  if(weight_rad < 0)
  {
    WerrorS("Invalid radius.\n");
    return NULL;
  }

  //check that perturbation degree is valid
  if(op_deg < 1 || tp_deg < 1 || op_deg > nV || tp_deg > nV)
  {
    WerrorS("Invalid perturbation degree.\n");
    return NULL;
  }

  BOOLEAN endwalks = FALSE;

  ideal Gomega, M, F, FF, G, Gomega1, Gomega2, M1,F1,Eresult,ssG;
  ring newRing, oldRing, TargetRing;
  intvec* iv_M;
  intvec* iv_M_dp;
  intvec* iv_M_lp;
  intvec* exivlp = Mivlp(nV);
  intvec* curr_weight = new intvec(nV);
  intvec* target_weight = new intvec(nV);
  for(i=0; i<nV; i++)
  {
    (*curr_weight)[i] = (*orig_M)[i];
    (*target_weight)[i] = (*target_M)[i];
  }
  intvec* orig_target = target_weight;
  intvec* pert_target_vector = target_weight;
  intvec* ivNull = new intvec(nV);
  intvec* iv_dp = MivUnit(nV);// define (1,1,...,1)
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  intvec* next_weight;

  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  ring XXRing = currRing;

  // perturbs the original vector
  if(orig_M->length() == nV)
  {
    if(MivComp(curr_weight, iv_dp) == 1) //rOrdStr(currRing) := "dp"
    {
#ifdef TIME_TEST
  to = clock();
#endif
      G = MstdCC(Go);
#ifdef TIME_TEST
      tostd = clock()-to;
#endif
      if(op_deg != 1)
      {
        iv_M_dp = MivMatrixOrderdp(nV);
        curr_weight = MPertVectors(G, iv_M_dp, op_deg);
      }
    }
    else
    {
      //define ring order := (a(curr_weight),lp);
      if (rParameter(currRing) != NULL)
        DefRingPar(curr_weight);
      else
        rChangeCurrRing(VMrDefault(curr_weight));

      G = idrMoveR(Go, XXRing,currRing);
#ifdef TIME_TEST
  to = clock();
#endif
      G = MstdCC(G);
#ifdef TIME_TEST
      tostd = clock()-to;
#endif
      if(op_deg != 1)
      {
        iv_M_dp = MivMatrixOrder(curr_weight);
        curr_weight = MPertVectors(G, iv_M_dp, op_deg);
      }
    }
  }
  else
  {
    rChangeCurrRing(VMatrDefault(orig_M));
    G = idrMoveR(Go, XXRing,currRing);
#ifdef TIME_TEST
    to = clock();
#endif
    G = MstdCC(G);
#ifdef TIME_TEST
    tostd = clock()-to;
#endif
    if(op_deg != 1)
    {
      curr_weight = MPertVectors(G, orig_M, op_deg);
    }
  }

  delete iv_dp;
  if(op_deg != 1) delete iv_M_dp;

  ring HelpRing = currRing;

  // perturbs the target weight vector
  if(target_M->length() == nV)
  {
    if(tp_deg > 1 && tp_deg <= nV)
    {
      if (rParameter(currRing) != NULL)
        DefRingPar(target_weight);
      else
        rChangeCurrRing(VMrDefault(target_weight));

      TargetRing = currRing;
      ssG = idrMoveR(G,HelpRing,currRing);
      if(MivSame(target_weight, exivlp) == 1)
      {
        iv_M_lp = MivMatrixOrderlp(nV);
        target_weight = MPertVectors(ssG, iv_M_lp, tp_deg);
      }
      else
      {
        iv_M_lp = MivMatrixOrder(target_weight);
        target_weight = MPertVectors(ssG, iv_M_lp, tp_deg);
      }
      delete iv_M_lp;
      pert_target_vector = target_weight;
      rChangeCurrRing(HelpRing);
      G = idrMoveR(ssG, TargetRing,currRing);
    }
  }
  else
  {
    if(tp_deg > 1 && tp_deg <= nV)
    {
      rChangeCurrRing(VMatrDefault(target_M));
      TargetRing = currRing;
      ssG = idrMoveR(G,HelpRing,currRing);
      target_weight = MPertVectors(ssG, target_M, tp_deg);
    }
  }
  if(printout > 0)
  {
    Print("\n//** Mprwalk: Random Perturbation Walk of degree (%d,%d):",op_deg,tp_deg);
    ivString(curr_weight, "//** Mprwalk: new current weight");
    ivString(target_weight, "//** Mprwalk: new target weight");
  }

#ifdef TIME_TEST
  to = clock();
#endif
  Gomega = MwalkInitialForm(G, curr_weight); // compute an initial form ideal of <G> w.r.t. "curr_vector"
#ifdef TIME_TEST
  tif = tif + clock()-to; //time for computing initial form ideal
#endif

  while(1)
  {
    nstep ++;
#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** Mprwalk: Gomega");
    }
#endif

    if(reduction == 0 && nstep > 1)
    {
      FF = middleOfCone(G,Gomega);
      if(FF != NULL)
      {
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        goto NEXT_VECTOR;
      }
    }

#ifdef ENDWALKS
    if(endwalks == TRUE)
    {
      if(printout > 0)
      {
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
        //idElements(G, "G");
        //headidString(G, "G");
      }
    }
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    if(target_M->length() == nV)
    {/*
      // define a new ring with ordering "(a(curr_weight),lp)
      if (rParameter(currRing) != NULL)
        DefRingPar(curr_weight);
      else
        rChangeCurrRing(VMrDefault(curr_weight));
*/
      rChangeCurrRing(VMrRefine(target_M,curr_weight));
    }
    else
    {
      rChangeCurrRing(VMatrRefine(target_M,curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);
#ifdef ENDWALKS
    if(endwalks == TRUE)
    {
      if(printout > 0)
      {
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));

        //idElements(Gomega1, "Gw");
        //headidString(Gomega1, "headGw");

        PrintS("\n// compute a rGB of Gw:\n");
      }
#ifndef  BUCHBERGER_ALG
      ivString(hilb_func, "w");
#endif
    }
#endif
#ifdef TIME_TEST
    tim = clock();
    to = clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(M,"//** Mprwalk: M");
    }
#endif
#ifdef TIME_TEST
    if(endwalks == TRUE)
    {
      xtstd = xtstd+clock()-to;
#ifdef ENDWALKS
      Print("\n// time for the last std(Gw)  = %.2f sec\n",
            ((double) clock())/1000000 -((double)tim) /1000000);
#endif
    }
    else
      tstd=tstd+clock()-to;
#endif
    /* change the ring to oldRing */
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a representation of the generators of submod (M)
       with respect to those of mod (Gomega).
       Gomega is a reduced Groebner basis w.r.t. the current ring */
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    if(endwalks == FALSE)
      tlift = tlift+clock()-to;
    else
      xtlift=clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F,"//** Mprwalk: F");
    }
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    if(reduction == 0)
    {
      G = idrMoveR(F,oldRing,currRing);
    }
    else
    {
      F1 = idrMoveR(F, oldRing,currRing);
      if(printout > 2)
      {
        PrintS("\n //** Mprwalk: reduce the Groebner basis.\n");
      }
#ifdef TIME_TEST
      to=clock();
#endif
      G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
      if(endwalks == FALSE)
        tred = tred+clock()-to;
      else
        xtred=clock()-to;
#endif
      idDelete(&F1);
    }

    if(endwalks == TRUE)
      break;

    NEXT_VECTOR:
#ifdef TIME_TEST
    to = clock();
#endif
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    tnw = tnw + clock() - to;
#endif

#ifdef TIME_TEST
    to = clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "next_vector"
    Gomega = MwalkInitialForm(G, next_weight);
#ifdef TIME_TEST
    tif = tif + clock()-to; //time for computing initial form ideal
#endif

    //lengthpoly(Gomega) = 1 if there is a polynomial in Gomega with at least 3 monomials and 0 otherwise
    if(lengthpoly(Gomega) > 0)
    {
      if(printout > 1)
      {
        PrintS("\n Mpwalk: there is a polynomial in Gomega with at least 3 monomials.\n");
      }
      // low-dimensional facet of the cone
      delete next_weight;
      if(target_M->length() == nV)
      {
        iv_M = MivMatrixOrder(curr_weight);
      }
      else
      {
        iv_M = MivMatrixOrderRefine(curr_weight,target_M);
      }
#ifdef TIME_TEST
      to = clock();
#endif
      next_weight = MWalkRandomNextWeight(G, iv_M, target_weight, weight_rad, op_deg);
#ifdef TIME_TEST
      tnw = tnw + clock() - to;
#endif
      idDelete(&Gomega);
#ifdef TIME_TEST
      to = clock();
#endif
      Gomega = MwalkInitialForm(G, next_weight);
#ifdef TIME_TEST
      tif = tif + clock()-to; //time for computing initial form ideal
#endif
      delete iv_M;
    }

#ifdef PRINT_VECTORS
    if(printout > 0)
    {
      MivString(curr_weight, target_weight, next_weight);
    }
#endif

    if(Overflow_Error == TRUE)
    {
      ntwC = 0;
      //Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
      //idElements(G, "G");
      delete next_weight;
      goto FINISH_160302;
    }
    if(MivComp(next_weight, ivNull) == 1){
      newRing = currRing;
      delete next_weight;
      //Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
      break;
    }
    if(MivComp(next_weight, target_weight) == 1)
      endwalks = TRUE;

    for(i=nV-1; i>=0; i--)
      (*curr_weight)[i] = (*next_weight)[i];

    delete next_weight;
  }// end of while-loop

  if(tp_deg != 1)
  {
    FINISH_160302:
    if(target_M->length() == nV)
    {
      if(MivSame(orig_target, exivlp) == 1)
        if (rParameter(currRing) != NULL)
          DefRingParlp();
        else
          VMrDefaultlp();
      else
        if (rParameter(currRing) != NULL)
          DefRingPar(orig_target);
        else
          rChangeCurrRing(VMrDefault(orig_target));
    }
    else
    {
      rChangeCurrRing(VMatrDefault(target_M));
    }
    TargetRing=currRing;
    F1 = idrMoveR(G, newRing,currRing);

    // check whether the pertubed target vector stays in the correct cone
    if(ntwC != 0)
    {
      ntestw = test_w_in_ConeCC(F1, pert_target_vector);
    }
    if(ntestw != 1 || ntwC == 0)
    {
      if(ntestw != 1 && printout > 2)
      {
#ifdef PRINT_VECTORS
        ivString(pert_target_vector, "tau");
#endif
        PrintS("\n// **Mprwalk: perturbed target vector doesn't stay in cone.");
        Print("\n// ring r%d = %s;\n", nstep, rString(currRing));
        //idElements(F1, "G");
      }
      // LastGB is "better" than the kStd subroutine
#ifdef TIME_TEST
      to=clock();
#endif
      ideal eF1;
      if(nP == 0 || tp_deg == 1 || MivSame(orig_target, exivlp) != 1 || target_M->length() != nV)
      {
        if(printout > 2)
        {
          PrintS("\n// ** Mprwalk: Call \"std\" to compute a Groebner basis.\n");
        }
        eF1 = MstdCC(F1);
        idDelete(&F1);
      }
      else
      {
        if(printout > 2)
        {
          PrintS("\n// **Mprwalk: Call \"LastGB\" to compute a Groebner basis.\n");
        }
        rChangeCurrRing(newRing);
        ideal F2 = idrMoveR(F1, TargetRing,currRing);
        eF1 = LastGB(F2, curr_weight, tp_deg-1);
        F2=NULL;
      }
#ifdef TIME_TEST
      xtextra=clock()-to;
#endif
      ring exTargetRing = currRing;

      rChangeCurrRing(XXRing);
      Eresult = idrMoveR(eF1, exTargetRing,currRing);
    }
    else
    {
      rChangeCurrRing(XXRing);
      Eresult = idrMoveR(F1, TargetRing,currRing);
    }
  }
  else
  {
    rChangeCurrRing(XXRing);
    Eresult = idrMoveR(G, newRing,currRing);
  }
  si_opt_1 = save1; //set original options, e. g. option(RedSB)
  delete ivNull;
  if(tp_deg != 1)
    delete target_weight;

  if(op_deg != 1 )
    delete curr_weight;

  delete exivlp;
  delete last_omega;

#ifdef TIME_TEST
  TimeStringFractal(tinput, tostd, tif+xtif, tstd+xtstd,0, tlift+xtlift, tred+xtred,
             tnw+xtnw);

  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// It took %d steps and Overflow_Error? (%d)\n", nstep,  Overflow_Error);
#endif

  if(printout > 0)
  {
    Print("\n//** Mprwalk: Perturbation Walk took %d steps.\n", nstep);
  }
  return(Eresult);
}

intvec* XivNull;

/*****************************
 * define a matrix (1 ... 1) *
 *****************************/
intvec* MMatrixone(int nV)
{
  int i,j;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
    for(j=0; j<nV; j++)
    (*ivM)[i*nV + j] = 1;

  return(ivM);
}

int nnflow;
int Xcall;
int Xngleich;

/***********************************************************************
 * Perturb the start weight vector at the top level, i.e. nlev = 1     *
 ***********************************************************************/
static ideal rec_fractal_call(ideal G, int nlev, intvec* ivtarget,
             int reduction, int printout)
{
  Overflow_Error =  FALSE;
  if(printout >0)
  {
    Print("\n\n// Entering the %d-th recursion:", nlev);
  }
  int i, nV = currRing->N;
  ring new_ring, testring;
  //ring extoRing;
  ideal Gomega, Gomega1, Gomega2, FF, F, Gresult, Gresult1, G1, Gt;
  int nwalks = 0;
  intvec* Mwlp;
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  //intvec* extXtau;
  intvec* next_vect;
  intvec* omega2 = new intvec(nV);
  intvec* omtmp = new intvec(nV);
  //intvec* altomega = new intvec(nV);

  for(i = nV -1; i>=0; i--)//Aenderung!!
  {
    (*omtmp)[i] = (*ivtarget)[i];
  }
  //BOOLEAN isnewtarget = FALSE;

  // to avoid (1,0,...,0) as the target vector (Hans)
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  intvec* omega = new intvec(nV);
  for(i=0; i<nV; i++) {
    if(Xsigma->length() == nV)
      (*omega)[i] =  (*Xsigma)[i];
    else
      (*omega)[i] = (*Xsigma)[(nV*(nlev-1))+i];

    (*omega2)[i] = (*Xtau)[(nlev-1)*nV+i];
  }

   if(nlev == 1)  Xcall = 1;
   else Xcall = 0;

  ring oRing = currRing;

  while(1)
  {
#ifdef FIRST_STEP_FRACTAL
    // perturb the current weight vector only on the top level or
    // after perturbation of the both vectors, nlev = 2 as the top level
    if((nlev == 1 && Xcall == 0) || (nlev == 2 && Xngleich == 1))
      if(islengthpoly2(G) == 1)
      {
        Mwlp = MivWeightOrderlp(omega);
        Xsigma = Mfpertvector(G, Mwlp);
        delete Mwlp;
        Overflow_Error = FALSE;
      }
#endif
    nwalks ++;
    NEXT_VECTOR_FRACTAL:
#ifdef TIME_TEST
    to=clock();
#endif
    // determine the next border
    next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
    oRing = currRing;

    // We only perturb the current target vector at the recursion level 1
    if(Xngleich == 0 && nlev == 1) //(ngleich == 0) important, e.g. ex2, ex3
      if (MivComp(next_vect, omega2) == 1)
      {
        // to dispense with taking initial (and lifting/interreducing
        // after the call of recursion
        if(printout > 0)
        {
          Print("\n//** rec_fractal_call: Perturb the both vectors with degree %d.",nlev);
          //idElements(G, "G");
        }

        Xngleich = 1;
        nlev +=1;

        if(ivtarget->length() == nV)
        {
/*
          if (rParameter(currRing) != NULL)
            DefRingPar(omtmp);
          else
            rChangeCurrRing(VMrDefault(omtmp));
*/
          rChangeCurrRing(VMrRefine(ivtarget,omtmp));
        }
        else
        {
          //rChangeCurrRing(VMatrDefault(ivtarget));
          rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
        }
        testring = currRing;
        Gt = idrMoveR(G, oRing,currRing);

        // perturb the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
          delete Xtau;
          Xtau = NewVectorlp(Gt);
        }
        else
        {
          delete Xtau;
          Xtau = Mfpertvector(Gt,ivtarget);
        }

        rChangeCurrRing(oRing);
        G = idrMoveR(Gt, testring,currRing);

        // perturb the current vector w.r.t. the current GB
        Mwlp = MivWeightOrderlp(omega);
        Xsigma = Mfpertvector(G, Mwlp);
        delete Mwlp;

        for(i=nV-1; i>=0; i--) {
          (*omega2)[i] = (*Xtau)[nV+i];
          (*omega)[i] = (*Xsigma)[nV+i];
        }

        delete next_vect;
#ifdef TIME_TEST
        to=clock();
#endif
        // to avoid the value of Overflow_Error that occur in Mfpertvector
        Overflow_Error = FALSE;
        next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
        xtnw=xtnw+clock()-to;
#endif
      }// end of (if MivComp(next_vect, omega2) == 1)

#ifdef PRINT_VECTORS
      if(printout > 0)
      {
        MivString(omega, omega2, next_vect);
      }
#endif

    // check whether the the computed vector is in the correct cone.
    // If no, compute the reduced Groebner basis of an omega-homogeneous
    // ideal with Buchberger's algorithm and stop this recursion step
    if(Overflow_Error == TRUE || test_w_in_ConeCC(G, next_vect) != 1)  //e.g. Example s7, cyc6
    {
      delete next_vect;
      if(ivtarget->length() == nV)
      {
/*
        if (rParameter(currRing) != NULL)
          DefRingPar(omtmp);
        else
          rChangeCurrRing(VMrDefault(omtmp));
*/
        rChangeCurrRing(VMrRefine(ivtarget,omtmp));
      }
      else
      {
        //rChangeCurrRing(VMatrDefault(ivtarget));
        rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
      }
#ifdef TEST_OVERFLOW
      Gt = idrMoveR(G, oRing,currRing);
      Gt = NULL; return(Gt);
#endif
      if(printout > 0)
      {
        Print("\n//** rec_fractal_call: Applying Buchberger's algorithm in ring r = %s;",
              rString(currRing));
      }
#ifdef TIME_TEST
      to=clock();
#endif
      Gt = idrMoveR(G, oRing,currRing);
      G1 = MstdCC(Gt);
#ifdef TIME_TEST
      xtextra=xtextra+clock()-to;
#endif
      Gt = NULL;

      delete omega2;
      //delete altomega;
      if(printout > 0)
      {
        Print("\n//** rec_fractal_call: Overflow. (4) Leaving the %d-th recursion with %d steps.\n",
              nlev, nwalks);
        //Print(" ** Overflow_Error? (%d)", Overflow_Error);
      }

      nnflow ++;
      Overflow_Error = FALSE;
      return (G1);
    }

    /* If the perturbed target vector stays in the correct cone,
       return the current GB,
       otherwise, return the computed  GB by the Buchberger-algorithm.
       Then we update the perturbed target vectors w.r.t. this GB. */

    /* the computed vector is equal to the origin vector, since
       t is not defined */

    if (MivComp(next_vect, XivNull) == 1)
    {
      if(ivtarget->length() == nV)
      {
/*
        if (rParameter(currRing) != NULL)
          DefRingPar(omtmp);
        else
          rChangeCurrRing(VMrDefault(omtmp));
*/
        rChangeCurrRing(VMrRefine(ivtarget,omtmp));
      }
      else
      {
        //rChangeCurrRing(VMatrDefault(ivtarget));
        rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
      }

      testring = currRing;
      Gt = idrMoveR(G, oRing,currRing);
      if(test_w_in_ConeCC(Gt, omega2) == 1)
      {
        delete omega2;
        delete next_vect;
        //delete altomega;
        if(printout > 0)
        {
          Print("\n//** rec_fractal_call: Correct cone. (5) Leaving the %d-th recursion with %d steps.\n",
              nlev, nwalks);
        }
        if(printout>2)
        {
          idString(Gt,"//** rec_fractal_call: Gt");
        }
        return (Gt);
      }
      else
      {
        if(printout > 0)
        {
          PrintS("\n//** rec_fractal_call: Wrong cone. Tau doesn't stay in the correct cone.\n");
        }

#ifndef  MSTDCC_FRACTAL
        intvec* Xtautmp;
        if(ivtarget->length() == nV)
        {
          Xtautmp = Mfpertvector(Gt, MivMatrixOrder(omtmp));
        }
        else
        {
          Xtautmp = Mfpertvector(Gt, ivtarget);
        }
#ifdef TEST_OVERFLOW
      if(Overflow_Error == TRUE)
      Gt = NULL; return(Gt);
#endif

        if(MivSame(Xtau, Xtautmp) == 1)
        {
          if(printout > 0)
          {
            PrintS("\n//** rec_fractal_call: Updated vectors are equal to the old vectors.\n");
          }
          delete Xtautmp;
          goto FRACTAL_MSTDCC;
        }

        Xtau = Xtautmp;
        Xtautmp = NULL;

        for(i=nV-1; i>=0; i--)
          (*omega2)[i] = (*Xtau)[(nlev-1)*nV+i];

        rChangeCurrRing(oRing);
        G = idrMoveR(Gt, testring,currRing);

        goto NEXT_VECTOR_FRACTAL;
#endif

      FRACTAL_MSTDCC:
        if(printout > 0)
        {
          Print("\n//** rec_fractal_call: Wrong cone. Applying Buchberger's algorithm in ring = %s.\n",
                rString(currRing));
        }
#ifdef TIME_TEST
        to=clock();
#endif
        G = MstdCC(Gt);
#ifdef TIME_TEST
        xtextra=xtextra+clock()-to;
#endif
        oRing = currRing;

        // update the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
/*
          if(MivSame(Xivinput, Xivlp) == 1)
            if (rParameter(currRing) != NULL)
              DefRingParlp();
            else
              VMrDefaultlp();
          else
            if (rParameter(currRing) != NULL)
              DefRingPar(Xivinput);
            else
              rChangeCurrRing(VMrDefault(Xivinput));
*/
          rChangeCurrRing(VMrRefine(ivtarget,Xivinput));
        }
        else
        {
          rChangeCurrRing(VMatrRefine(ivtarget,Xivinput));
        }
        testring = currRing;
        Gt = idrMoveR(G, oRing,currRing);

        // perturb the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
          delete Xtau;
          Xtau = NewVectorlp(Gt);
        }
        else
        {
          delete Xtau;
          Xtau = Mfpertvector(Gt,ivtarget);
        }

        rChangeCurrRing(oRing);
        G = idrMoveR(Gt, testring,currRing);

        delete omega2;
        delete next_vect;
        //delete altomega;
        if(printout > 0)
        {
          Print("\n//** rec_fractal_call: Vectors updated. (6) Leaving the %d-th recursion with %d steps.\n",
              nlev, nwalks);
          //Print(" ** Overflow_Error? (%d)", Overflow_Error);
        }
        if(Overflow_Error == TRUE)
          nnflow ++;

        Overflow_Error = FALSE;
        return(G);
      }
    }// end of (if next_vect==nullvector)

    for(i=nV-1; i>=0; i--) {
      //(*altomega)[i] = (*omega)[i];
      (*omega)[i] = (*next_vect)[i];
    }
    delete next_vect;
#ifdef TIME_TEST
    to=clock();
#endif
    // Take the initial form of <G> w.r.t. omega
    Gomega = MwalkInitialForm(G, omega);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** rec_fractal_call: Gomega");
    }
#endif
    if(reduction == 0)
    {
      // Check whether the intermediate weight vector lies in the interior of the cone.
      // If so, only perform reductions. Otherwise apply Buchberger's algorithm.
      FF = middleOfCone(G,Gomega);
      if( FF != NULL)
      {
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        // Compue next vector.
        goto NEXT_VECTOR_FRACTAL;
      }
    }

#ifndef  BUCHBERGER_ALG
    if(isNolVector(omega) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,omega,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif

    if(ivtarget->length() == nV)
    {
/*
      if (rParameter(currRing) != NULL)
        DefRingPar(omega);
      else
        rChangeCurrRing(VMrDefault(omega));
*/
      rChangeCurrRing(VMrRefine(ivtarget,omega));
    }
    else
    {
      rChangeCurrRing(VMatrRefine(ivtarget,omega));
    }
    Gomega1 = idrMoveR(Gomega, oRing,currRing);

    // Maximal recursion depth, to compute a red. GB
    // Fractal walk with the alternative recursion
    // alternative recursion
    if(nlev == Xnlev || lengthpoly(Gomega1) == 0)
    {
      if(printout > 1)
      {
        PrintS("\n//** rec_fractal_call: Maximal recursion depth.\n");
      }
#ifdef TIME_TEST
      to=clock();
#endif
#ifdef  BUCHBERGER_ALG
      Gresult = MstdhomCC(Gomega1);
#else
      Gresult =kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,omega);
      delete hilb_func;
#endif
#ifdef TIME_TEST
      xtstd=xtstd+clock()-to;
#endif
    }
    else
    {
      rChangeCurrRing(oRing);
      Gomega1 = idrMoveR(Gomega1, oRing,currRing);
      Gresult = rec_fractal_call(idCopy(Gomega1),nlev+1,omega,reduction,printout);
    }
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(Gresult,"//** rec_fractal_call: M");
    }
#endif
    //convert a Groebner basis from a ring to another ring
    new_ring = currRing;

    rChangeCurrRing(oRing);
    Gresult1 = idrMoveR(Gresult, new_ring,currRing);
    Gomega2 = idrMoveR(Gomega1, new_ring,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    // Lifting process
    F = MLifttwoIdeal(Gomega2, Gresult1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F,"//** rec_fractal_call: F");
    }
#endif
    id_Normalize(F,currRing);
    idDelete(&Gresult1);
    idDelete(&Gomega2);
    idDelete(&G);

    rChangeCurrRing(new_ring);
    G = idrMoveR(F,oRing,currRing);
/*
    ideal F1 = idrMoveR(F, oRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    // Interreduce G
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);
*/
  }
}

/************************************************************************
 * Perturb the start weight vector at the top level with random element *
 ************************************************************************/
static ideal rec_r_fractal_call(ideal G, int nlev, intvec* ivtarget,
                int weight_rad, int reduction, int printout)
{
  Overflow_Error =  FALSE;
  //Print("\n\n// Entering the %d-th recursion:", nlev);

  int nwalks = 0,i,nV=currRing->N;//polylength
  ring new_ring, testring;
  //ring extoRing;
  ideal Gomega, Gomega1, Gomega2, F, FF, Gresult, Gresult1, G1, Gt;
#ifdef TIME_TEST
  ideal F1;
#endif
  intvec* Mwlp;
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
//  intvec* extXtau;
  intvec* next_vect;
  intvec* iv_M;
  intvec* omega2 = new intvec(nV);
  intvec* omtmp = new intvec(nV);
  intvec* altomega = new intvec(nV);

  //BOOLEAN isnewtarget = FALSE;

  for(i = nV -1; i>=0; i--)
  {
    (*omtmp)[i] = (*ivtarget)[i];
  }
  // to avoid (1,0,...,0) as the target vector (Hans)
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  intvec* omega = new intvec(nV);
  for(i=0; i<nV; i++) {
    if(Xsigma->length() == nV)
      (*omega)[i] =  (*Xsigma)[i];
    else
      (*omega)[i] = (*Xsigma)[(nV*(nlev-1))+i];

    (*omega2)[i] = (*Xtau)[(nlev-1)*nV+i];
  }

   if(nlev == 1)  Xcall = 1;
   else Xcall = 0;

  ring oRing = currRing;

  while(1)
  {
#ifdef FIRST_STEP_FRACTAL
    /*
    perturb the current weight vector only on the top level or
    after perturbation of the both vectors, nlev = 2 as the top level
    */
    if((nlev == 1 && Xcall == 0) || (nlev == 2 && Xngleich == 1))
      if(islengthpoly2(G) == 1)
      {
        Mwlp = MivWeightOrderlp(omega);
        Xsigma = Mfpertvector(G, Mwlp);
        delete Mwlp;
        Overflow_Error = FALSE;
      }
#endif
    nwalks ++;
    NEXT_VECTOR_FRACTAL:
#ifdef TIME_TEST
    to=clock();
#endif
    /* determine the next border */
    next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
    if(lengthpoly(MwalkInitialForm(G, next_vect)) > 0 && G->m[0] != NULL)
    {
      if(printout > 0)
      {
        PrintS("\n**// rec_r_fractal_call: there is a polynomial in Gomega with at least 3 monomials.\n");
      }
      delete next_vect;
      iv_M = MivMatrixOrder(omega);
#ifdef TIME_TEST
      to=clock();
#endif
      next_vect = MWalkRandomNextWeight(G,iv_M,omega2,weight_rad,nlev);
#ifdef TIME_TEST
      xtnw=xtnw+clock()-to;
#endif
      if(isNegNolVector(next_vect) == 1)
      {
        delete next_vect;
#ifdef TIME_TEST
        to=clock();
#endif
        next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
        xtnw=xtnw+clock()-to;
#endif
      }
    }
    oRing = currRing;

    // We only perturb the current target vector at the recursion  level 1
    if(Xngleich == 0 && nlev == 1) //(ngleich == 0) important, e.g. ex2, ex3
      if (MivComp(next_vect, omega2) == 1)
      {
        // to dispense with taking initials and lifting/interreducing
        // after the call of recursion.
        if(printout > 0)
        {
          Print("\n//** rec_r_fractal_call: Perturb both vectors with degree %d.",nlev);
          //idElements(G, "G");
        }
        Xngleich = 1;
        nlev +=1;
        if(ivtarget->length() == nV)
        {
/*
          if (rParameter(currRing) != NULL)
            DefRingPar(omtmp);
          else
            rChangeCurrRing(VMrDefault(omtmp));
*/
          rChangeCurrRing(VMrRefine(ivtarget,omtmp));
        }
        else
        {
          //rChangeCurrRing(VMatrDefault(ivtarget));
          rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
        }
        testring = currRing;
        Gt = idrMoveR(G, oRing,currRing);

        // perturb the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
          delete Xtau;
          Xtau = NewVectorlp(Gt);
        }
        else
        {
          delete Xtau;
          Xtau = Mfpertvector(Gt,ivtarget);
        }

        rChangeCurrRing(oRing);
        G = idrMoveR(Gt,testring,currRing);

        // perturb the current vector w.r.t. the current GB
        Mwlp = MivWeightOrderlp(omega);
        if(ivtarget->length() > nV)
        {
          delete Mwlp;
          Mwlp = MivMatrixOrderRefine(omega,ivtarget);
        }
        Xsigma = Mfpertvector(G, Mwlp);
        delete Mwlp;

        for(i=nV-1; i>=0; i--)
        {
          (*omega2)[i] = (*Xtau)[nV+i];
          (*omega)[i] = (*Xsigma)[nV+i];
        }

        delete next_vect;

   //to avoid the value of Overflow_Error that occur in Mfpertvector
        Overflow_Error = FALSE;
#ifdef TIME_TEST
        to=clock();
#endif
        next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
        xtnw=xtnw+clock()-to;
#endif
        if(lengthpoly(MwalkInitialForm(G, next_vect)) > 0 && G->m[0] != NULL)
        {
          // there is a polynomial in Gomega with at least 3 monomials
          iv_M = MivMatrixOrder(omega);
          delete next_vect;
#ifdef TIME_TEST
          to=clock();
#endif
          next_vect = MWalkRandomNextWeight(G,iv_M,omega2,weight_rad,nlev);
#ifdef TIME_TEST
          xtnw=xtnw+clock()-to;
#endif
          delete iv_M;
          if(isNegNolVector(next_vect) == 1)
          {
            delete next_vect;
#ifdef TIME_TEST
            to=clock();
#endif
            next_vect = MkInterRedNextWeight(omega,omega2,G);
#ifdef TIME_TEST
        xtnw=xtnw+clock()-to;
#endif
          }
        }
      }
#ifdef PRINT_VECTORS
      if(printout > 0)
      {
        MivString(omega, omega2, next_vect);
      }
#endif

/*     check whether the the computed vector is in the correct cone
       If no, the reduced GB of an omega-homogeneous ideal will be
       computed by Buchberger algorithm and stop this recursion step
*/
    if(Overflow_Error == TRUE || test_w_in_ConeCC(G,next_vect) != 1)//e.g. Example s7, cyc6
    {
      delete next_vect;
      if(ivtarget->length() == nV)
      {
/*
        if (rParameter(currRing) != NULL)
        {
          DefRingPar(omtmp);
        }
        else
        {
          rChangeCurrRing(VMrDefault(omtmp));
        }
*/
        rChangeCurrRing(VMrRefine(ivtarget,omtmp));
      }
      else
      {
        //rChangeCurrRing(VMatrDefault(ivtarget));
        rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
      }
#ifdef TEST_OVERFLOW
      Gt = idrMoveR(G, oRing,currRing);
      Gt = NULL;
      return(Gt);
#endif
      if(printout > 0)
      {
        Print("\n//** rec_r_fractal_call: applying Buchberger's algorithm in ring r = %s;",
              rString(currRing));
      }
      Gt = idrMoveR(G, oRing,currRing);
#ifdef TIME_TEST
      to=clock();
#endif
      G1 = MstdCC(Gt);
#ifdef TIME_TEST
      xtextra=xtextra+clock()-to;
#endif
      Gt = NULL;

      delete omega2;
      delete altomega;
      if(printout > 0)
      {
        Print("\n//** rec_r_fractal_call: (1) Leaving the %d-th recursion with %d steps.\n",
              nlev, nwalks);
        //Print(" ** Overflow_Error? (%d)", Overflow_Error);
      }
      nnflow ++;
      Overflow_Error = FALSE;
      return (G1);
    }
    /*
       If the perturbed target vector stays in the correct cone,
       return the current Groebner basis.
       Otherwise, return the Groebner basis computed with Buchberger's
       algorithm.
       Then we update the perturbed target vectors w.r.t. this GB.
    */
    if (MivComp(next_vect, XivNull) == 1)
    {
      // The computed vector is equal to the origin vector,
      // because t is not defined
      if(ivtarget->length() == nV)
      {
/*
        if (rParameter(currRing) != NULL)
          DefRingPar(omtmp);
        else
          rChangeCurrRing(VMrDefault(omtmp));
*/
        rChangeCurrRing(VMrRefine(ivtarget,omtmp));
      }
      else
      {
        //rChangeCurrRing(VMatrDefault(ivtarget));
        rChangeCurrRing(VMatrRefine(ivtarget,omtmp));
      }
      testring = currRing;
      Gt = idrMoveR(G, oRing,currRing);

      if(test_w_in_ConeCC(Gt, omega2) == 1)
      {
        delete omega2;
        delete next_vect;
        delete altomega;
        if(printout > 0)
        {
          Print("\n//** rec_r_fractal_call: (2) Leaving the %d-th recursion with %d steps.\n",
                nlev, nwalks);
          //Print(" ** Overflow_Error? (%d)", Overflow_Error);
        }
        return (Gt);
      }
      else
      {
        if(printout > 0)
        {
          Print("\n//** rec_r_fractal_call: target weight doesn't stay in the correct cone.\n");
        }

#ifndef  MSTDCC_FRACTAL
#ifdef PRINT_VECTORS
        if(printout > 0)
        {
          ivString(Xtau, "old Xtau");
        }
#endif
        intvec* Xtautmp;
        if(ivtarget->length() == nV)
        {
          Xtautmp = Mfpertvector(Gt, MivMatrixOrder(omtmp));
        }
        else
        {
          Xtautmp = Mfpertvector(Gt, ivtarget);
        }
#ifdef TEST_OVERFLOW
      if(Overflow_Error == TRUE)
      Gt = NULL; return(Gt);
#endif

        if(MivSame(Xtau, Xtautmp) == 1)
        {
          //PrintS("\n// Update vectors are equal to the old vectors!!");
          delete Xtautmp;
          goto FRACTAL_MSTDCC;
        }

        Xtau = Xtautmp;
        Xtautmp = NULL;
#ifdef PRINT_VECTORS
        if(printout > 0)
        {
          ivString(Xtau, "new  Xtau");
        }
#endif

        for(i=nV-1; i>=0; i--)
          (*omega2)[i] = (*Xtau)[(nlev-1)*nV+i];

        //Print("\n//  ring tau = %s;", rString(currRing));
        rChangeCurrRing(oRing);
        G = idrMoveR(Gt, testring,currRing);

        goto NEXT_VECTOR_FRACTAL;
#endif

      FRACTAL_MSTDCC:
        if(printout > 0)
        {
          Print("\n//** rec_r_fractal_call: apply Buchberger's algorithm in ring = %s.\n",
                rString(currRing));
        }
#ifdef TIME_TEST
        to=clock();
#endif
        G = MstdCC(Gt);
#ifdef TIME_TEST
        xtextra=xtextra+clock()-to;
#endif
        oRing = currRing;

        // update the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
/*
          if(MivSame(Xivinput, Xivlp) == 1)
            if (rParameter(currRing) != NULL)
              DefRingParlp();
            else
              VMrDefaultlp();
          else
            if (rParameter(currRing) != NULL)
              DefRingPar(Xivinput);
            else
              rChangeCurrRing(VMrDefault(Xivinput));
*/
          rChangeCurrRing(VMrRefine(ivtarget,Xivinput));
        }
        else
        {
          rChangeCurrRing(VMatrRefine(ivtarget,Xivinput));
        }
        testring = currRing;
        Gt = idrMoveR(G, oRing,currRing);

        // perturb the original target vector w.r.t. the current GB
        if(ivtarget->length() == nV)
        {
          delete Xtau;
          Xtau = NewVectorlp(Gt);
        }
        else
        {
          delete Xtau;
          Xtau = Mfpertvector(Gt,ivtarget);
        }

        rChangeCurrRing(oRing);
        G = idrMoveR(Gt, testring,currRing);

        delete omega2;
        delete next_vect;
        delete altomega;
        if(printout > 0)
        {
          Print("\n//** rec_r_fractal_call: (3) Leaving the %d-th recursion with %d steps.\n",
                nlev,nwalks);
          //Print(" ** Overflow_Error? (%d)", Overflow_Error);
        }
        if(Overflow_Error == TRUE)
          nnflow ++;

        Overflow_Error = FALSE;
        return(G);
      }
    } //end of if(MivComp(next_vect, XivNull) == 1)

    for(i=nV-1; i>=0; i--)
    {
      (*altomega)[i] = (*omega)[i];
      (*omega)[i] = (*next_vect)[i];
    }
    delete next_vect;
#ifdef TIME_TEST
    to=clock();
#endif
    // Take the initial form of <G> w.r.t. omega
    Gomega = MwalkInitialForm(G, omega);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif
    //polylength = 1 if there is a polynomial in Gomega with at least 3 monomials and 0 otherwise
    //polylength = lengthpoly(Gomega);
#ifdef CHECK_IDEAL_MWALK
    if(printout > 1)
    {
      idString(Gomega,"//** rec_r_fractal_call: Gomega");
    }
#endif
    if(reduction == 0)
    {
      /* Check whether the intermediate weight vector lies in the interior of the cone.
       * If so, only perform reductions. Otherwise apply Buchberger's algorithm. */
      FF = middleOfCone(G,Gomega);
      if( FF != NULL)
      {
        idDelete(&G);
        G = idCopy(FF);
        idDelete(&FF);
        /* Compue next vector. */
        goto NEXT_VECTOR_FRACTAL;
      }
    }

#ifndef  BUCHBERGER_ALG
    if(isNolVector(omega) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,omega,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif
    if(ivtarget->length() == nV)
    {
/*
      if (rParameter(currRing) != NULL)
        DefRingPar(omega);
      else
        rChangeCurrRing(VMrDefault(omega));
*/
      rChangeCurrRing(VMrRefine(ivtarget,omega));
    }
    else
    {
      rChangeCurrRing(VMatrRefine(ivtarget,omega));
    }
    Gomega1 = idrMoveR(Gomega, oRing,currRing);

    // Maximal recursion depth, to compute a red. GB
    // Fractal walk with the alternative recursion
    // alternative recursion
    if(nlev == Xnlev || lengthpoly(Gomega1) == 0)
    {
#ifdef TIME_TEST
      to=clock();
#endif
#ifdef  BUCHBERGER_ALG
      Gresult = MstdhomCC(Gomega1);
#else
      Gresult =kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,omega);
      delete hilb_func;
#endif
#ifdef TIME_TEST
      xtstd=xtstd+clock()-to;
#endif
    }
    else
    {
      rChangeCurrRing(oRing);
      Gomega1 = idrMoveR(Gomega1, oRing,currRing);
      Gresult = rec_r_fractal_call(idCopy(Gomega1),nlev+1,omega,weight_rad,reduction,printout);
    }
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(Gresult,"//** rec_r_fractal_call: M");
    }
#endif
    //convert a Groebner basis from a ring to another ring
    new_ring = currRing;

    rChangeCurrRing(oRing);
    Gresult1 = idrMoveR(Gresult, new_ring,currRing);
    Gomega2 = idrMoveR(Gomega1, new_ring,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    // Lifting process
    F = MLifttwoIdeal(Gomega2, Gresult1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif
#ifdef CHECK_IDEAL_MWALK
    if(printout > 2)
    {
      idString(F,"//** rec_r_fractal_call: F");
    }
#endif
    id_Normalize(F,currRing);
    idDelete(&Gresult1);
    idDelete(&Gomega2);
    idDelete(&G);

    rChangeCurrRing(new_ring);
    //F1 = idrMoveR(F, oRing,currRing);
    G = idrMoveR(F,oRing,currRing);
/*
#ifdef TIME_TEST
    to=clock();
#endif
    // Interreduce G
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);
*/
  }
}


/*******************************************************************************
 * The implementation of the fractal walk algorithm                            *
 *                                                                             *
 * The main procedure Mfwalk calls the recursive Subroutine                    *
 * rec_fractal_call to compute the wanted Groebner basis.                      *
 * At the main procedur we compute the reduced Groebner basis w.r.t. a "fast"  *
 * order, e.g. "dp" and a sequence of weight vectors which are row vectors     *
 * of a matrix. This matrix defines the given monomial order, e.g. "lp"        *
 *******************************************************************************/
ideal Mfwalk(ideal G, intvec* ivstart, intvec* ivtarget,
             int reduction, int printout)
{
  BITSET save1 = si_opt_1; // save current options
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    //si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// ring ro = %s;", rString(currRing));

  nnflow = 0;
  Xngleich = 0;
  Xcall = 0;
#ifdef TIME_TEST
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0; xtextra=0;
  xftinput = clock();
#endif
  ring  oldRing = currRing;
  int i, nV = currRing->N;
  XivNull = new intvec(nV);
  Xivinput = ivtarget;
  ngleich = 0;
#ifdef TIME_TEST
  to=clock();
#endif
  ideal I = MstdCC(G);
  G = NULL;
#ifdef TIME_TEST
  xftostd=clock()-to;
#endif
  Xsigma = ivstart;

  Xnlev=nV;

#ifdef FIRST_STEP_FRACTAL
  ideal Gw = MwalkInitialForm(I, ivstart);
  for(i=IDELEMS(Gw)-1; i>=0; i--)
  {
    if((Gw->m[i]!=NULL) // len >=0
    && (Gw->m[i]->next!=NULL) // len >=1
    && (Gw->m[i]->next->next!=NULL)) // len >=2
    {
      intvec* iv_dp = MivUnit(nV); // define (1,1,...,1)
      intvec* Mdp;
      if(ivstart->length() == nV)
      {
        if(MivSame(ivstart, iv_dp) != 1)
          Mdp = MivWeightOrderdp(ivstart);
        else
          Mdp = MivMatrixOrderdp(nV);
      }
      else
      {
        Mdp = ivstart;
      }

      Xsigma = Mfpertvector(I, Mdp);
      Overflow_Error = FALSE;

      delete Mdp;
      delete iv_dp;
      break;
    }
  }
  idDelete(&Gw);
#endif

  ideal I1;
  intvec* Mlp;
  Xivlp = Mivlp(nV);

  if(ivtarget->length() == nV)
  {
    if(MivComp(ivtarget, Xivlp)  != 1)
    {
      if (rParameter(currRing) != NULL)
        DefRingPar(ivtarget);
      else
        rChangeCurrRing(VMrDefault(ivtarget));

      I1 = idrMoveR(I, oldRing,currRing);
      Mlp = MivWeightOrderlp(ivtarget);
      Xtau = Mfpertvector(I1, Mlp);
    }
    else
    {
      if (rParameter(currRing) != NULL)
        DefRingParlp();
      else
        VMrDefaultlp();

      I1 = idrMoveR(I, oldRing,currRing);
      Mlp =  MivMatrixOrderlp(nV);
      Xtau = Mfpertvector(I1, Mlp);
    }
  }
  else
  {
    rChangeCurrRing(VMatrDefault(ivtarget));
    I1 = idrMoveR(I,oldRing,currRing);
    Mlp =  ivtarget;
    Xtau = Mfpertvector(I1, Mlp);
  }
  delete Mlp;
  Overflow_Error = FALSE;

  //ivString(Xsigma, "Xsigma");
  //ivString(Xtau, "Xtau");

  id_Delete(&I, oldRing);
  ring tRing = currRing;
  if(ivtarget->length() == nV)
  {
/*
    if (rParameter(currRing) != NULL)
      DefRingPar(ivstart);
    else
      rChangeCurrRing(VMrDefault(ivstart));
*/
    rChangeCurrRing(VMrRefine(ivtarget,ivstart));
  }
  else
  {
    //rChangeCurrRing(VMatrDefault(ivstart));
    rChangeCurrRing(VMatrRefine(ivtarget,ivstart));
  }

  I = idrMoveR(I1,tRing,currRing);
#ifdef TIME_TEST
  to=clock();
#endif
  ideal J = MstdCC(I);
  idDelete(&I);
#ifdef TIME_TEST
  xftostd=xftostd+clock()-to;
#endif
  ideal resF;
  ring helpRing = currRing;

  J = rec_fractal_call(J,1,ivtarget,reduction,printout);
  //idString(J,"//** Mfwalk: J");
  rChangeCurrRing(oldRing);
  //Print("\n//Mfwalk: (2)\n");
  resF = idrMoveR(J, helpRing,currRing);
  //Print("\n//Mfwalk: (3)\n");
  idSkipZeroes(resF);
  //Print("\n//Mfwalk: (4)\n");

  si_opt_1 = save1; //set original options, e. g. option(RedSB)
  delete Xivlp;
  //delete Xsigma;
  delete Xtau;
  delete XivNull;
  //Print("\n//Mfwalk: (5)\n");
#ifdef TIME_TEST
  TimeStringFractal(xftinput, xftostd, xtif, xtstd, xtextra,
                    xtlift, xtred, xtnw);


  //Print("\n// pSetm_Error = (%d)", ErrorCheck());
  Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
  Print("\n// the numbers of Overflow_Error (%d)", nnflow);
#endif
  //Print("\n//Mfwalk: (6)\n");
  //idString(resF,"//** Mfwalk: resF");
  return(idCopy(resF));
}

/*******************************************************************************
 * The implementation of the fractal walk algorithm with random element        *
 *                                                                             *
 * The main procedur Mfwalk calls the recursive Subroutine                     *
 * rec_r_fractal_call to compute the wanted Groebner basis.                    *
 * At the main procedure we compute the reduced Groebner basis w.r.t. a "fast" *
 * order, e.g. "dp" and a sequence of weight vectors which are row vectors     *
 * of a matrix. This matrix defines the given monomial order, e.g. "lp"        *
 *******************************************************************************/
ideal Mfrwalk(ideal G, intvec* ivstart, intvec* ivtarget,
              int weight_rad, int reduction, int printout)
{
  BITSET save1 = si_opt_1; // save current options
  //check that weight radius is valid
  if(weight_rad < 0)
  {
    WerrorS("Invalid radius.\n");
    return NULL;
  }
  if(reduction == 0)
  {
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); // no reduced Groebner basis
    si_opt_1 &= (~Sy_bit(OPT_REDTAIL)); // not tail reductions
  }
  Set_Error(FALSE);
  Overflow_Error = FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// ring ro = %s;", rString(currRing));

  nnflow = 0;
  Xngleich = 0;
  Xcall = 0;
#ifdef TIME_TEST
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0; xtextra=0;
  xftinput = clock();
#endif
  ring  oldRing = currRing;
  int i, nV = currRing->N;
  XivNull = new intvec(nV);
  Xivinput = ivtarget;
  ngleich = 0;
#ifdef TIME_TEST
  to=clock();
#endif
  ideal I = MstdCC(G);
  G = NULL;
#ifdef TIME_TEST
  xftostd=clock()-to;
#endif
  Xsigma = ivstart;

  Xnlev=nV;

#ifdef FIRST_STEP_FRACTAL
  ideal Gw = MwalkInitialForm(I, ivstart);
  for(i=IDELEMS(Gw)-1; i>=0; i--)
  {
    if((Gw->m[i]!=NULL) // len >=0
    && (Gw->m[i]->next!=NULL) // len >=1
    && (Gw->m[i]->next->next!=NULL)) // len >=2
    {
      intvec* iv_dp = MivUnit(nV); // define (1,1,...,1)
      intvec* Mdp;
      if(ivstart->length() == nV)
      {
        if(MivSame(ivstart, iv_dp) != 1)
          Mdp = MivWeightOrderdp(ivstart);
        else
          Mdp = MivMatrixOrderdp(nV);
      }
      else
      {
        Mdp = ivstart;
      }

      Xsigma = Mfpertvector(I, Mdp);
      Overflow_Error = FALSE;

      delete Mdp;
      delete iv_dp;
      break;
    }
  }
  idDelete(&Gw);
#endif

  ideal I1;
  intvec* Mlp;
  Xivlp = Mivlp(nV);

  if(ivtarget->length() == nV)
  {
    if(MivComp(ivtarget, Xivlp)  != 1)
    {
      if (rParameter(currRing) != NULL)
        DefRingPar(ivtarget);
      else
        rChangeCurrRing(VMrDefault(ivtarget));

      I1 = idrMoveR(I, oldRing,currRing);
      Mlp = MivWeightOrderlp(ivtarget);
      Xtau = Mfpertvector(I1, Mlp);
    }
    else
    {
      if (rParameter(currRing) != NULL)
        DefRingParlp();
      else
        VMrDefaultlp();

      I1 = idrMoveR(I, oldRing,currRing);
      Mlp =  MivMatrixOrderlp(nV);
      Xtau = Mfpertvector(I1, Mlp);
    }
  }
  else
  {
    rChangeCurrRing(VMatrDefault(ivtarget));
    I1 = idrMoveR(I,oldRing,currRing);
    Mlp =  ivtarget;
    Xtau = Mfpertvector(I1, Mlp);
  }
  delete Mlp;
  Overflow_Error = FALSE;

  //ivString(Xsigma, "Xsigma");
  //ivString(Xtau, "Xtau");

  id_Delete(&I, oldRing);
  ring tRing = currRing;
  if(ivtarget->length() == nV)
  {
/*
    if (rParameter(currRing) != NULL)
      DefRingPar(ivstart);
    else
      rChangeCurrRing(VMrDefault(ivstart));
*/
    rChangeCurrRing(VMrRefine(ivtarget,ivstart));
  }
  else
  {
    //rChangeCurrRing(VMatrDefault(ivstart));
    rChangeCurrRing(VMatrRefine(ivtarget,ivstart));
  }

  I = idrMoveR(I1,tRing,currRing);
#ifdef TIME_TEST
  to=clock();
#endif
  ideal J = MstdCC(I);
  idDelete(&I);
#ifdef TIME_TEST
  xftostd=xftostd+clock()-to;
#endif
  ideal resF;
  ring helpRing = currRing;

  J = rec_r_fractal_call(J,1,ivtarget,weight_rad,reduction,printout);
  //idString(J,"//*** Mfrwalk: J");
  //Print("\n//** Mfrwalk hier (1)\n");
  rChangeCurrRing(oldRing);
  //Print("\n//** Mfrwalk hier (2)\n");
  resF = idrMoveR(J, helpRing,currRing);
  //Print("\n//** Mfrwalk hier (3)\n");
  //idSkipZeroes(resF);
  //Print("\n//** Mfrwalk hier (4)\n");
  si_opt_1 = save1; //set original options, e. g. option(RedSB)
  delete Xivlp;
  //delete Xsigma;
  delete Xtau;
  delete XivNull;
  //Print("\n//** Mfrwalk hier (5)\n");
#ifdef TIME_TEST
  TimeStringFractal(xftinput, xftostd, xtif, xtstd, xtextra,
                    xtlift, xtred, xtnw);


 // Print("\n// pSetm_Error = (%d)", ErrorCheck());
  Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
  Print("\n// the numbers of Overflow_Error (%d)", nnflow);
#endif
  //Print("\n//** Mfrwalk hier (6)\n");
  //idString(resF,"resF");
  //Print("\n//** Mfrwalk hier (7)\n");
  return(resF);
}

/*******************************************************
 * Tran's algorithm                                    *
 *                                                     *
 * use kStd, if nP = 0, else call Ab_Rec_Pert (LastGB) *
 *******************************************************/
ideal TranMImprovwalk(ideal G,intvec* curr_weight,intvec* target_tmp, int nP)
{
#ifdef TIME_TEST
  clock_t mtim = clock();
#endif
  Set_Error(FALSE  );
  Overflow_Error =  FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// ring ro = %s;", rString(currRing));

#ifdef TIME_TEST
  clock_t tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0, textra=0;
  clock_t tinput = clock();
#endif
  int nsteppert=0, i, nV = currRing->N, nwalk=0, npert_tmp=0;
  int *npert=(int*)omAlloc(2*nV*sizeof(int));
  ideal Gomega, M,F,  G1, Gomega1, Gomega2, M1, F1;
  //ring endRing;
  ring newRing, oldRing, lpRing;
  intvec* next_weight;
  intvec* ivNull = new intvec(nV); //define (0,...,0)
  intvec* iv_dp = MivUnit(nV);// define (1,1,...,1)
  intvec* iv_lp = Mivlp(nV); //define (1,0,...,0)
  ideal H0;
  //ideal  H1;
  ideal H2, Glp;
  int nGB, endwalks = 0,  nwalkpert=0;
  intvec* Mlp =  MivMatrixOrderlp(nV);
  intvec* vector_tmp = new intvec(nV);
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
    (*last_omega)[i] = 1;
  (*last_omega)[0] = 10000;

  //  intvec* extra_curr_weight = new intvec(nV);
  intvec* target_weight = new intvec(nV);
  for(i=nV-1; i>=0; i--)
    (*target_weight)[i] = (*target_tmp)[i];

  ring XXRing = currRing;
  newRing = currRing;

#ifdef TIME_TEST
  to=clock();
#endif
  // compute a red. GB w.r.t. the help ring
  if(MivComp(curr_weight, iv_dp) == 1) //rOrdStr(currRing) = "dp"
    G = MstdCC(G);
  else
  {
    //rOrdStr(currRing) = (a(.c_w..),lp,C)
    if (rParameter(currRing) != NULL)
      DefRingPar(curr_weight);
    else
      rChangeCurrRing(VMrDefault(curr_weight));
    G = idrMoveR(G, XXRing,currRing);
    G = MstdCC(G);
  }
#ifdef TIME_TEST
  tostd=clock()-to;
#endif

#ifdef REPRESENTATION_OF_SIGMA
  ideal Gw = MwalkInitialForm(G, curr_weight);

  if(islengthpoly2(Gw)==1)
  {
    intvec* MDp;
    if(MivComp(curr_weight, iv_dp) == 1)
      MDp = MatrixOrderdp(nV); //MivWeightOrderlp(iv_dp);
    else
      MDp = MivWeightOrderlp(curr_weight);

    curr_weight = RepresentationMatrix_Dp(G, MDp);

    delete MDp;

    ring exring = currRing;

    if (rParameter(currRing) != NULL)
      DefRingPar(curr_weight);
    else
      rChangeCurrRing(VMrDefault(curr_weight));
#ifdef TIME_TEST
    to=clock();
#endif
    Gw = idrMoveR(G, exring,currRing);
    G = MstdCC(Gw);
    Gw = NULL;
#ifdef TIME_TEST
    tostd=tostd+clock()-to;
#endif
    //ivString(curr_weight,"rep. sigma");
    goto COMPUTE_NEW_VECTOR;
  }

  idDelete(&Gw);
  delete iv_dp;
#endif


  while(1)
  {
#ifdef TIME_TEST
    to=clock();
#endif
    /* compute an initial form ideal of <G> w.r.t. "curr_vector" */
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    tif=tif+clock()-to;
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    /* define a new ring that its ordering is "(a(curr_weight),lp) */
    if (rParameter(currRing) != NULL)
      DefRingPar(curr_weight);
    else
      rChangeCurrRing(VMrDefault(curr_weight));

    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a reduced Groebner basis of <Gomega> w.r.t. "newRing" */
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif // BUCHBERGER_ALG
#ifdef TIME_TEST
    tstd=tstd+clock()-to;
#endif

    /* change the ring to oldRing */
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* compute a representation of the generators of submod (M)
       with respect to those of mod (Gomega).
       Gomega is a reduced Groebner basis w.r.t. the current ring */
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    tlift=tlift+clock()-to;
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    /* change the ring to newRing */
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    /* reduce the Groebner basis <G> w.r.t. new ring */
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    tred=tred+clock()-to;
#endif
    idDelete(&F1);


  COMPUTE_NEW_VECTOR:
    newRing = currRing;
    nwalk++;
    nwalkpert++;
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a next weight vector
    next_weight = MwalkNextWeightCC(curr_weight,target_weight, G);
#ifdef TIME_TEST
    tnw=tnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    /* check whether the computed intermediate weight vector is in
       the correct cone; sometimes it is very big e.g. s7, cyc7.
       If it is NOT in the correct cone, then compute directly
       a reduced Groebner basis with respect to the lexicographic ordering
       for the known Groebner basis that it is computed in the last step.
    */
    //if(test_w_in_ConeCC(G, next_weight) != 1)
    if(Overflow_Error == TRUE)
    {
    OMEGA_OVERFLOW_TRAN_NEW:
      //Print("\n//  takes %d steps!", nwalk-1);
      //Print("\n//ring lastRing = %s;", rString(currRing));
#ifdef TEST_OVERFLOW
      goto  BE_FINISH;
#endif
/*
#ifdef CHECK_IDEAL_MWALK
      idElements(G, "G");
      //headidString(G, "G");
#endif
*/
      if(MivSame(target_tmp, iv_lp) == 1)
        if (rParameter(currRing) != NULL)
          DefRingParlp();
        else
          VMrDefaultlp();
      else
        if (rParameter(currRing) != NULL)
          DefRingPar(target_tmp);
        else
          rChangeCurrRing(VMrDefault(target_tmp));

      lpRing = currRing;
      G1 = idrMoveR(G, newRing,currRing);

#ifdef TIME_TEST
      to=clock();
#endif
      /*apply kStd or LastGB to compute  a lex. red. Groebner basis of <G>*/
      if(nP == 0 || MivSame(target_tmp, iv_lp) == 0){
        //Print("\n\n// calls \"std in ring r_%d = %s;", nwalk, rString(currRing));
        G = MstdCC(G1);//no result for qnt1
      }
      else {
        rChangeCurrRing(newRing);
        G1 = idrMoveR(G1, lpRing,currRing);

        //Print("\n\n// calls \"LastGB\" (%d) to compute a GB", nV-1);
        G = LastGB(G1, curr_weight, nV-1); //no result for kats7

        rChangeCurrRing(lpRing);
        G = idrMoveR(G, newRing,currRing);
      }
#ifdef TIME_TEST
      textra=clock()-to;
#endif
      npert[endwalks]=nwalk-npert_tmp;
      npert_tmp = nwalk;
      endwalks ++;
      break;
    }

    /* check whether the computed Groebner basis is really a Groebner basis.
       If not, we perturb the target vector with the maximal "perturbation"
       degree.*/
    if(MivComp(next_weight, target_weight) == 1 ||
       MivComp(next_weight, curr_weight) == 1 )
    {
      //Print("\n//ring r_%d = %s;", nwalk, rString(currRing));


      //compute the number of perturbations and its step
      npert[endwalks]=nwalk-npert_tmp;
      npert_tmp = nwalk;

      endwalks ++;

      /*it is very important if the walk only uses one step, e.g. Fate, liu*/
      if(endwalks == 1 && MivComp(next_weight, curr_weight) == 1){
        rChangeCurrRing(XXRing);
        G = idrMoveR(G, newRing,currRing);
        goto FINISH;
      }
      H0 = id_Head(G,currRing);

      if(MivSame(target_tmp, iv_lp) == 1)
        if (rParameter(currRing) != NULL)
          DefRingParlp();
        else
          VMrDefaultlp();
      else
        if (rParameter(currRing) != NULL)
          DefRingPar(target_tmp);
        else
          rChangeCurrRing(VMrDefault(target_tmp));

      lpRing = currRing;
      Glp = idrMoveR(G, newRing,currRing);
      H2 = idrMoveR(H0, newRing,currRing);

      /* Apply Lemma 2.2 in Collart et. al (1997) to check whether
         cone(k-1) is equal to cone(k) */
      nGB = 1;
      for(i=IDELEMS(Glp)-1; i>=0; i--)
      {
        poly t;
        if((t=pSub(pHead(Glp->m[i]), pCopy(H2->m[i]))) != NULL)
        {
          pDelete(&t);
          idDelete(&H2);//5.5.02
          nGB = 0; //i.e. Glp is no reduced Groebner basis
          break;
        }
        pDelete(&t);
      }

      idDelete(&H2);//5.5.02

      if(nGB == 1)
      {
        G = Glp;
        Glp = NULL;
        break;
      }

       /* perturb the target weight vector, if the vector target_tmp
          stays in many cones */
      poly p;
      BOOLEAN plength3 = FALSE;
      for(i=IDELEMS(Glp)-1; i>=0; i--)
      {
        p = MpolyInitialForm(Glp->m[i], target_tmp);
        if(p->next != NULL &&
           p->next->next != NULL &&
           p->next->next->next != NULL)
        {
          Overflow_Error = FALSE;

          for(i=0; i<nV; i++)
            (*vector_tmp)[i] = (*target_weight)[i];

          delete target_weight;
          target_weight = MPertVectors(Glp, Mlp, nV);

          if(MivComp(vector_tmp, target_weight)==1)
          {
            //PrintS("\n// The old and new representaion vector are the same!!");
            G = Glp;
            newRing = currRing;
            goto OMEGA_OVERFLOW_TRAN_NEW;
           }

          if(Overflow_Error == TRUE)
          {
            rChangeCurrRing(newRing);
            G = idrMoveR(Glp, lpRing,currRing);
            goto OMEGA_OVERFLOW_TRAN_NEW;
          }

          plength3 = TRUE;
          pDelete(&p);
          break;
        }
        pDelete(&p);
      }

      if(plength3 == FALSE)
      {
        rChangeCurrRing(newRing);
        G = idrMoveR(Glp, lpRing,currRing);
        goto TRAN_LIFTING;
      }


      nwalkpert = 1;
      nsteppert ++;

      /*
      Print("\n// Subroutine needs (%d) steps.", nwalk);
      idElements(Glp, "last G in walk:");
      PrintS("\n// ****************************************");
      Print("\n// Perturb the original target vector (%d): ", nsteppert);
      ivString(target_weight, "new target");
      PrintS("\n// ****************************************\n");
      */
      rChangeCurrRing(newRing);
      G = idrMoveR(Glp, lpRing,currRing);

      delete next_weight;

      //Print("\n// ring rNEW = %s;", rString(currRing));
      goto COMPUTE_NEW_VECTOR;
    }

  TRAN_LIFTING:
    for(i=nV-1; i>=0; i--)
      (*curr_weight)[i] = (*next_weight)[i];

    delete next_weight;
  }//while
#ifdef TEST_OVERFLOW
 BE_FINISH:
#endif
  rChangeCurrRing(XXRing);
  G = idrMoveR(G, lpRing,currRing);

 FINISH:
  delete ivNull;
  delete next_weight;
  delete iv_lp;
  omFree(npert);
/*
#ifdef TIME_TEST
  Print("\n// Computation took %d steps and %.2f sec",
        nwalk, ((double) (clock()-mtim)/1000000));

  TimeStringFractal(tinput, tostd, tif, tstd, textra, tlift, tred, tnw);

 // Print("\n// pSetm_Error = (%d)", ErrorCheck());
  Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
#endif
*/
  return(G);
}

#if 0
/*******************************************************
 * Tran's algorithm with random element                *
 *                                                     *
 * use kStd, if nP = 0, else call Ab_Rec_Pert (LastGB) *
 *******************************************************/
ideal TranMrImprovwalk(ideal G,intvec* curr_weight,intvec* target_tmp, int nP, int weight_rad, int pert_deg)
{
#ifdef TIME_TEST
  clock_t mtim = clock();
#endif
  Set_Error(FALSE  );
  Overflow_Error =  FALSE;
  //Print("// pSetm_Error = (%d)", ErrorCheck());
  //Print("\n// ring ro = %s;", rString(currRing));

#ifdef TIME_TEST
  clock_t tostd, tif=0, tstd=0, tlift=0, tred=0, tnw=0, textra=0;
  clock_t tinput = clock();
#endif
  int nsteppert=0, i, nV = currRing->N, nwalk=0, npert_tmp=0;
  int *npert=(int*)omAlloc(2*nV*sizeof(int));
  ideal Gomega, M,F,  G1, Gomega1, Gomega2, M1, F1;
  //ring endRing;
  ring newRing, oldRing, lpRing;
  intvec* next_weight;
  intvec* ivNull = new intvec(nV); //define (0,...,0)
  intvec* iv_dp = MivUnit(nV);// define (1,1,...,1)
  intvec* iv_lp = Mivlp(nV); //define (1,0,...,0)
  ideal H0;
  //ideal H1;
  ideal H2, Glp;
  int weight_norm, nGB, endwalks = 0,  nwalkpert=0,  npertstep=0;
  intvec* Mlp =  MivMatrixOrderlp(nV);
  intvec* vector_tmp = new intvec(nV);
#ifndef BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;

//intvec* extra_curr_weight = new intvec(nV);
  intvec* target_weight = new intvec(nV);
  for(i=nV-1; i>=0; i--)
  {
    (*target_weight)[i] = (*target_tmp)[i];
  }
  ring XXRing = currRing;
  newRing = currRing;

#ifdef TIME_TEST
  to=clock();
#endif
  // compute a red. GB w.r.t. the help ring
  if(MivComp(curr_weight, iv_dp) == 1)
  {
    //rOrdStr(currRing) = "dp"
    G = MstdCC(G);
  }
  else
  {
    //rOrdStr(currRing) = (a(.c_w..),lp,C)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    G = idrMoveR(G, XXRing,currRing);
    G = MstdCC(G);
  }
#ifdef TIME_TEST
  tostd=clock()-to;
#endif

#ifdef REPRESENTATION_OF_SIGMA
  ideal Gw = MwalkInitialForm(G, curr_weight);

  if(islengthpoly2(Gw)==1)
  {
    intvec* MDp;
    if(MivComp(curr_weight, iv_dp) == 1)
    {
      MDp = MatrixOrderdp(nV); //MivWeightOrderlp(iv_dp);
    }
    else
    {
      MDp = MivWeightOrderlp(curr_weight);
    }
    curr_weight = RepresentationMatrix_Dp(G, MDp);

    delete MDp;

    ring exring = currRing;

    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
#ifdef TIME_TEST
    to=clock();
#endif
    Gw = idrMoveR(G, exring,currRing);
    G = MstdCC(Gw);
    Gw = NULL;
#ifdef TIME_TEST
    tostd=tostd+clock()-to;
#endif
    //ivString(curr_weight,"rep. sigma");
    goto COMPUTE_NEW_VECTOR;
  }

  idDelete(&Gw);
  delete iv_dp;
#endif


  while(1)
  {
#ifdef TIME_TEST
    to=clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    tif=tif+clock()-to;
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    // define a new ring with ordering "(a(curr_weight),lp)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif
#ifdef TIME_TEST
    tstd=tstd+clock()-to;
#endif

    // change the ring to oldRing
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // compute a representation of the generators of submod (M) with respect to those of mod (Gomega).
    // Gomega is a reduced Groebner basis w.r.t. the current ring
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    tlift=tlift+clock()-to;
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // reduce the Groebner basis <G> w.r.t. new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    tred=tred+clock()-to;
#endif
    idDelete(&F1);

  COMPUTE_NEW_VECTOR:
    newRing = currRing;
    nwalk++;
    nwalkpert++;
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a next weight vector
    //next_weight = MwalkNextWeightCC(curr_weight,target_weight, G);
    next_weight = MWalkRandomNextWeight(G, curr_weight, target_weight, weight_rad, pert_deg);
/*
    next_weight = MkInterRedNextWeight(curr_weight,target_weight,G);

    if(MivComp(next_weight, target_weight) != 1)
    {
      // compute a perturbed next weight vector "next_weight1"
      intvec* next_weight1 = MkInterRedNextWeight(MPertVectors(G, MivMatrixOrder(curr_weight), pert_deg), target_weight, G);

      // compare next_weight and next_weight1
      ideal G_test = MwalkInitialForm(G, next_weight);
      ideal G_test1 = MwalkInitialForm(G, next_weight1);
      if(IDELEMS(G_test1) <= IDELEMS(G_test))
      {
        next_weight = ivCopy(next_weight1);
      }
      delete next_weight1;
      // compute a random next weight vector "next_weight2"
      intvec* next_weight22 = ivCopy(target_weight);
      // Print("\n//  size of target_weight  = %d", sizeof((*target_weight)));
      k = 0;

      while(test_w_in_ConeCC(G, next_weight22) == 0 && k < 11)
      {
        k++;
        if(k>10)
        {
          break;
        }
        weight_norm = 0;
        while(weight_norm == 0)
        {
          for(i=nV-1; i>=0; i--)
          {
            // Print("\n//  next_weight[%d]  = %d", i, (*next_weight)[i]);
            (*next_weight22)[i] = rand() % 60000 - 30000;
            weight_norm = weight_norm + (*next_weight22)[i]*(*next_weight22)[i];
          }
          weight_norm = 1 + floor(sqrt(weight_norm));
        }
        for(i=nV-1; i>=0; i--)
        {
          if((*next_weight22)[i] < 0)
          {
            (*next_weight22)[i] = 1 + (*curr_weight)[i] + floor(weight_rad*(*next_weight22)[i]/weight_norm);
          }
          else
          {
            (*next_weight22)[i] = (*curr_weight)[i] + floor(weight_rad*(*next_weight22)[i]/weight_norm);
          }
          // Print("\n//  next_weight22[%d]  = %d", i, (*next_weight22)[i]);
        }
      }

      if(test_w_in_ConeCC(G, next_weight22) == 1)
      {
        // compare next_weight and next_weight2
        // Print("\n// ZUFALL IM KEGEL");
        intvec* next_weight2 = MkInterRedNextWeight(next_weight22, target_weight, G);

        ideal G_test2 = MwalkInitialForm(G, next_weight2);
        if(IDELEMS(G_test2) <= IDELEMS(G_test))
        {
          if(IDELEMS(G_test2) <= IDELEMS(G_test1))
          {
             // Print("\n// ZUFALL BENUTZT!\n");
            next_weight = ivCopy(next_weight2);
          }
        }
        idDelete(&G_test2);
        delete next_weight2;
      }
      delete next_weight22;
      idDelete(&G_test);
      idDelete(&G_test1);
    }*/

#ifdef TIME_TEST
    tnw=tnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

/*   check whether the computed intermediate weight vector is in
     the correct cone; sometimes it is very big e.g. s7, cyc7.
     If it is NOT in the correct cone, then compute directly
     a reduced Groebner basis with respect to the lexicographic ordering
     for the known Groebner basis that it is computed in the last step.
*/
    //if(test_w_in_ConeCC(G, next_weight) != 1)
    if(Overflow_Error == TRUE)
    {
    OMEGA_OVERFLOW_TRAN_NEW:
      //Print("\n//  takes %d steps!", nwalk-1);
      //Print("\n//ring lastRing = %s;", rString(currRing));
#ifdef TEST_OVERFLOW
      goto  BE_FINISH;
#endif

#ifdef CHECK_IDEAL_MWALK
      idElements(G, "G");
      //headidString(G, "G");
#endif

      if(MivSame(target_tmp, iv_lp) == 1)
      {
        if (rParameter(currRing) != NULL)
        {
          DefRingParlp();
        }
        else
        {
          VMrDefaultlp();
        }
      }
      else
      {
        if (rParameter(currRing) != NULL)
        {
          DefRingPar(target_tmp);
        }
        else
        {
          rChangeCurrRing(VMrDefault(target_tmp));
        }
      }
      lpRing = currRing;
      G1 = idrMoveR(G, newRing,currRing);

#ifdef TIME_TEST
      to=clock();
#endif
      // apply kStd or LastGB to compute  a lex. red. Groebner basis of <G>
      if(nP == 0 || MivSame(target_tmp, iv_lp) == 0)
      {
        //Print("\n\n// calls \"std in ring r_%d = %s;", nwalk, rString(currRing));
        G = MstdCC(G1);//no result for qnt1
      }
      else
      {
        rChangeCurrRing(newRing);
        G1 = idrMoveR(G1, lpRing,currRing);

        //Print("\n\n// calls \"LastGB\" (%d) to compute a GB", nV-1);
        G = LastGB(G1, curr_weight, nV-1); //no result for kats7

        rChangeCurrRing(lpRing);
        G = idrMoveR(G, newRing,currRing);
      }
#ifdef TIME_TEST
      textra=clock()-to;
#endif
      npert[endwalks]=nwalk-npert_tmp;
      npert_tmp = nwalk;
      endwalks ++;
      break;
    }

    // check whether the computed Groebner basis is really a Groebner basis.
    // If not, we perturb the target vector with the maximal "perturbation" degree.

    if(MivComp(next_weight, target_weight) == 1 || MivComp(next_weight, curr_weight) == 1 )
    {
      //Print("\n//ring r_%d = %s;", nwalk, rString(currRing));


      //compute the number of perturbations and its step
      npert[endwalks]=nwalk-npert_tmp;
      npert_tmp = nwalk;

      endwalks ++;

      // it is very important if the walk only uses one step, e.g. Fate, liu
      if(endwalks == 1 && MivComp(next_weight, curr_weight) == 1)
      {
        rChangeCurrRing(XXRing);
        G = idrMoveR(G, newRing,currRing);
        goto FINISH;
      }
      H0 = id_Head(G,currRing);

      if(MivSame(target_tmp, iv_lp) == 1)
      {
        if (rParameter(currRing) != NULL)
        {
          DefRingParlp();
        }
        else
        {
          VMrDefaultlp();
        }
      }
      else
      {
        if (rParameter(currRing) != NULL)
        {
          DefRingPar(target_tmp);
        }
        else
        {
          rChangeCurrRing(VMrDefault(target_tmp));
        }
      }
      lpRing = currRing;
      Glp = idrMoveR(G, newRing,currRing);
      H2 = idrMoveR(H0, newRing,currRing);

      // Apply Lemma 2.2 in Collart et. al (1997) to check whether cone(k-1) is equal to cone(k)
      nGB = 1;
      for(i=IDELEMS(Glp)-1; i>=0; i--)
      {
        poly t;
        if((t=pSub(pHead(Glp->m[i]), pCopy(H2->m[i]))) != NULL)
        {
          pDelete(&t);
          idDelete(&H2);//5.5.02
          nGB = 0; //i.e. Glp is no reduced Groebner basis
          break;
        }
        pDelete(&t);
      }

      idDelete(&H2);//5.5.02

      if(nGB == 1)
      {
        G = Glp;
        Glp = NULL;
        break;
      }

       // perturb the target weight vector, if the vector target_tmp stays in many cones
      poly p;
      BOOLEAN plength3 = FALSE;
      for(i=IDELEMS(Glp)-1; i>=0; i--)
      {
        p = MpolyInitialForm(Glp->m[i], target_tmp);
        if(p->next != NULL &&
           p->next->next != NULL &&
           p->next->next->next != NULL)
        {
          Overflow_Error = FALSE;

          for(i=0; i<nV; i++)
          {
            (*vector_tmp)[i] = (*target_weight)[i];
          }
          delete target_weight;
          target_weight = MPertVectors(Glp, Mlp, nV);

          if(MivComp(vector_tmp, target_weight)==1)
          {
            //PrintS("\n// The old and new representaion vector are the same!!");
            G = Glp;
            newRing = currRing;
            goto OMEGA_OVERFLOW_TRAN_NEW;
           }

          if(Overflow_Error == TRUE)
          {
            rChangeCurrRing(newRing);
            G = idrMoveR(Glp, lpRing,currRing);
            goto OMEGA_OVERFLOW_TRAN_NEW;
          }

          plength3 = TRUE;
          pDelete(&p);
          break;
        }
        pDelete(&p);
      }

      if(plength3 == FALSE)
      {
        rChangeCurrRing(newRing);
        G = idrMoveR(Glp, lpRing,currRing);
        goto TRAN_LIFTING;
      }


      npertstep = nwalk;
      nwalkpert = 1;
      nsteppert ++;

      /*
      Print("\n// Subroutine needs (%d) steps.", nwalk);
      idElements(Glp, "last G in walk:");
      PrintS("\n// ****************************************");
      Print("\n// Perturb the original target vector (%d): ", nsteppert);
      ivString(target_weight, "new target");
      PrintS("\n// ****************************************\n");
      */
      rChangeCurrRing(newRing);
      G = idrMoveR(Glp, lpRing,currRing);

      delete next_weight;

      //Print("\n// ring rNEW = %s;", rString(currRing));
      goto COMPUTE_NEW_VECTOR;
    }

  TRAN_LIFTING:
    for(i=nV-1; i>=0; i--)
    {
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  } // end of while
#ifdef TEST_OVERFLOW
 BE_FINISH:
#endif
  rChangeCurrRing(XXRing);
  G = idrMoveR(G, lpRing,currRing);

 FINISH:
  delete ivNull;
  delete next_weight;
  delete iv_lp;
  omFree(npert);

#ifdef TIME_TEST
  Print("\n// Computation took %d steps and %.2f sec", nwalk, ((double) (clock()-mtim)/1000000));

  TimeStringFractal(tinput, tostd, tif, tstd, textra, tlift, tred, tnw);

  Print("\n// pSetm_Error = (%d)", ErrorCheck());
  Print("\n// Overflow_Error? (%d)\n", Overflow_Error);
#endif

  return(G);
}
#endif

/*****************************************************************
 * compute the reduced Groebner basis of an ideal <Go> w.r.t. lp *
 *****************************************************************/
static ideal Mpwalk_MAltwalk1(ideal Go, intvec* curr_weight, int tp_deg)
{
  Overflow_Error = FALSE;
 // BOOLEAN nOverflow_Error = FALSE;
#ifdef TIME_TEST
  clock_t tproc=0;
  clock_t tinput=clock();
#endif
  int i, nV = currRing->N;

  //check that perturbation degree is valid
  if(tp_deg < 1 || tp_deg > nV)
  {
    WerrorS("Invalid perturbation degree.\n");
    return NULL;
  }

  int nwalk=0, endwalks=0, ntestwinC=1;
  int tp_deg_tmp = tp_deg;
  ideal Gomega, M, F, G, M1, F1, Gomega1, Gomega2, G1;
  ring newRing, oldRing, TargetRing;
  intvec* next_weight;
  intvec* ivNull = new intvec(nV);

  ring YXXRing = currRing;

  intvec* iv_M_dpp = MivMatrixOrderlp(nV);
  intvec* target_weight;// = Mivlp(nV);
  ideal ssG;

  // perturb the target vector
  while(1)
  {
    if(Overflow_Error == FALSE)
    {
      if (rParameter(currRing) != NULL)
      {
        DefRingParlp();
      }
      else
      {
        VMrDefaultlp();
      }
      TargetRing = currRing;
      ssG = idrMoveR(Go,YXXRing,currRing);
    }
    Overflow_Error = FALSE;
    if(tp_deg != 1)
    {
      target_weight = MPertVectors(ssG, iv_M_dpp, tp_deg);
    }
    else
    {
      target_weight = Mivlp(nV);
      break;
    }
    if(Overflow_Error == FALSE)
    {
      break;
    }
    Overflow_Error = TRUE;
    tp_deg --;
  }
  if(tp_deg != tp_deg_tmp)
  {
    Overflow_Error = TRUE;
    //nOverflow_Error = TRUE;
  }

  //  Print("\n// tp_deg = %d", tp_deg);
  // ivString(target_weight, "pert target");

  delete iv_M_dpp;
#ifndef  BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;

  rChangeCurrRing(YXXRing);
  G = idrMoveR(ssG, TargetRing,currRing);

  while(1)
  {
    nwalk ++;
    nstep ++;

    if(nwalk==1)
    {
      goto FIRST_STEP;
    }
#ifdef TIME_TEST
    to=clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif

#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    else
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
#endif

    oldRing = currRing;

    // define a new ring that its ordering is "(a(curr_weight),lp)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);
/*
#ifdef ENDWALKS
    if(endwalks == 1)
    {
      Print("\n//  it is  %d-th step!!", nwalk);
      idString(Gomega1, "Gw");
      PrintS("\n//  compute a rGB of Gw:");
    }
#endif
*/
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif // BUCHBERGER_ALG
#ifdef TIME_TEST
    xtstd=xtstd+clock()-to;
#endif

    // change the ring to oldRing
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif

    // if(endwalks == 1){PrintS("\n//  Lifting is still working:");}

    // compute a reduced Groebner basis of <G> w.r.t. "newRing" by the lifting process
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);
#ifdef TIME_TEST
    to=clock();
#endif
    //if(endwalks == 1){ PrintS("\n//  InterRed is still working:");}
    // reduce the Groebner basis <G> w.r.t. the new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);

    if(endwalks == 1)
      break;

  FIRST_STEP:
    Overflow_Error=FALSE;
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a next weight vector
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif

    if(Overflow_Error == TRUE)
    {
      delete next_weight;
      if(tp_deg > 1){
        //nOverflow_Error = Overflow_Error;
#ifdef TIME_TEST
        tproc = tproc+clock()-tinput;
#endif
        //Print("\n// A subroutine takes %d steps and calls \"Mpwalk\" (1,%d):", nwalk, tp_deg-1);
        G1 = Mpwalk_MAltwalk1(G, curr_weight, tp_deg-1);
        goto MPW_Finish;
      }
      else {
        newRing = currRing;
        ntestwinC = 0;
        break;
      }
    }

    if(MivComp(next_weight, ivNull) == 1)
    {
      newRing = currRing;
      delete next_weight;
      break;
    }
    if(MivComp(next_weight, target_weight) == 1)
    {
      endwalks = 1;
    }
    for(i=nV-1; i>=0; i--)
    {
      //(*extra_curr_weight)[i] = (*curr_weight)[i];
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }//while

  // check whether the pertubed target vector is correct

  //define and execute ring with lex. order
  if (rParameter(currRing) != NULL)
  {
    DefRingParlp();
  }
  else
  {
    VMrDefaultlp();
  }
  G1 = idrMoveR(G, newRing,currRing);

  if( test_w_in_ConeCC(G1, target_weight) != 1 || ntestwinC == 0)
  {
    //PrintS("\n// The perturbed target vector doesn't STAY in the correct cone!!");
    if(tp_deg == 1)
    {
      //Print("\n// subroutine takes %d steps and applys \"std\"", nwalk);
#ifdef TIME_TEST
      to=clock();
#endif
      ideal G2 = MstdCC(G1);
#ifdef TIME_TEST
      xtextra=xtextra+clock()-to;
#endif
      idDelete(&G1);
      G1 = G2;
      G2 = NULL;
    }
    else
    {
      //nOverflow_Error = Overflow_Error;
#ifdef TIME_TEST
      tproc = tproc+clock()-tinput;
#endif
      // Print("\n// B subroutine takes %d steps and calls \"Mpwalk\" (1,%d) :", nwalk,  tp_deg-1);
      G1 = Mpwalk_MAltwalk1(G1, curr_weight, tp_deg-1);
    }
  }

 MPW_Finish:
  newRing = currRing;
  rChangeCurrRing(YXXRing);
  ideal result = idrMoveR(G1, newRing,currRing);

  delete ivNull;
  delete target_weight;

  //Print("\n// \"Mpwalk\" (1,%d) took %d steps and %.2f sec. Overflow_Error (%d)", tp_deg, nwalk, ((double) clock()-tinput)/1000000, nOverflow_Error);
  //Print("\n// Mprwalk took %d steps. Ring= %s;\n", nwalk, rString(currRing));
  return(result);
}

/*******************************************************************
 * Implementation of the first alternative Groebner Walk Algorithm *
 *******************************************************************/
ideal MAltwalk1(ideal Go, int op_deg, int tp_deg, intvec* curr_weight,
                intvec* target_weight)
{
  Set_Error(FALSE  );
  Overflow_Error = FALSE;
#ifdef TIME_TEST
  BOOLEAN nOverflow_Error = FALSE;
#endif
  // Print("// pSetm_Error = (%d)", ErrorCheck());

#ifdef TIME_TEST
  xtif=0; xtstd=0; xtlift=0; xtred=0; xtnw=0; xtextra=0;
  xftinput = clock();
  clock_t tostd, tproc;
#endif

  nstep = 0;
  int i, nV = currRing->N;
  int nwalk=0, endwalks=0;
  int op_tmp = op_deg;
  ideal Gomega, M, F, G, Gomega1, Gomega2, M1, F1;
  ring newRing, oldRing;
  intvec* next_weight;
  intvec* iv_M_dp;
  intvec* ivNull = new intvec(nV);
  intvec* iv_dp = MivUnit(nV);// define (1,1,...,1)
  intvec* exivlp = Mivlp(nV);
  //intvec* extra_curr_weight = new intvec(nV);
#ifndef  BUCHBERGER_ALG
  intvec* hilb_func;
#endif
  intvec* cw_tmp = curr_weight;

  // to avoid (1,0,...,0) as the target vector
  intvec* last_omega = new intvec(nV);
  for(i=nV-1; i>0; i--)
  {
    (*last_omega)[i] = 1;
  }
  (*last_omega)[0] = 10000;

  ring XXRing = currRing;

#ifdef TIME_TEST
  to=clock();
#endif
  /* compute a pertubed weight vector of the original weight vector.
     The perturbation degree is recursive decrease until that vector
     stays inn the correct cone. */
  while(1)
  {
    if(Overflow_Error == FALSE)
    {
      if(MivComp(curr_weight, iv_dp) == 1)
      {
      //rOrdStr(currRing) = "dp"
        if(op_tmp == op_deg)
        {
          G = MstdCC(Go);
          if(op_deg != 1)
          {
            iv_M_dp = MivMatrixOrderdp(nV);
          }
        }
      }
    }
    else
    {
      if(op_tmp == op_deg)
      {
        //rOrdStr(currRing) = (a(...),lp,C)
        if (rParameter(currRing) != NULL)
        {
          DefRingPar(cw_tmp);
        }
        else
        {
          rChangeCurrRing(VMrDefault(cw_tmp));
        }
        G = idrMoveR(Go, XXRing,currRing);
        G = MstdCC(G);
        if(op_deg != 1)
          iv_M_dp = MivMatrixOrder(cw_tmp);
      }
    }
    Overflow_Error = FALSE;
    if(op_deg != 1)
    {
      curr_weight = MPertVectors(G, iv_M_dp, op_deg);
    }
    else
    {
      curr_weight =  cw_tmp;
      break;
    }
    if(Overflow_Error == FALSE)
    {
      break;
    }
    Overflow_Error = TRUE;
    op_deg --;
  }
#ifdef TIME_TEST
  tostd=clock()-to;
#endif

  if(op_tmp != 1 )
    delete iv_M_dp;
  delete iv_dp;

  if(currRing->order[0] == ringorder_a)
    goto NEXT_VECTOR;

  while(1)
  {
    nwalk ++;
    nstep ++;

#ifdef TIME_TEST
    to = clock();
#endif
    // compute an initial form ideal of <G> w.r.t. "curr_vector"
    Gomega = MwalkInitialForm(G, curr_weight);
#ifdef TIME_TEST
    xtif=xtif+clock()-to;
#endif
#if 0
    if(Overflow_Error == TRUE)
    {
      for(i=nV-1; i>=0; i--)
        (*curr_weight)[i] = (*extra_curr_weight)[i];
      delete extra_curr_weight;

      newRing = currRing;
      goto MSTD_ALT1;
    }
#endif
#ifndef  BUCHBERGER_ALG
    if(isNolVector(curr_weight) == 0)
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,curr_weight,currRing);
    }
    else
    {
      hilb_func = hFirstSeries(Gomega,NULL,NULL,last_omega,currRing);
    }
#endif // BUCHBERGER_ALG

    oldRing = currRing;

    // define a new ring which ordering is "(a(curr_weight),lp)
    if (rParameter(currRing) != NULL)
    {
      DefRingPar(curr_weight);
    }
    else
    {
      rChangeCurrRing(VMrDefault(curr_weight));
    }
    newRing = currRing;
    Gomega1 = idrMoveR(Gomega, oldRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // compute a reduced Groebner basis of <Gomega> w.r.t. "newRing"
#ifdef  BUCHBERGER_ALG
    M = MstdhomCC(Gomega1);
#else
    M=kStd(Gomega1,NULL,isHomog,NULL,hilb_func,0,NULL,curr_weight);
    delete hilb_func;
#endif // BUCHBERGER_ALG
#ifdef TIME_TEST
    xtstd=xtstd+clock()-to;
#endif

    // change the ring to oldRing
    rChangeCurrRing(oldRing);
    M1 =  idrMoveR(M, newRing,currRing);
    Gomega2 =  idrMoveR(Gomega1, newRing,currRing);

#ifdef TIME_TEST
    to=clock();
#endif
    // compute a reduced Groebner basis of <G> w.r.t. "newRing" by the lifting process
    F = MLifttwoIdeal(Gomega2, M1, G);
#ifdef TIME_TEST
    xtlift=xtlift+clock()-to;
#endif

    idDelete(&M1);
    idDelete(&Gomega2);
    idDelete(&G);

    // change the ring to newRing
    rChangeCurrRing(newRing);
    F1 = idrMoveR(F, oldRing,currRing);
    if (oldRing!=IDRING(currRingHdl)) rDelete(oldRing); // do not delete the global currRing
    oldRing=NULL;

#ifdef TIME_TEST
    to=clock();
#endif
    // reduce the Groebner basis <G> w.r.t. new ring
    G = kInterRedCC(F1, NULL);
#ifdef TIME_TEST
    xtred=xtred+clock()-to;
#endif
    idDelete(&F1);

    if(endwalks == 1)
    {
      break;
    }
  NEXT_VECTOR:
#ifdef TIME_TEST
    to=clock();
#endif
    // compute a next weight vector
    next_weight = MkInterRedNextWeight(curr_weight,target_weight, G);
#ifdef TIME_TEST
    xtnw=xtnw+clock()-to;
#endif
#ifdef PRINT_VECTORS
    MivString(curr_weight, target_weight, next_weight);
#endif
    if(Overflow_Error == TRUE)
    {
      newRing = currRing;

      if (rParameter(currRing) != NULL)
      {
        DefRingPar(target_weight);
      }
      else
      {
        rChangeCurrRing(VMrDefault(target_weight));
      }
      F1 = idrMoveR(G, newRing,currRing);
      G = MstdCC(F1);
      idDelete(&F1);
      newRing = currRing;
      break; //for while
    }


    /* G is the wanted Groebner basis if next_weight == curr_weight */
    if(MivComp(next_weight, ivNull) == 1)
    {
      newRing = currRing;
      delete next_weight;
      break; //for while
    }

    if(MivComp(next_weight, target_weight) == 1)
    {
      if(tp_deg == 1 || MivSame(target_weight, exivlp) == 0)
        endwalks = 1;
      else
      {
       // MSTD_ALT1:
#ifdef TIME_TEST
        nOverflow_Error = Overflow_Error;
        tproc = clock()-xftinput;
#endif

        //Print("\n//  main routine takes %d steps and calls \"Mpwalk\" (1,%d):", nwalk,  tp_deg);

        // compute the red. GB of <G> w.r.t. the lex order by the "recursive-modified" perturbation walk alg (1,tp_deg)
        G = Mpwalk_MAltwalk1(G, curr_weight, tp_deg);
        delete next_weight;
        break; // for while
      }
    }

    //NOT Changed, to free memory
    for(i=nV-1; i>=0; i--)
    {
      //(*extra_curr_weight)[i] = (*curr_weight)[i];
      (*curr_weight)[i] = (*next_weight)[i];
    }
    delete next_weight;
  }//while

  rChangeCurrRing(XXRing);
  ideal result = idrMoveR(G, newRing,currRing);
  id_Delete(&G, newRing);

  delete ivNull;
  if(op_deg != 1 )
  {
    delete curr_weight;
  }
  delete exivlp;
#ifdef TIME_TEST
/*
  Print("\n// \"Main procedure\"  took %d steps, %.2f sec. and Overflow_Error(%d)",
        nwalk, ((double) tproc)/1000000, nOverflow_Error);

  TimeStringFractal(xftinput, tostd, xtif, xtstd,xtextra, xtlift, xtred, xtnw);
*/
 // Print("\n// pSetm_Error = (%d)", ErrorCheck());
 // Print("\n// Overflow_Error? (%d)", Overflow_Error);
 // Print("\n// Awalk1 took %d steps.\n", nstep);
#endif
  return(result);
}
