#include <kernel/polys.h>
#include <kernel/kstd1.h>
#include <libpolys/coeffs/longrat.h>
#include <libpolys/polys/clapsing.h>
#include <bbcone.h>

#include <initialReduction.h>

poly initial(poly p)
{
  poly g = p;
  poly h = p_Head(g,currRing);
  poly f = h;
  long d = p_Deg(g,currRing);
  pIter(g);
  while ((g != NULL) && (p_Deg(g,currRing) == d))
  {
    pNext(h) = p_Head(g,currRing);
    pIter(h);
    pIter(g);
  }
  return(f);
}


BOOLEAN initial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      poly p = (poly) u->Data();
      res->rtyp = POLY_CMD;
      res->data = (void*) initial(p);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      ideal inI = idInit(IDELEMS(I));
      for (int i=0; i<IDELEMS(I); i++)
        inI->m[i]=initial(I->m[i]);
      res->rtyp = IDEAL_CMD;
      res->data = (void*) inI;
      return FALSE;
    }
  }
  WerrorS("initial: unexpected parameters");
  return TRUE;
}


BOOLEAN homogeneitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        while (g != NULL)
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(gfan::ZMatrix(0, equations.getWidth()),equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("homogeneitySpace: unexpected parameters");
  return TRUE;
}


BOOLEAN groebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g = NULL;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      long d;
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        d = p_Deg(g,currRing);
        while ((g != NULL) && (p_Deg(g,currRing) == d))
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }

        if (g != NULL)
        {
          while (g != NULL)
          {
            pGetExpV(g,tailexpv);
            tailexpw = intStar2ZVector(n, tailexpv);
            inequalities.appendRow(leadexpw-tailexpw);
            pIter(g);
          }
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(inequalities,equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("groebnerCone: unexpected parameters");
  return TRUE;
}


gfan::ZCone* maximalGroebnerCone(const ring &r, const ideal &I)
{
  int n = rVar(r);
  poly g = NULL;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i]; pGetExpV(g,leadexpv);
    leadexpw = intStar2ZVector(n, leadexpv);
    pIter(g);
    while (g != NULL)
    {
      pGetExpV(g,tailexpv);
      tailexpw = intStar2ZVector(n, tailexpv);
      inequalities.appendRow(leadexpw-tailexpw);
      pIter(g);
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return new gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
}


BOOLEAN maximalGroebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      res->rtyp = coneID;
      res->data = (void*) maximalGroebnerCone(currRing, I);
      return FALSE;
    }
  }
  WerrorS("maximalGroebnerCone: unexpected parameters");
  return TRUE;
}

/***
 * If 1, replaces all occuring t with prime p,
 * where theoretically feasable.
 * Also computes GCD over integers than
 * over univariate polynomials
 **/
#define T_TO_P 0

/***
 * Suppose I=g_0,...,g_{n-1}.
 * This function uses bubble sort to sorts g_1,...,g_{n-1}
 * such that lm(g_1)>...>lm(g_{n-1})
 **/
static inline void sortingLaterGenerators(ideal I)
{
  poly cache; int i; int n=IDELEMS(I); int newn;
  do
  {
    newn=0;
    for (i=2; i<n; i++)
    {
      if (pLmCmp(I->m[i-1],I->m[i])<0)
      {
        cache=I->m[i-1];
        I->m[i-1]=I->m[i];
        I->m[i]=cache;
        newn = i;
      }
    }
    n=newn;
  } while(n);
}


