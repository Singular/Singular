/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: walk.cc,v 1.1.1.1 2003-10-06 12:16:04 Singular Exp $ */
/*
* ABSTRACT: Implementation of the Groebner walk
*/

/* includes */
#include "mod2.h"
#include "walk.h"
#include "polys.h"
#include "ideals.h"
#include "intvec.h"
#include "ipid.h"
#include "tok.h"
#include <omalloc.h>
#include "febase.h"
#include "numbers.h"
#include "ipid.h"
#include "ring.h"
#include "kstd1.h"
#include "matpol.h"
#include "weight.h"
#include "intvec.h"
#include "syz.h"
#include "lists.h"
#include "prCopy.h"
#include <string.h>
#include "structs.h"
#include "longalg.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
#endif

static void* idString(ideal L)
{
  int i;
  printf("//ideal Itmp: ");
  for(i=0; i<IDELEMS(L); i++)
    printf(" %s, ", pString(L->m[i]));
  printf("\n");
}


// returns gcd of integers a and b
static inline long gcd(const long a, const long b)
{
  long r, p0 = a, p1 = b;
  //assume(p0 >= 0 && p1 >= 0);
  if(p0 < 0)
    p0 = -p0;

  if(p1 < 0)
    p1 = -p1;
  while(p1 != 0)
  {
    r = p0 % p1;
    p0 = p1;
    p1 = r;
  }
  return p0;
}

// cancel gcd of integers zaehler and nenner
static inline void cancel(long &zaehler, long &nenner)
{
  assume(zaehler >= 0 && nenner > 0);
  long g = gcd(zaehler, nenner);
  if (g > 1)
  {
    zaehler = zaehler / g;
    nenner = nenner / g;
  }
}

/********************************************************************
 * compute a weight degree of a monomial p w.r.t. a weight_vector   *
 ********************************************************************/
static inline int MLmWeightedDegree(const poly p, intvec* weight)
{
  int i, d = 0;

  for (i=1; i<=pVariables; i++)
    d += pGetExp(p, i) * (*weight)[i-1];

  return d;
}

/********************************************************************
 * compute a weight degree of a polynomial p w.r.t. a weight_vector *
 ********************************************************************/
static inline int MwalkWeightDegree(poly p, intvec* weight_vector)
{
  assume(weight_vector->length() >= pVariables);
  int max = 0, maxtemp;
  poly hp;

  while(p != NULL)
  {
    hp = pHead(p);
    pIter(p);
    maxtemp = MLmWeightedDegree(hp, weight_vector);

    if (maxtemp > max)
      max = maxtemp;
  }
  return max;
}

/*****************************************************************************
 * return an initial form of the polynom g w.r.t. a weight vector curr_weight *
 *****************************************************************************/
static poly MpolyInitialForm(poly g, intvec* curr_weight)
{
  if(g == NULL)
    return g;

  int maxtmp, max = 0;
  poly in_w_g = NULL, hg;

  while(g != NULL)
  {
    hg = pHead(g);
    pIter(g);
    maxtmp = MwalkWeightDegree(hg, curr_weight);

    if(maxtmp > max)
    {
      max = maxtmp;
      in_w_g = hg;
    } else {
      if(maxtmp == max)
	in_w_g = pAdd(in_w_g, hg);
    }
  }
  return in_w_g;
}


/*****************************************************************************
 * compute the initial form of an ideal "G" w.r.t. weight vector curr_weight *
 ****************************************************************************/
ideal MwalkInitialForm(ideal G, intvec* curr_weight)
{
  int i;
  int nG = IDELEMS(G);
  ideal Gomega  = idInit(nG, G->rank);

  for(i=0; i<nG; i++)
    Gomega->m[i] = MpolyInitialForm(G->m[i], curr_weight);

  //return Gomega;
  ideal result = idCopy(Gomega);
  idDelete(&Gomega);
  return result;
}

/************************************************************************
 * test that does the weight vector iv exist in the cone of the ideal G *
 *            i.e. does in(in_w(g)) =? in(g), for all g in G            *
 ************************************************************************/
void* test_w_in_Cone(ideal G, intvec* iv)
{
  int nG = IDELEMS(G);
  int i;
  BOOLEAN ok = TRUE;
  poly mi, in_mi,  gi;
  for(i=0; i<nG; i++)
  {
    mi = MpolyInitialForm(G->m[i], iv);
    in_mi = pHead(mi);
    gi = pHead(G->m[i]);
    if(pEqualPolys(in_mi, gi) != ok)
    {
      printf("//ring Test_W_in_Cone = %s ;\n", rString(currRing));
      printf("//the computed next weight vector doesn't exist in the cone\n");
      break;
    }
  }
}

