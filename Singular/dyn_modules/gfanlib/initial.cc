#include <kernel/ideals.h>
#include <libpolys/polys/monomials/p_polys.h>

#include <gfanlib/gfanlib.h>

#include <exception>

long wDeg(const poly p, const ring r, const gfan::ZVector w)
{
  long d=0;
  for (unsigned i=0; i<w.size(); i++)
  {
    if (!w[i].fitsInInt())
      throw 0; // weightOverflow;
    d += p_GetExp(p,i+1,r)*w[i].toInt();
  }
  return d;
}

gfan::ZVector WDeg(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  gfan::ZVector d = gfan::ZVector(W.getHeight()+1);
  d[0] = wDeg(p,r,w);
  for (int i=0; i<W.getHeight(); i++)
    d[i+1] = wDeg(p,r,W[i]);
  return d;
}

poly initial(const poly p, const ring r, const gfan::ZVector w)
{
  if (p==NULL)
    return NULL;

  poly q0 = p_Head(p,r);
  poly q1 = q0;
  long d = wDeg(p,r,w);
  for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
  {
    long e = wDeg(currentTerm,r,w);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p_Head(currentTerm,r);
      q1 = q0;
      d = e;
    }
    else
      if (e==d)
      {
        pNext(q1) = p_Head(currentTerm,r);
        pIter(q1);
      }
  }
  return q0;
}

ideal initial(const ideal I, const ring r, const gfan::ZVector w)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r,w);
  return inI;
}

poly initial(const poly p, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  if (p==NULL)
    return NULL;

  poly q0 = p_Head(p,r);
  poly q1 = q0;
  gfan::ZVector d = WDeg(p,r,w,W);
  for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
  {
    gfan::ZVector e = WDeg(currentTerm,r,w,W);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p_Head(p,r);
      q1 = q0;
      d = e;
    }
    else
      if (d==e)
      {
        pNext(q1) = p_Head(currentTerm,r);
        pIter(q1);
      }
  }
  return q0;
}

ideal initial(const ideal I, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  int k = idSize(I); ideal inI = idInit(k);
  for (int i=0; i<k; i++)
    inI->m[i] = initial(I->m[i],r,w,W);
  return inI;
}

void initial(poly* pStar, const ring r, const gfan::ZVector w)
{
  poly p = *pStar;
  if (p==NULL)
    return;

  long d = wDeg(p,r,w);
  poly q0 = p;
  poly q1 = q0;
  pNext(q1) = NULL;
  pIter(p);

  while(p)
  {
    long e = wDeg(p,r,w);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p;
      q1 = q0;
      pNext(q1) = NULL;
      d = e;
      pIter(p);
    }
    else
      if (e==d)
      {
        pNext(q1) = p;
        pIter(q1);
        pNext(q1) = NULL;
        pIter(p);
      }
      else
        p = p_LmDeleteAndNext(p,r);
  }
  pStar = &q0;
  return;
}

void initial(ideal* IStar, const ring r, const gfan::ZVector w)
{
  ideal I = *IStar;
  int k = idSize(I);
  for (int i=0; i<k; i++)
    initial(&I->m[i],r,w);
  return;
}

void initial(poly* pStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  poly p = *pStar;
  if (p==NULL)
    return;

  gfan::ZVector d = WDeg(p,r,w,W);
  poly q0 = p;
  poly q1 = q0;
  pNext(q1) = NULL;
  pIter(p);

  while(p)
  {
    gfan::ZVector e = WDeg(p,r,w,W);
    if (d<e)
    {
      p_Delete(&q0,r);
      q0 = p;
      q1 = q0;
      pNext(q1) = NULL;
      d = e;
      pIter(p);
    }
    else
      if (d==e)
      {
        pNext(q1) = p;
        pIter(q1);
        pNext(q1) = NULL;
        pIter(p);
      }
      else
        p = p_LmDeleteAndNext(p,r);
  }
  pStar = &q0;
  return;
}

void initial(ideal* IStar, const ring r, const gfan::ZVector w, const gfan::ZMatrix W)
{
  ideal I = *IStar;
  int k = idSize(I);
  for (int i=0; i<k; i++)
    initial(&I->m[i],r,w,W);
  return;
}


/***
 * throwaway code
 **/
// gfan::ZVector WDeg(const poly p, const ring r, const gfan::ZMatrix W)
// {
//   gfan::ZVector d = gfan::ZVector(W.getHeight());
//   for (int i=0; i<W.getHeight(); i++)
//     d[i] = wDeg(p,r,W[i]);
//   return d;
// }