// /***
//  * replaces coefficients in g of lowest degree in t
//  * divisible by p with a suitable power of t
//  **/
// static bool preduce(poly g, const number p)
// {
//   // go along g and look for relevant terms.
//   // monomials in x which have been checked are tracked in done.
//   // because we assume the leading coefficient of g is 1,
//   // the leading term does not need to be considered.
//   poly done = p_LmInit(g,currRing);
//   p_SetExp(done,1,0,currRing); p_SetCoeff(done,n_Init(1,currRing->cf),currRing);
//   p_Setm(done,currRing);
//   poly doneEnd = done;
//   poly doneCache;
//   number dnumber; long d;
//   poly subst; number ppower;
//   while(pNext(g))
//   {
//     // check whether next term needs to be reduced:
//     // first, check whether monomial in x has come up yet
//     for (doneCache=done; doneCache; pIter(doneCache))
//     {
//       if (p_LmDivisibleBy(doneCache,pNext(g),currRing))
//         break;
//     }
//     if (!doneCache) // if for loop did not terminate prematurely,
//                     // then the monomial in x is new
//     {
//       // second, check whether coefficient is divisible by p
//       if (n_DivBy(p_GetCoeff(pNext(g),currRing->cf),p,currRing->cf))
//       {
//         // reduce the term with respect to p-t:
//         // divide coefficient by p, remove old term,
//         // add t multiple of old term
//         dnumber = n_Div(p_GetCoeff(pNext(g),currRing->cf),p,currRing->cf);
//         d = n_Int(dnumber,currRing->cf);
//         n_Delete(&dnumber,currRing->cf);
//         if (!d)
//         {
//           p_Delete(&done,currRing);
//           WerrorS("initialReduction: overflow in a t-exponent");
//           return true;
//         }
//         subst=p_LmInit(pNext(g),currRing);
//         if (d>0)
//         {
//           p_AddExp(subst,1,d,currRing);
//           p_SetCoeff(subst,n_Init(1,currRing->cf),currRing);
//         }
//         else
//         {
//           p_AddExp(subst,1,-d,currRing);
//           p_SetCoeff(subst,n_Init(-1,currRing->cf),currRing);
//         }
//         p_Setm(subst,currRing); pTest(subst);
//         pNext(g)=p_LmDeleteAndNext(pNext(g),currRing);
//         pNext(g)=p_Add_q(pNext(g),subst,currRing);
//         pTest(pNext(g));
//       }
//       // either way, add monomial in x to done
//       pNext(doneEnd)=p_LmInit(pNext(g),currRing);
//       pIter(doneEnd);
//       p_SetExp(doneEnd,1,0,currRing);
//       p_SetCoeff(doneEnd,n_Init(1,currRing->cf),currRing);
//       p_Setm(doneEnd,currRing);
//     }
//     pIter(g);
//   }
//   p_Delete(&done,currRing);
//   return false;
// }


// #ifndef NDEBUG
// BOOLEAN preduce(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == POLY_CMD))
//   {
//     poly g; bool b;
//     number p = n_Init(3,currRing->cf);
//     omUpdateInfo();
//     Print("usedBytes=%ld\n",om_Info.UsedBytes);
//     g = (poly) u->CopyD();
//     b = preduce(g,p);
//     p_Delete(&g,currRing);
//     if (b) return TRUE;
//     omUpdateInfo();
//     Print("usedBytes=%ld\n",om_Info.UsedBytes);
//     n_Delete(&p,currRing->cf);
//     res->rtyp = NONE;
//     res->data = NULL;
//     return FALSE;
//   }
//   return TRUE;
// }
// #endif //NDEBUG


// /***
//  * Returns the highest term in g with the matching x-monomial to m
//  * or, if it does not exist, the NULL pointer
//  **/
// static poly highestMatchingX(poly g, const poly m)
// {
//   pTest(g); pTest(m);
//   poly xalpha=p_LmInit(m,currRing);

//   // go along g and find the first term
//   // with the same monomial in x as xalpha
//   while (g)
//   {
//     p_SetExp(xalpha,1,p_GetExp(g,1,currRing),currRing);
//     p_Setm(xalpha,currRing);
//     if (p_LmEqual(g,xalpha,currRing))
//       break;
//     pIter(g);
//   }

//   // gCache now either points at the wanted term
//   // or is NULL
//   p_Delete(&xalpha,currRing);
//   pTest(g);
//   return g;
// }


// /***
//  * Given g with lm(g)=t^\beta x^\alpha, returns g_\alpha
//  **/
// static poly powerSeriesCoeff(poly g)
// {
//   // the first term obviously is part of our output
//   // so we copy it...
//   poly xalpha=p_LmInit(g,currRing);
//   poly coeff=p_One(currRing);
//   p_SetCoeff(coeff,n_Copy(p_GetCoeff(g,currRing),currRing->cf),currRing);
//   p_SetExp(coeff,1,p_GetExp(g,1,currRing),currRing);
//   p_Setm(coeff,currRing);