//compute a least common multiple of two integers
static inline long Mlcm(long &i1, long &i2)
{
  long temp = gcd(i1, i2);
  return ((i1*i2) / temp);
}


/***************************************************
 * return  the dot product of two intvecs a and b  *
 ***************************************************/
static inline long  MivDotProduct(intvec* a, intvec* b)
{
  assume( a->length() ==  b->length());
  int i, n = a->length();
  long result = 0;

  for(i=0; i<n; i++)
    result += (*a)[i] * (*b)[i];

  return result;
}


/**21.10.00*******************************************
 * return the "intvec" lead exponent of a polynomial *
 *****************************************************/
static intvec* MExpPol(poly f)
{
  int nR = currRing->N;

  intvec* result = new intvec(nR);
  int i;

  for(i=0; i<nR; i++)
    (*result)[i] = pGetExp(f,i+1);

  intvec *res = ivCopy(result);
  omFree((ADDRESS) result);
  return res;
}


/***23-24.10.00******************************************
 * compute a division of two monoms, "a" by a monom "b" *
 *    i.e. leading term of two polynoms a and b         *
 ********************************************************/
static poly MpDiv(poly a, poly b)
{
  assume (b != NULL);
  BOOLEAN ok = TRUE;

  if(a == NULL)
    return a;

  int nR = currRing->N;

  number nn = (number) omAllocBin(rnumber_bin);

  poly  ptmp, ppotenz;
  poly result = pISet(1);

  intvec* iva = MExpPol(a);  //head exponent of a
  intvec* ivb = MExpPol(b);  //head exponent of a

  int nab;
  for(int i=0; i<nR; i++)
  {
    nab = (*iva)[i] - (*ivb)[i];
    // b does not divide a
    if(nab < 0)
    {
      result = NULL;
      return result;
    }
    //define a polynomial which is a variable of the basering
    ptmp = (poly) pmInit(currRing->names[i], ok);  //p:=xi
    ppotenz = pPower(ptmp, nab);
    result = pMult(result, ppotenz);
  }
  nn = nDiv(pGetCoeff(a), pGetCoeff(b));
  result = pMult_nn(result, nn);
  nDelete(&nn);

  return result;
}


/***24.10.00 *****************************************
 *      compute a product of two monoms a and b      *
 *      i.e. leading term of two polynoms a and b    *
 *****************************************************/
static poly MpMult(poly a, poly b)
{
  if(a == NULL || b == NULL)
    return a;

  int nR = currRing->N;
  BOOLEAN ok = TRUE;

  poly  ptmp, ppotenz;
  poly result = pISet(1);  // result := 1
  intvec* ivab = ivAdd(MExpPol(a), MExpPol(b));

  for(int i=0; i<nR; i++)
  {
    //define a polynomial which is a variable of the basering
    ptmp = pmInit(currRing->names[i], ok);
    ppotenz = pPower(ptmp, (*ivab)[i]);
    result = pMult(result, ppotenz);
  }
  number nn = nMult(pGetCoeff(a), pGetCoeff(b));
  result = pMult_nn(result, nn);

  return result;
}


poly  MivSame(intvec* u , intvec* v)
{
  assume(u->length() == v->length());

  int i, niv = u->length();

  for (i=0; i<niv; i++)
    if ((*u)[i] != (*v)[i])
      return pISet(1);

  return (poly) NULL;
}

poly  M3ivSame(intvec* temp, intvec* u , intvec* v)
{
  assume(temp->length() == u->length() && u->length() == v->length());

  if(MivSame(temp, u) == NULL)
    return (poly) NULL;
  if(MivSame(temp, v) == NULL)
    return pISet(1);
  return pISet(2);
}


/************************
 *  Define a monom x^iv *
 ************************/
poly MPolVar(intvec* iv)
{
  int i, niv = pVariables;

  poly ptemp = pOne();
  poly pvar, ppotenz;
  BOOLEAN ok = TRUE;

  for(i=0; i<niv; i++)
  {
    pvar = (poly) pmInit(currRing->names[i], ok);  //p:=x_i
    ppotenz = pPower(pvar, (*iv)[i]);
    ptemp = pMult(ptemp, ppotenz);
  }
  return ptemp;
}


/* compute a Groebner basis of an ideal */
ideal Mstd(ideal G)
{
  G = kStd(G, NULL, testHomog, NULL);
  G = kInterRed(G, NULL);//5.02
  idSkipZeroes(G);
  return G;
}

/* compute a Groebner basis of a homogenoues ideal */
ideal Mstdhom(ideal G)
{
  G = kStd(G, NULL, isHomog, NULL);
  G = kInterRed(G, NULL);//21.02
  idSkipZeroes(G);
  return G;
}