// /**
//  * Checks if p is sorted with respect to w.
//  */
// static bool checkSloppyInput(const poly p, const ring r, const gfan::ZVector w)
// {
//   long d = wDeg(p,r,w);
//   for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
//   {
//     long e = wDeg(currentTerm,r,w);
//     if (e>d)
//       return false;
//   }
//   return true;
// }

// /***
//  * Returns the terms of p of same weighted degree under w as the leading term.
//  * Coincides with the initial form of p with respect to w if and only if p was already
//  * sorted with respect to w in the sense that the leading term is of highest w-degree.
//  **/
// poly sloppyInitial(const poly p, const ring r, const gfan::ZVector w)
// {
//   assume(checkSloppyInput(p,r,w));
//   int n = rVar(r);
//   int* expv = (int*) omAlloc(n*sizeof(int));
//   poly q0 = p_Head(p,r);
//   poly q1 = q0;
//   long d = wDeg(p,r,w);
//   for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
//   {
//     if (wDeg(currentTerm,r,w) == d)
//     {
//       pNext(q1) = p_Head(currentTerm,r);
//       pIter(q1);
//     }
//   }
//   omFreeSize(expv,n*sizeof(int));
//   return q0;
// }

// /***
//  * Runs the above procedure over all generators of an ideal.
//  * Coincides with the initial ideal of I with respect to w if and only if
//  * the elements of I were already sorted with respect to w and
//  * I is a standard basis form with respect to w.
//  **/
// ideal sloppyInitial(const ideal I, const ring r, const gfan::ZVector w)
// {
//   int k = idSize(I); ideal inI = idInit(k);
//   for (int i=0; i<k; i++)
//     inI->m[i] = sloppyInitial(I->m[i],r,w);
//   return inI;
// }

// #ifndef NDEBUG
// BOOLEAN initial0(leftv res, leftv args)
// {
//   leftv u = args;
//   ideal I = (ideal) u->CopyD();
//   leftv v = u->next;
//   bigintmat* w0 = (bigintmat*) v->Data();
//   gfan::ZVector* w = bigintmatToZVector(w0);
//   omUpdateInfo();
//   Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
//   ideal inI = initial(I,currRing,*w);
//   id_Delete(&I,currRing);
//   delete w;
//   res->rtyp = IDEAL_CMD;
//   res->data = (char*) inI;
//   return FALSE;
// }
// #endif

// poly initial(const poly p, const ring r, const gfan::ZMatrix W)
// {
//   poly q0 = p_Head(p,r);
//   poly q1 = q0;
//   gfan::ZVector d = WDeg(p,r,W);
//   for (poly currentTerm = p->next; currentTerm; pIter(currentTerm))
//   {
//     gfan::ZVector e = WDeg(currentTerm,r,W);
//     if (d<e)
//     {
//       p_Delete(&q0,r);
//       q0 = p_Head(p,r);
//       q1 = q0;
//       d = e;
//     }
//     else
//       if (d==e)
//       {
//         pNext(q1) = p_Head(currentTerm,r);
//         pIter(q1);
//       }
//   }
//   return q0;
// }

// ideal initial(const ideal I, const ring r, const gfan::ZMatrix W)
// {
//   int k = idSize(I); ideal inI = idInit(k);
//   for (int i=0; i<k; i++)
//     inI->m[i] = initial(I->m[i],r,W);
//   return inI;
// }

// /***
//  * Computes the initial form of p with respect to the first row in the order matrix
//  **/
// poly initial(const poly p, const ring r)
// {
//   long d = p_Deg(p,r);
//   poly initialForm0 = p_Head(p,r);
//   poly initialForm1 = initialForm0;
//   poly currentTerm = p->next;
//   while (currentTerm && p_Deg(currentTerm,r)==d)
//   {
//     pNext(initialForm1) = p_Head(currentTerm,r);
//     pIter(currentTerm); pIter(initialForm1);
//   }
//   return initialForm0;
// }

// /***
//  * Computes the initial form of all generators of I.
//  * If I is a standard basis, then this is a standard basis
//  * of the initial ideal.
//  **/
// ideal initial(const ideal I, const ring r)
// {
//   int k = idSize(I); ideal inI = idInit(k);
//   for (int i=0; i<k; i++)
//     inI->m[i] = initial(I->m[i],r);
//   return inI;
// }