//   // ..and proceed with the remaining terms,
//   // appending the relevant terms to coeff via coeffCache
//   poly coeffCache=coeff;
//   pIter(g);
//   while (g)
//   {
//     p_SetExp(xalpha,1,p_GetExp(g,1,currRing),currRing);
//     p_Setm(xalpha,currRing);
//     if (p_LmEqual(g,xalpha,currRing))
//     {
//       pNext(coeffCache)=p_Init(currRing);
//       pIter(coeffCache);
//       p_SetCoeff(coeffCache,n_Copy(p_GetCoeff(g,currRing),currRing->cf),currRing);
//       p_SetExp(coeffCache,1,p_GetExp(g,1,currRing),currRing);
//       p_Setm(coeffCache,currRing);
//     }
//     pIter(g);
//   }

//   p_Delete(&xalpha,currRing);
//   return coeff;
// }


// /***
//  * divides g by t^b knowing that each term of g
//  * is divisible by t^b, i.e. no divisibility checks
//  * needed
//  **/
// static void divideByT(poly g, const long b)
// {
//   while (g)
//   {
//     p_SetExp(g,1,p_GetExp(g,1,currRing)-b,currRing);
//     p_Setm(g,currRing);
//     pIter(g);
//   }
// }


// static void divideByGcd(poly &g)
// {
//   // first determine all g_\alpha
//   // as alpha runs over all exponent vectors
//   // of monomials in x occuring in g

//   // gAlpha will store all g_\alpha,
//   // the first term will, for comparison purposes for now,
//   // also keep their monomial in x.
//   // we assume that the weight on the x are positive
//   // so that the x won't make the monomial smaller
//   ideal gAlphaFront = idInit(pLength(g));
//   gAlphaFront->m[0] = p_Head(g,currRing);
//   p_SetExp(gAlphaFront->m[0],1,0,currRing);
//   p_Setm(gAlphaFront->m[0],currRing);
//   ideal gAlphaEnd = idInit(pLength(g));
//   gAlphaEnd->m[0] = gAlphaFront->m[0];
//   unsigned long gAlpha_sev[pLength(g)];
//   gAlpha_sev[0] = p_GetShortExpVector(g,currRing);
//   long beta[pLength(g)];
//   beta[0] = p_GetExp(g,1,currRing);
//   int count=0;

//   poly current = pNext(g); unsigned long current_not_sev;
//   int i;
//   while (current)
//   {
//     // see whether the monomial in x of current already came up
//     // since everything is homogeneous in x and the ordering is local in t,
//     // this comes down to a divisibility test in two stages
//     current_not_sev = ~p_GetShortExpVector(current,currRing);
//     for(i=0; i<=count; i++)
//     {
//       // first stage using short exponent vectors
//       // second stage a proper test
//       if (p_LmShortDivisibleBy(gAlphaFront->m[i],gAlpha_sev[i],current,current_not_sev, currRing)
//             && p_LmDivisibleBy(gAlphaFront->m[i],current,currRing))
//       {
//         // if it already occured, add the term to the respective entry
//         // without the x part
//         pNext(gAlphaEnd->m[i])=p_Init(currRing);
//         pIter(gAlphaEnd->m[i]);
//         p_SetExp(gAlphaEnd->m[i],1,p_GetExp(current,1,currRing)-beta[i],currRing);
//         p_SetCoeff(gAlphaEnd->m[i],n_Copy(p_GetCoeff(current,currRing),currRing->cf),currRing);
//         p_Setm(gAlphaEnd->m[i],currRing);
//         break;
//       }
//     }
//     if (i>count)
//     {
//       // if it is new, create a new entry for the term
//       // including the monomial in x
//       count++;
//       gAlphaFront->m[count] = p_Head(current,currRing);
//       beta[count] = p_GetExp(current,1,currRing);
//       gAlphaEnd->m[count] = gAlphaFront->m[count];
//       gAlpha_sev[count] = p_GetShortExpVector(current,currRing);
//     }

//     pIter(current);
//   }

//   if (count)
//   {
//     // now remove all the x in the leading terms
//     // so that gAlpha only contais terms in t
//     int j; long d;
//     for (i=0; i<=count; i++)
//     {
//       for (j=2; j<=currRing->N; j++)
//         p_SetExp(gAlphaFront->m[i],j,0,currRing);
//       p_Setm(gAlphaFront->m[i],currRing);
//       gAlphaEnd->m[i]=NULL;
//     }