/* compute a reduced Groebner basis of a Groebner basis */
ideal MkInterRed(ideal G)
{
  if(G == NULL)
    return G;

  G = kInterRed(G, NULL);
  idSkipZeroes(G);
  return G;
}


/*****************************************************
 *              PERTURBATION WALK                    *
 *****************************************************/

/***************************************
* create an ordering matrix as intvec  *
****************************************/
intvec* MivMatrixOrder(intvec* iv)
{
  int i,j;
  int nR = currRing->N;
  intvec* ivm = new intvec(nR*nR);

  for(i=0; i<nR; i++)
    (*ivm)[i] = (*iv)[i];

  for(i=1; i<nR; i++)
    (*ivm)[i*nR+i-1] = (int) 1;

  return ivm;
}

static intvec* MivMatUnit(void)
{
  int nR = currRing->N;
  intvec* ivm = new intvec(nR);

  (*ivm)[0] = 1;

  return ivm;
}

/* return iv = (1, ..., 1) */
intvec* Mivdp(int nR)
{
  int i;
  intvec* ivm = new intvec(nR);

  for(i=0; i<nR; i++)
    (*ivm)[i] = 1;

  return ivm;
}

/* return iv = (1,0, ..., 0) */
intvec* Mivlp(int nR)
{
  int i;
  intvec* ivm = new intvec(nR);
  (*ivm)[0] = 1;

  return ivm;
}

intvec* Mivdp0(int nR)
{
  int i;
  intvec* ivm = new intvec(nR);
  (*ivm)[nR-1] = 0;
  for(i=0; i<nR-1; i++)
    (*ivm)[i] = 1;

  return ivm;
}

/*****************************************************************************
* If target_ord = intmat(A1, ..., An) then calculate the perturbation        *
* vectors                                                                    *
*   tau_p_dep = inveps^(p_deg-1)*A1 + inveps^(p_deg-2)*A2 +... + A_p_deg     *
* where                                                                      *
*      inveps > totaldegree(G)*(max(A2)+...+max(A_p_deg))                    *
* intmat target_ord is an integer order matrix of the monomial ordering of   *
* basering.                                                                  *
* This programm computes a perturbated vector with a p_deg perturbation      *
* degree which smaller than the numbers of varibles                          *
******************************************************************************/
/* ivtarget is a matrix of a degree reverse lex. order */
intvec* MPertVectors(ideal G, intvec* ivtarget, int pdeg)
{
  int nV = currRing->N;
  //assume(pdeg <= nV && pdeg >= 0);

  int i, j;
  intvec* pert_vector =  new intvec(nV);

  //Checking that the perturbated degree is valid
  if(pdeg > nV || pdeg <= 0)
  {
    WerrorS("The perturbed degree is wrong!!");
    return pert_vector;
  }
  for(i=0; i<nV; i++)
    (*pert_vector)[i]=(*ivtarget)[i];

  if(pdeg == 1)
    return pert_vector;

  // Calculate max1 = Max(A2)+Max(A3)+...+Max(Apdeg),
  // where the Ai are the i-te rows of the matrix target_ord.

  int ntemp, maxAi, maxA=0;
  //for(i=1; i<pdeg; i++)
  for(i=0; i<pdeg; i++) //for "dp"
  {
    maxAi = (*ivtarget)[i*nV];
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp > maxAi)
        maxAi = ntemp;
    }
    maxA += maxAi;
  }

  // Calculate inveps = 1/eps, where 1/eps > totaldeg(p)*max1 for all p in G.
  int inveps, tot_deg = 0, maxdeg;

  intvec* ivUnit = Mivdp(nV);//19.02
  for(i=0; i<IDELEMS(G); i++)
  {
    //maxdeg = pTotaldegree(G->m[i], currRing); //it's wrong for ex1,2,rose
    maxdeg = MwalkWeightDegree(G->m[i], ivUnit);
    if (maxdeg > tot_deg )
      tot_deg = maxdeg;
  }
  inveps = (tot_deg * maxA) + 1;

  // pert(A1) = inveps^(pdeg-1)*A1 + inveps^(pdeg-2)*A2+...+A_pdeg,
  // pert_vector := A1
  for ( i=1; i < pdeg; i++ )
    for(j=0; j<nV; j++)
       (*pert_vector)[j] = inveps*(*pert_vector)[j] + (*ivtarget)[i*nV+j];


  int temp = (*pert_vector)[0];
  for(i=1; i<nV; i++)
  {
    temp = gcd(temp, (*pert_vector)[i]);
    if(temp == 1)
      break;
  }
  if(temp != 1)
    for(i=0; i<nV; i++)
      (*pert_vector)[i] = (*pert_vector)[i] / temp;

  //test_w_in_Cone(G, pert_vector);
  return pert_vector;
}