//     // now compute the gcd over all g_\alpha
//     // and set the input to null as they are deleted in the process
//     poly gcd = singclap_gcd(gAlphaFront->m[0],gAlphaFront->m[1],currRing);
//     gAlphaFront->m[0] = NULL;
//     gAlphaFront->m[1] = NULL;
//     for(i=2; i<=count; i++)
//     {
//       gcd = singclap_gcd(gcd,gAlphaFront->m[i],currRing);
//       gAlphaFront->m[i] = NULL;
//     }
//     // divide g by the gcd
//     poly h = singclap_pdivide(g,gcd,currRing);
//     p_Delete(&gcd,currRing);
//     p_Delete(&g,currRing);
//     g = h;

//     id_Delete(&gAlphaFront,currRing);
//     id_Delete(&gAlphaEnd,currRing);
//   }
//   else
//   {
//     // if g contains only one monomial in x,
//     // then we can get rid of all the higher t
//     p_Delete(&g,currRing);
//     g = gAlphaFront->m[0];
//     pIter(gAlphaFront->m[0]);
//     pNext(g)=NULL;
//     gAlphaEnd->m[0] = NULL;
//     id_Delete(&gAlphaFront,currRing);
//     id_Delete(&gAlphaEnd,currRing);
//   }
// }


// /***
//  * 1. For each \alpha\in\NN^n, changes (c_\beta t^\beta + c_{\beta+1} t^{\beta+1} + ...)
//  *    to (c_\beta + c_{\beta+1}*p + ...) t^\beta
//  * 2. Computes the gcd over all (c_\beta + c_{\beta+1}*p + ...) and divides g by it
//  **/
// static void simplify(poly g, const number p)
// {
//   // go along g and look for relevant terms.
//   // monomials in x which have been checked are tracked in done.
//   poly done = p_LmInit(g,currRing);
//   p_SetCoeff(done,n_Init(1,currRing->cf),currRing);
//   p_Setm(done,currRing);
//   poly doneEnd = done;
//   poly doneCurrent;

//   poly subst; number substCoeff, substCoeffCache;
//   unsigned long d;

//   poly gCurrent = g;
//   while(pNext(gCurrent))
//   {
//     // check whether next term needs to be reduced:
//     // first, check whether monomial in x has come up yet
//     for (doneCurrent=done; doneCurrent; pIter(doneCurrent))
//     {
//       if (p_LmDivisibleBy(doneCurrent,pNext(gCurrent),currRing))
//         break;
//     }
//     // if the monomial in x already occured, then we want to replace
//     // as many t with p as theoretically feasable
//     if (doneCurrent)
//     {
//       // suppose pNext(gCurrent)=3*t5x and doneCurrent=t3x
//       // then we want to replace pNext(gCurrent) with 3p2*t3x
//       // subst = ?*t3x
//       subst = p_LmInit(doneCurrent,currRing);
//       // substcoeff = p2
//       n_Power(p,p_GetExp(subst,1,currRing)-p_GetExp(doneCurrent,1,currRing),&substCoeff,currRing->cf);
//       // substcoeff = 3p2
//       n_InpMult(substCoeff,p_GetCoeff(pNext(gCurrent),currRing),currRing->cf);
//       // subst = 3p2*t3x
//       p_SetCoeff(subst,substCoeff,currRing);
//       p_Setm(subst,currRing); pTest(subst);

//       // g = g - pNext(gCurrent) + subst
//       pNext(gCurrent)=p_LmDeleteAndNext(pNext(gCurrent),currRing);
//       g=p_Add_q(g,subst,currRing);
//       pTest(pNext(gbeginning));
//     }
//     else
//     {
//       // if the monomial in x is brand new,
//       // then we check whether the coefficient is divisible by p
//       if (n_DivBy(p_GetCoeff(pNext(gCurrent),currRing->cf),p,currRing->cf))
//       {
//         // reduce the term with respect to p-t:
//         // suppose pNext(gCurrent)=4p3*tx
//         // then we want to replace it with 4*t4x
//         // divide 4p3 repeatedly by p until it is not divisible anymore,
//         // keeping track on the final value 4
//         // and the number of times it has been divided 3
//         substCoeff = n_Div(p_GetCoeff(pNext(gCurrent),currRing->cf),p,currRing->cf);
//         d = 1;
//         while (n_DivBy(substCoeff,p,currRing->cf))
//         {
//           substCoeffCache = n_Div(p_GetCoeff(pNext(gCurrent),currRing->cf),p,currRing->cf);
//           n_Delete(&substCoeff,currRing->cf);
//           substCoeff = substCoeffCache;
//           d++;
//           assume(d>0); // check for overflow
//         }

//         // subst = ?*tx
//         subst=p_LmInit(pNext(gCurrent),currRing);
//         // subst = ?*t4x
//         p_AddExp(subst,1,d,currRing);
//         // subst = 4*t4x
//         p_SetCoeff(subst,substCoeffCache,currRing);
//         p_Setm(subst,currRing); pTest(subst);

//         // g = g - pNext(gCurrent) + subst
//         pNext(gCurrent)=p_LmDeleteAndNext(pNext(gCurrent),currRing);
//         pNext(gCurrent)=p_Add_q(pNext(gCurrent),subst,currRing);
//         pTest(pNext(gCurrent));
//       }

//       // either way, add monomial in x with minimal t to done
//       pNext(doneEnd)=p_LmInit(pNext(gCurrent),currRing);
//       pIter(doneEnd);
//       p_SetCoeff(doneEnd,n_Init(1,currRing->cf),currRing);
//       p_Setm(doneEnd,currRing);
//     }
//     pIter(gCurrent);
//   }
//   p_Delete(&done,currRing);
// }


// #ifndef NDEBUG
// BOOLEAN divideByGcd(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == POLY_CMD))
//   {
//     poly g;
//     omUpdateInfo();
//     Print("usedBytes1=%ld\n",om_Info.UsedBytes);
//     g = (poly) u->CopyD();
//     divideByGcd(g);
//     p_Delete(&g,currRing);
//     omUpdateInfo();
//     Print("usedBytes1=%ld\n",om_Info.UsedBytes);
//     res->rtyp = NONE;
//     res->data = NULL;
//     return FALSE;
//   }
//   return TRUE;
// }
// #endif //NDEBUG


// BOOLEAN initialReduction(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == IDEAL_CMD))
//   {
//     leftv v = u->next;
//     if (v == NULL)
//     {
//       ideal I = (ideal) u->Data();

//       /***
//        * Suppose I=g_0,...,g_n and g_0=p-t.
//        * Step 1: sort elements g_1,...,g_{n-1} such that lm(g_1)>...>lm(g_{n-1})
//        * (the following algorithm is a bubble sort)
//        **/
//       sortingLaterGenerators(I);

//       /***
//        * Step 2: replace coefficient of terms of lowest t-degree divisible by p with t
//        **/
//       number p = p_GetCoeff(I->m[0],currRing);
//       for (int i=1; i<IDELEMS(I); i++)
//       {
//         if (preduce(I->m[i],p))
//           return TRUE;
//       }

//       /***
//        * Step 3: the first pass. removing terms with the same monomials in x as lt(g_i)
//        * out of g_j for i<j
//        **/
//       int i,j;
//       poly uniti, unitj;
//       for (i=1; i<IDELEMS(I)-1; i++)
//       {
//         for (j=i+1; j<IDELEMS(I); j++)
//         {
//           unitj = highestMatchingX(I->m[j],I->m[i]);
//           if (unitj)
//           {
//             unitj = powerSeriesCoeff(unitj);
//             divideByT(unitj,p_GetExp(I->m[i],1,currRing));
//             uniti = powerSeriesCoeff(I->m[i]);
//             divideByT(uniti,p_GetExp(I->m[i],1,currRing));
//             pTest(unitj); pTest(uniti); pTest(I->m[j]); pTest(I->m[i]);
//             I->m[j]=p_Add_q(p_Mult_q(uniti,I->m[j],currRing),
//                             p_Neg(p_Mult_q(unitj,p_Copy(I->m[i],currRing),currRing),currRing),
//                             currRing);
//             divideByGcd(I->m[j]);
//           }
//         }
//       }
//       for (int i=1; i<IDELEMS(I); i++)
//       {
//         if (preduce(I->m[i],p))
//           return TRUE;
//       }

//       /***
//        * Step 4: the second pass. removing terms divisible by lt(g_j) out of g_i for i<j
//        **/
//       for (i=1; i<IDELEMS(I)-1; i++)
//       {
//         for (j=i+1; j<IDELEMS(I); j++)
//         {
//           uniti = highestMatchingX(I->m[i],I->m[j]);
//           if (uniti && p_GetExp(uniti,1,currRing)>=p_GetExp(I->m[j],1,currRing))
//           {
//             uniti = powerSeriesCoeff(uniti);
//             divideByT(uniti,p_GetExp(I->m[j],1,currRing));
//             unitj = powerSeriesCoeff(I->m[j]);
//             divideByT(unitj,p_GetExp(I->m[j],1,currRing));
//             I->m[i] = p_Add_q(p_Mult_q(unitj,I->m[i],currRing),
//                               p_Neg(p_Mult_q(uniti,p_Copy(I->m[j],currRing),currRing),currRing),
//                               currRing);
//             divideByGcd(I->m[j]);
//           }
//         }
//       }
//       for (int i=1; i<IDELEMS(I); i++)
//       {
//         if (preduce(I->m[i],p))
//           return TRUE;
//       }