/* ivtarget is a matrix of the lex. order */
intvec* MPertVectorslp(ideal G, intvec* ivtarget, int pdeg)
{
  int nV = currRing->N;
  //assume(pdeg <= nV && pdeg >= 0);

  int i, j;
  intvec* pert_vector =  new intvec(nV);

  //Checking that the perturbated degree is valid
  if(pdeg > nV || pdeg <= 0)
  {
    WerrorS("The perturbed degree is wrong!!");
    return pert_vector;
  }
  for(i=0; i<nV; i++)
    (*pert_vector)[i]=(*ivtarget)[i];

  if(pdeg == 1)
    return pert_vector;

  // Calculate max1 = Max(A2)+Max(A3)+...+Max(Apdeg),
  // where the Ai are the i-te rows of the matrix target_ord.
  int ntemp, maxAi, maxA=0;
  for(i=1; i<pdeg; i++)
  //for(i=0; i<pdeg; i++) //for "dp"
  {
    maxAi = (*ivtarget)[i*nV];
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp > maxAi)
        maxAi = ntemp;
    }
    maxA += maxAi;
  }

  // Calculate inveps := 1/eps, where 1/eps > deg(p)*max1 for all p in G.
  int inveps, tot_deg = 0, maxdeg;

  intvec* ivUnit = Mivdp(nV);//19.02
  for(i=0; i<IDELEMS(G); i++)
  {
    //maxdeg = pTotaldegree(G->m[i], currRing); //it's wrong for ex1,2,rose
    maxdeg = MwalkWeightDegree(G->m[i], ivUnit);
    if (maxdeg > tot_deg )
      tot_deg = maxdeg;
  }
  inveps = (tot_deg * maxA) + 1;

  // Pert(A1) = inveps^(pdeg-1)*A1 + inveps^(pdeg-2)*A2+...+A_pdeg,

  for ( i=1; i < pdeg; i++ )
    for(j=0; j<nV; j++)
      (*pert_vector)[j] = inveps*((*pert_vector)[j]) + (*ivtarget)[i*nV+j];

  int temp = (*pert_vector)[0];
  for(i=1; i<nV; i++)
  {
    temp = gcd(temp, (*pert_vector)[i]);
    if(temp == 1)
      break;
  }
  if(temp != 1)
    for(i=0; i<nV; i++)
      (*pert_vector)[i] = (*pert_vector)[i] / temp;

  //test_w_in_Cone(G, pert_vector);
  return pert_vector;
}


/***************************************************************
 *                    FRACTAL WALK                             *
 ***************************************************************/

/*****  define a lexicographic order matrix as intvec  ********/
intvec* MivMatrixOrderlp(int nV)
{
  int i;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
    (*ivM)[i*nV + i] = 1;

  return(ivM);
}

intvec* MivMatrixOrderdp(int nV)
{
  int i;
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
    (*ivM)[i] = 1;

  for(i=1; i<nV; i++)
    (*ivM)[(i+1)*nV - i] = -1;

  return(ivM);
}

//creates an intvec of the monomial order Wp(ivstart)
intvec* MivWeightOrderlp(intvec* ivstart)
{
  int i;
  int nV = ivstart->length();
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
    (*ivM)[i] = (*ivstart)[i];

  for(i=1; i<nV; i++)
    (*ivM)[i*nV + i-1] = 1;

  return(ivM);
}

intvec* MivWeightOrderdp(intvec* ivstart)
{
  int i;
  int nV = ivstart->length();
  intvec* ivM = new intvec(nV*nV);

  for(i=0; i<nV; i++)
    (*ivM)[i] = (*ivstart)[i];

  for(i=0; i<nV; i++)
    (*ivM)[nV+i] = 1;

  for(i=2; i<nV; i++)
    (*ivM)[(i+1)*nV - i] = -1;

  return(ivM);
}

intvec* MivUnit(int nV)
{
  int i;
  intvec* ivM = new intvec(nV);

  for(i=0; i<nV; i++)
    (*ivM)[i] = 1;

  return(ivM);
}