//       res->rtyp = NONE;
//       res->data = NULL;
//       IDDATA((idhdl)u->data) = (char*) I;
//       return FALSE;
//     }
//   }
//   WerrorS("initialReduction: unexpected parameters");
//   return TRUE;
// }


#if 0
/***
 * Given a general ring r with any ordering,
 * changes the ordering to a(v),ws(-w)
 **/
bool changetoAWSRing(ring r, gfan::ZVector v, gfan::ZVector w)
{
  omFree(r->order);
  r->order  = (int*) omAlloc0(4*sizeof(int));
  omFree(r->block0);
  r->block0 = (int*) omAlloc0(4*sizeof(int));
  omFree(r->block1);
  r->block1 = (int*) omAlloc0(4*sizeof(int));
  for (int i=0; r->wvhdl[i]; i++)
  { omFree(r->wvhdl[i]); }
  omFree(r->wvhdl);
  r->wvhdl  = (int**) omAlloc0(4*sizeof(int*));

  bool ok = false;
  r->order[0]  = ringorder_a;
  r->block0[0] = 1;
  r->block1[0] = r->N;
  r->wvhdl[0]  = ZVectorToIntStar(v,ok);
  r->order[1]  = ringorder_ws;
  r->block0[1] = 1;
  r->block1[1] = r->N;
  r->wvhdl[1]  = ZVectorToIntStar(w,ok);
  r->order[2]=ringorder_C;
  return ok;
}


/***
 * Given a ring with ordering a(v'),ws(w'),
 * changes the weights to v,w
 **/
bool changeAWSWeights(ring r, gfan::ZVector v, gfan::ZVector w)
{
  omFree(r->wvhdl[0]);
  omFree(r->wvhdl[1]);
  bool ok = false;
  r->wvhdl[0]  = ZVectorToIntStar(v,ok);
  r->wvhdl[1]  = ZVectorToIntStar(w,ok);
  return ok;
}


// /***
//  * Creates an int* representing the transposition of the last two variables
//  **/
// static inline int* createPermutationVectorForSaturation(static const ring &r)
// {
//   int* w = (int*) omAlloc0((rVar(r)+1)*sizeof(int));
//   for (int i=1; i<=rVar(r)-2; i++)
//     w[i] = i;
//   w[rVar(r)-1] = rVar(r);
//   w[rVar(r)] = rVar(r)-1;
// }


/***
 * Creates an int* representing the permutation
 * 1 -> 1, ..., i-1 -> i-1, i -> n, i+1 -> n-1, ... , n -> i
 **/
static inline int* createPermutationVectorForSaturation(const ring &r, const int i)
{
  int* sigma = (int*) omAlloc0((rVar(r)+1)*sizeof(int));
  int j;
  for (j=1; j<i; j++)
    sigma[j] = j;
  for (; j<=rVar(r); j++)
    sigma[j] = rVar(r)-j+i;
  return(sigma);
}


/***
 * Changes the int* representing the permutation
 * 1 -> 1, ..., i -> i, i+1 -> n, i+2 -> n-1, ... , n -> i+1
 * to an int* representing the permutation
 * 1 -> 1, ..., i-1 -> i-1, i -> n, i+1 -> n-1, ... , n -> i
 **/
static void changePermutationVectorForSaturation(int* sigma, const ring &r, const int i)
{
  for (int j=i; j<rVar(r); j++)
    sigma[j] = rVar(r)-j+i;
  sigma[rVar(r)] = i;
}


/***
 * returns a ring in which the weights of the ring variables are permuted
 * if handed over a poly in which the variables are permuted, this is basically
 * as good as permuting the variables of the ring itself.
 **/
static ring permuteWeighstOfRingVariables(const ring &r, const int* const sigma)
{
  ring s = rCopy0(r);
  for (int j=0; j<rVar(r); j++)
  {
    s->wvhdl[0][j] = r->wvhdl[0][sigma[j+1]];
    s->wvhdl[1][j] = r->wvhdl[1][sigma[j+1]];
  }
  rComplete(s,1);
  return s;
}


/***
 * creates a ring s that is a copy of r except with ordering wp(w)
 **/
static inline ring createInitialRingForSaturation(const ring &r, const gfan::ZVector &w, bool &ok)
{
  assume(rVar(r) == (int) w.size());

  ring s = rCopy0(r); int i;
  for (i=0; s->order[i]; i++)
    omFreeSize(s->wvhdl[i],rVar(r)*sizeof(int));
  i++;
  omFreeSize(s->order,i*sizeof(int));
  s->order  = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->block0,i*sizeof(int));
  s->block0 = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->block1,i*sizeof(int));
  s->block1 = (int*) omAlloc0(3*sizeof(int));
  omFreeSize(s->wvhdl,i*sizeof(int*));
  s->wvhdl  = (int**) omAlloc0(3*sizeof(int*));

  s->order[0]  = ringorder_wp;
  s->block0[0] = 1;
  s->block1[0] = rVar(r);
  s->wvhdl[0]  = ZVectorToIntStar(w,ok);
  s->order[1]=ringorder_C;

  rComplete(s,1);
  return s;
}


/***
 * Given an weighted homogeneous ideal I with respect to weight w
 * that in standard basis form with respect to the ordering ws(-w),
 * derives the standard basis of I:<x_n>^\infty
 * and returns a long k such that I:<x_n>^\infty=I:<x_n>^k
 **/
static long deriveStandardBasisOfSaturation(ideal &I, ring &r)
{
  long k=0, l; poly current;
  for (int i=0; i<IDELEMS(I); i++)
  {
    current = I->m[i];
    l = p_GetExp(current,rVar(r),r);
    if (k<l) k=l;
    while (current)
    {
      p_SubExp(current,rVar(r),l,r); p_Setm(current,r);
      pIter(current);
    }
  }
  return k;
}


/***
 * Given a weighted homogeneous ideal I with respect to weight w
 * with constant first element,
 * returns NULL if I does not contain a monomial
 * otherwise returns the monomial contained in I
 **/
poly containsMonomial(const ideal &I, const gfan::ZVector &w)
{
  assume(rField_is_Ring_Z(currRing));

  // first we switch to the ground field currRing->cf / I->m[0]
  ring r = rCopy0(currRing);
  nKillChar(r->cf);
  r->cf = nInitChar(n_Zp,(void*)(long)n_Int(p_GetCoeff(I->m[0],currRing),currRing->cf));
  rComplete(r);

  ideal J = id_Copy(I, currRing); poly cache; number temp;
  for (int i=0; i<IDELEMS(I); i++)
  {
    cache = J->m[i];
    while (cache)
    {
      // TODO: temp = npMapGMP(p_GetCoeff(cache,currRing),currRing->cf,r->cf);
      p_SetCoeff(cache,temp,r); pIter(cache);
    }
  }


  J = kStd(J,NULL,isHomog,NULL);

  bool b = false;
  ring s = createInitialRingForSaturation(currRing, w, b);
  if (b)
  {
    WerrorS("containsMonomial: overflow in weight vector");
    return NULL;
  }

  return NULL;
}


gfan::ZCone* startingCone(ideal I)
{
  I = kStd(I,NULL,isNotHomog,NULL);
  gfan::ZCone* zc = maximalGroebnerCone(currRing,I);
  gfan::ZMatrix rays = zc->extremeRays();
  gfan::ZVector v;
  for (int i=0; i<rays.getHeight(); i++)
  {
    v = rays[i];
  }
  return zc;
}
#endif


void tropical_setup(SModulFunctions* p)
{
  p->iiAddCproc("","groebnerCone",FALSE,groebnerCone);
  p->iiAddCproc("","maximalGroebnerCone",FALSE,maximalGroebnerCone);
  p->iiAddCproc("","initial",FALSE,initial);
#ifndef NDEBUG
  // p->iiAddCproc("","divideByGcd",FALSE,divideByGcd);
  p->iiAddCproc("","pReduce",FALSE,pReduce);
#endif //NDEBUG
  // p->iiAddCproc("","initialReduction",FALSE,initialReduction);
  p->iiAddCproc("","homogeneitySpace",FALSE,homogeneitySpace);
}