/************************************************************************
*  compute a perturbed weight vector of a matrix order w.r.t. an ideal  *
*************************************************************************/
intvec* Mfpertvector(ideal G, intvec* ivtarget)
//intvec* Mfpertvector(ideal G)
{
  int i, j;
  int nV = currRing->N;
  int niv = nV*nV;

  // Calculate max1 = Max(A2) + Max(A3) + ... + Max(AnV),
  // where the Ai are the i-te rows of the matrix 'targer_ord'.
  int ntemp, maxAi, maxA=0;
  for(i=1; i<nV; i++) //30.03
    //for(i=0; i<nV; i++) //for "dp"
  {
    maxAi = (*ivtarget)[i*nV];
    for(j=i*nV+1; j<(i+1)*nV; j++)
    {
      ntemp = (*ivtarget)[j];
      if(ntemp > maxAi)
        maxAi = ntemp;
    }
    maxA = maxA + maxAi;
  }
  intvec* ivUnit = Mivdp(nV);

  // Calculate inveps = 1/eps, where 1/eps > deg(p)*max1 for all p in G.
  int inveps, tot_deg = 0, maxdeg;
  for(i=0; i<IDELEMS(G); i++)
  {
    maxdeg = MwalkWeightDegree(G->m[i], ivUnit);
    //maxdeg = pTotaldegree(G->m[i]);
    if (maxdeg > tot_deg )
      tot_deg = maxdeg;
  }
  inveps = (tot_deg * maxA) + 1;

  // Calculate the perturbed target orders:
  intvec* ivtemp = new intvec(nV);
  intvec* pert_vector = new intvec(niv);
  for(i=0; i<nV; i++)
  {
    ntemp = (*ivtarget)[i];
    (* ivtemp)[i] = ntemp;
    (* pert_vector)[i] = ntemp;
  }
  for(i=1; i<nV; i++)
  {
    for(j=0; j<nV; j++)
      (* ivtemp)[j] = inveps*(*ivtemp)[j] + (*ivtarget)[i*nV+j];
    for(j=0; j<nV; j++)
      (* pert_vector)[i*nV+j] = (* ivtemp)[j];
  }
  omFree((ADDRESS)ivtemp);
  return pert_vector;
}


/**********************************************************************
 *  computes a transformation matrix as an ideal L
    an i-th element of L is a representasion of an i-th element M w.r.t.
    the generators of Gomega
********************************************************************/

ideal MidLift(ideal Gomega, ideal M)
{
  //M = idLift(Gomega, M, NULL, FALSE, FALSE, TRUE, NULL);
  //return M;
  //17.04.01
  ideal Mtmp = idInit(IDELEMS(M),1);
  Mtmp = idLift(Gomega, M, NULL, FALSE, FALSE, TRUE, NULL);
  idSkipZeroes(Mtmp);
  M = idCopy(Mtmp);

  omFree((ADDRESS)Mtmp);
  return M;
}

/****************************************************************
 * Multiplikation of two ideals by elementwise                  *
 * i.e. Let be A := (a_i) and B := (b_i), return C := (a_i*b_i) *
 ****************************************************************/
ideal MidMultLift(ideal A, ideal B)
{
  int mA = IDELEMS(A), mB = IDELEMS(B);
  ideal result;

  if(A==NULL || B==NULL)
    return result;

  if(mB < mA)
  {
    mA = mB;
    result = idInit(mA, 1);
  }
  else
  result = idInit(mA, 1);

  int i, k=0;
  for(i=0; i<mA; i++)
    if(A->m[i] != NULL)
    {
      result->m[k] = pMult(pCopy(A->m[i]), pCopy(B->m[i]));
      k++;
    }

  //idSkipZeroes(result); //walkalp_CON
  ideal res = idCopy(result);
  idDelete(&result);
  return res;
}

//computes a  multiplication of two ideals L and G, ie. L[i]*G[i]
ideal MLiftLmalG(ideal L, ideal G)
{
  int i, j;
  ideal Gtemp = idInit(IDELEMS(L),1);
  ideal mG = idInit(IDELEMS(G),1);
  poly pGtmp = NULL, pmG;
  ideal T;

  for(i=0; i<IDELEMS(L); i++)
  {
    T = idVec2Ideal(L->m[i]);
    mG = MidMultLift(T,G);
    idSkipZeroes(mG);

    for(j=0; j<IDELEMS(mG); j++)
    {
      pGtmp = pAdd(pGtmp, mG->m[j]);
    }
    Gtemp->m[i] = pCopy(pGtmp);
  }
  idSkipZeroes(Gtemp);

  //compute a reduced Groebner basis of GF
  //Gtemp = kInterRed(Gtemp, NULL);
  L = idCopy(Gtemp);

  omFree((ADDRESS)mG);
  omFree((ADDRESS)Gtemp);
  return L;
}

/*********************************************************************
 * G is a red. Groebner basis w.r.t. <_1                             *
 * Gomega is an initial form ideal of <G> w.r.t. a weight vector w   *
 * M is a subideal of <Gomega> and M selft is a red. Groebner basis  *
 *    of the ideal <Gomega> w.r.t. <_w                               *
 * Let m_i = h1.gw1 + ... + hs.gws for each m_i in M; gwi in Gomega  *
 * return F with n(F) = n(M) and f_i = h1.g1 + ... + hs.gs for each i*
 ********************************************************************/
 /* MidLift + MLiftLmalG */
ideal MLiftLmalGNew(ideal Gomega, ideal M, ideal G)
{
  int i, j;
  M = idLift(Gomega, M, NULL, FALSE, FALSE, TRUE, NULL);
  int nM = IDELEMS(M);
  ideal Gtemp = idInit(IDELEMS(M),1);
  ideal mG = idInit(IDELEMS(G),1);
  poly pmG, pGtmp = NULL;
  ideal T;

  for(i=0; i<nM; i++)
  {
    T = idVec2Ideal(M->m[i]);
    mG = MidMultLift(T, G);

    for(j=0; j<IDELEMS(mG); j++)
      pGtmp = pAdd(pGtmp, mG->m[j]);

    Gtemp->m[i] = pCopy(pGtmp);
  }
  idSkipZeroes(Gtemp);

  M = idCopy(Gtemp);

  omFree((ADDRESS)mG);
  omFree((ADDRESS)Gtemp);
  return M;
}

/******************************************************************************
 * compute a next weight vector on the line from curr_weight to target_weight *
 * and it still stays in the cone of the ideal G where the curr_weight too    *
 *****************************************************************************/
intvec* MwalkNextWeight(intvec* curr_weight, intvec* target_weight, ideal G)
{
  assume(currRing != NULL && curr_weight != NULL &&
         target_weight != NULL && G != NULL);

  int nRing = currRing->N;
  int nG = IDELEMS(G);
  intvec* ivtemp;

  long t_zaehler = 0, t_nenner = 0;
  long s_zaehler, s_nenner, temp, MwWd;
  long deg_w0_p1, deg_d0_p1;
  int j;

  intvec* diff_weight = ivSub(target_weight, curr_weight);
  poly g;
  for (j=0; j<nG; j++)
  {
    g = G->m[j];
    if (g != NULL)
    {
      ivtemp = MExpPol(g);
      deg_w0_p1 = MivDotProduct(ivtemp, curr_weight);
      deg_d0_p1 = MivDotProduct(ivtemp, diff_weight);

      pIter(g);

      while (g != NULL)
      {
        //s_zaehler = deg_w0_p1 - pGetOrder(g);
	ivtemp = MExpPol(g);
	MwWd = MivDotProduct(ivtemp, curr_weight);
	s_zaehler = deg_w0_p1 - MwWd;

        if (s_zaehler != 0)
        {
          //s_nenner = MwalkWeightDegree(g, diff_weight) - deg_d0_p1;
	  MwWd = MivDotProduct(ivtemp, diff_weight);
          s_nenner = MwWd - deg_d0_p1;

          // check for 0 < s <= 1
          if ( (s_zaehler > 0 && s_nenner >= s_zaehler) ||
               (s_zaehler < 0 && s_nenner <= s_zaehler) )
          {
            // make both positive
            if (s_zaehler < 0)
            {
              s_zaehler = - s_zaehler;
              s_nenner = - s_nenner;
            }

            // compute a simply fraction of s
            cancel(s_zaehler, s_nenner);

            if(t_nenner != 0)
            {
              if(s_zaehler * t_nenner < s_nenner * t_zaehler)
              {
                t_nenner = s_nenner;
                t_zaehler = s_zaehler;
              }	
            }
            else
            {
              t_nenner = s_nenner;
              t_zaehler = s_zaehler;
            }
          }
        }
        pIter(g); //g = g - pHead(g);
      }
    }
  }
  if(t_nenner == 0)
  {
    diff_weight = ivCopy(curr_weight);
    return diff_weight;
  }

  if(t_nenner == 1 && t_zaehler == 1)
  {
    diff_weight = ivCopy(target_weight);
    return diff_weight;
  }

  // construct a new weight vector
  for (j=0; j<nRing; j++)
  {
    (*diff_weight)[j] = t_nenner*(*curr_weight)[j] +
      t_zaehler*(*diff_weight)[j];
  }

  // and take out the content
  temp = (*diff_weight)[0];
  if(temp != 1)
    for (j=1; j<nRing; j++)
    {
      temp = gcd(temp, (*diff_weight)[j]);
      if (temp == 1)
	return diff_weight;
    }

  for (j=0; j<nRing; j++)
      (*diff_weight)[j] = (*diff_weight)[j] / temp;

  return diff_weight;
}

/* Condition: poly f must be divided by the ideal G */
/* Let f = h1g1+...+hsgs, then the result is (h1, ... ,hs) */
static ideal MNormalForm(poly f, ideal G)
{
  int nG = IDELEMS(G);
  ideal lmG = idInit(nG, 1);
  ideal result = idInit(nG, 1);
  int i, ind=0, ncheck=0;

  for(i=0; i<nG; i++)
  {
    lmG->m[i] = pHead(G->m[i]);
    result->m[i] = NULL;
  }

  poly h = f;
  poly lmh, q, pmax = pISet(1), quot, qtmp=NULL;
  int ntest = 0;
  while(h != NULL)
  {
    lmh = pHead(h);
    for(i=0; i<nG; i++)
    {
      q = MpDiv(lmh, lmG->m[i]);

      if(q != NULL)
      {
	  if(ncheck == 0)
	  {
	    result->m[i] = pCopy(pAdd(result->m[i], q));
	    h = pSub(h, pMult(q, pCopy(G->m[i])));
	    break;
	  }
	  else {
	    h = pSub(f, pMult(q, pCopy(G->m[i])));
	    if(quot != NULL)
	    {
	      ntest = 1;
	      qtmp = q;
	      ind = i;
	      ncheck = 0;
	    }
	  }
      }
    }
    if(i==nG)
    {
      f = h;
      pIter(h);
      ncheck = 1;
    }

    if(ntest == 1)
    {
      result->m[ind] = pCopy(pAdd(result->m[ind], qtmp));
      ntest = 0;
    }
  }
  ideal rest = idCopy(result);
  idDelete(&result);
  idDelete(&lmG);
  return rest;
}

/* GW is an initial form of the ideal G w.r.t. a weight vector */
/* polynom f is divided by the ideal GW                        */
/* Let f := h_1.gw_1 + ... + h_s.gw_s, then the result is      */
/*          h_1.g_1 + ... + h_s.g_s                            */
static poly MpolyConversion(poly f, ideal GW, ideal G)
{
  ideal H = MNormalForm(f, GW);
  ideal HG = MidMultLift(H, G);

  poly result = NULL;
  int i;
  int nG = IDELEMS(G);

  for(i=0; i<nG; i++)
   result = pCopy(pAdd(result, HG->m[i]));

  return result;
}

/* GW is an initial form of the ideal G w.r.t. a weight vector */
/* Each polynom f of the ideal M is divided by the ideal GW    */
/* Let m_i := h_1.gw_1 + ... + h_s.gw_s, then the i-th polynom */
/* of result is  f_i := h_1.g_1 + ... + h_s.g_s                */
ideal MidealConversion(ideal M, ideal GW, ideal G)
{
  int nM = IDELEMS(M);
  int i;

  for(i=0; i<nM; i++)
  {
    M->m[i] = MpolyConversion(M->m[i], GW, G);
  }
  ideal result = idCopy(M);
  return result;
}

/* check that the monomial f is reduced by a monomial ideal G */
static inline int MCheckpRedId(poly f, ideal G)
{
  int nG = IDELEMS(G);
  int i;
  poly q;

  for(i=0; i<nG; i++)
  {
    q = MpDiv(f, G->m[i]);
    if(q != NULL)
      return 0;
  }
  return 1;
}

poly MpReduceId(poly f, ideal GO)
{
  ideal G = idCopy(GO);
  int nG = IDELEMS(G);
  int i, pcheck;
  ideal HG = idInit(nG, 1);

  for(i=0; i<nG; i++)
    HG->m[i] = pHead(G->m[i]);

  poly h = f;
  poly lmh, q,qg, result = NULL;

  while(h!=NULL)
  {
    f = pCopy( h);
    lmh = pHead(h);

    if(MCheckpRedId(lmh, HG) != 0)
    {
      result = pCopy(pAdd(result, lmh));
      pIter(h);
    }
    else
      for(i=0; i<nG; i++)
      {
	q = MpDiv(lmh, HG->m[i]);
	if(q != NULL)
	{
	  f = pAdd(result, f);
	  qg = pMult(q, G->m[i]);
	  h = pSub(f, qg);
	  result = NULL;

	  lmh = pHead(h);
	  pcheck = MCheckpRedId(lmh, HG);
	  if(pcheck != 0)
	  {
	    break;
	  }
	}
      }
 }
 idDelete(&HG);
 return result;
}

/* return f, if a head term of f is not divided by a head ideal M */
static poly MpMinimId(poly f, ideal M)
{
  int nM = IDELEMS(M);
  ideal HM = idInit(nM, 1);
  int i, pcheck;

  for(i=0; i<nM; i++)
    HM->m[i] = pCopy(M->m[i]);

  poly result = pCopy(f);
  poly hf=pHead(f), q, qtmp, h=f;

  if(MCheckpRedId(pHead(f), HM) != 0)
    goto FINISH;

  while(1)
  {
    for(i=0; i<nM; i++)
    {
      q = MpDiv(hf, HM->m[i]);
      if(q != NULL)
	{
	  qtmp = pMult(q, M->m[i]);
	  h = pSub(h, qtmp);

	  hf = pHead(h);
	  pcheck = MCheckpRedId(hf, HM);
	  if(pcheck != 0)
	  {
	    result = pCopy(h);
	    goto FINISH;	
	  }
	  break;
	}
    }
 }

 FINISH:
 idDelete(&HM);
 return result;
}

/* return a minimal ideal of an ideal M */
ideal MidMinimId(ideal M)
{
  int i,j=0;
  ideal result = idInit(IDELEMS(M),1);
  poly pmin;
  for(i=0; i<IDELEMS(M); i++)
  {
    ideal Mtmp = idCopy(M);
    Mtmp->m[j] = NULL;
    idSkipZeroes(Mtmp);
    pmin = MpMinimId(pCopy(M->m[i]), Mtmp);
    M->m[i] = pCopy(pmin);
    result->m[j] = pmin;
    if(pmin == NULL)
    {
      i--;
      j--;
      idSkipZeroes(M);
    }
    j++;
    idDelete(&Mtmp);
  }
  idSkipZeroes(result);
  ideal res = idCopy(result);
  idDelete(&result);
  return res;
}


ideal MidMinBase(ideal G)
{
  if(G == NULL)
    return G;

    intvec * wth;
    lists re = min_std(G,currQuotient,(tHomog)TRUE,&wth,NULL,0,3);
    G = (ideal)re->m[1].data;
    re->m[1].data = NULL;
    re->m[1].rtyp = NONE;
    re->Clean();

  return G;
}


/* compute a Groebner basis of a homogenoues ideal */
ideal MNWstdhomRed(ideal G, intvec* iv)
{

  ideal Gomega = MwalkInitialForm(G, iv);
  G = kStd(Gomega, NULL, isHomog, NULL);
  Gomega = MkInterRed(G);

  return Gomega;
}

/*****************************************************************************
* If target_ord = intmat(A1,..., An) then calculate the perturbation vectors *
*     tau_p_dep = inveps^(p_deg-1)*A1 + inveps^(p_deg-2)*A2 +... + A_p_deg   *
* where                                                                      *
*     inveps > totaldegree(G)*(max(A2)+...+max(A_p_deg))                     *
* and                                                                        *
*     p_deg <= the number of variables of a basering                         *
******************************************************************************/
intvec* Mfivpert(ideal G, intvec* target, int p_deg)
{
  int i, j;
  int nV = currRing->N;

  //Checking that the perturbation degree is valid
  if(p_deg > nV || p_deg <= 0)
  {
    WerrorS("The perturbed degree is wrong!!");
    return NULL;
  }

  // Calculate max_el_rows = Max(A2)+Max(A3)+...+Max(Ap_deg),
  //    where the Ai are the rows of the target-order matrix.
  int nmax = 0, maxAi, ntemp;

  for(i=1; i < p_deg; i++)
  {
    maxAi = (*target)[i*nV];
    for(j=1; j < nV; j++)
    {
      ntemp = (*target)[i*nV + j];
      if(ntemp > maxAi)
        maxAi = ntemp;
    }
    nmax += maxAi;
  }

  // Calculate inv_eps := 1/eps, where 1/eps > deg(p)*max_el_rows
  //        for all p in G.
  int inv_eps, degG, max_deg=0;
  intvec* ivUnit = Mivdp(nV);

  for (i=0; i<IDELEMS(G); i++)
  {
    degG = MwalkWeightDegree(G->m[i], ivUnit);
    if(degG > max_deg)
      max_deg = degG;
  }
  inv_eps = (max_deg * nmax) + 1;


  // Calculate the perturbed target order:
  // Since a weight vector in Singular has to be in integral, we compute
  // tau_p_deg = inv_eps^(p_deg-1)*A1 - inv_eps^(p_deg-2)*A2+...+A_p_deg,

  intvec* ivtemp = new intvec(nV);
  intvec* pert_vector = new intvec(nV);

  for(i=0; i<nV; i++)
  {
    ntemp = (*target)[i];
    (* ivtemp)[i] = ntemp;
    (* pert_vector)[i] = ntemp;
  }

  for(i=1; i<p_deg; i++)
  {
    for(j=0; j<nV; j++)
      (* ivtemp)[j] = inv_eps*(*ivtemp)[j] + (*target)[i*nV+j];

    pert_vector = ivtemp;
  }
  omFree((ADDRESS) ivtemp);
  return pert_vector;
}

ideal MpHeadIdeal(ideal G)
{
  int i, nG = IDELEMS(G);
  ideal result = idInit(nG,1);

  for(i=0; i<nG; i++)
  {
    result->m[i] = pHead(G->m[i]);
  }

  ideal res = idCopy(result);
  idDelete(&result);
  return res;
}

void* checkideal(ideal G)
{
  int i;
  printf("//** Size(G)= %d, and ", IDELEMS(G));

  for(i=0; i<IDELEMS(G); i++)
  {
    printf("G[%d] = %d, ", i, pLength(G->m[i]));
  }
  printf("\n");
}

