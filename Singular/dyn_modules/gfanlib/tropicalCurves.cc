#include <gfanlib/gfanlib_matrix.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <gfanlib_exceptions.h>
#include <bbcone.h>
#include <bbpolytope.h>
#include <bbfan.h>
#include <containsMonomial.h>
#include <initial.h>
#include <witness.h>
#include <set>

/***
 * Returns the tropical curve of a polynomial g in ring r
 **/
gfan::ZFan tropicalCurve(const poly g, const ring r)
{
  int n = r->N;

  // return the empty fan if g is zero or a single term
  if (!g || !g->next)
    return gfan::ZFan(n);

  // otherwise cycle through all pairs of terms of g,
  // construct the cone in which these two are amongst the highest weighted degree,
  // and insert it into the tropical variety.
  int* expv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZMatrix exponents = gfan::ZMatrix(0,n);
  for (poly s=g; s; pIter(s))
  {
    p_GetExpV(s,expv,r);
    gfan::ZVector zv = intStar2ZVector(n,expv);
    exponents.appendRow(intStar2ZVector(n,expv));
  }
  omFreeSize(expv,(n+1)*sizeof(int));

  gfan::ZFan zf = gfan::ZFan(n);
  int l = exponents.getHeight();
  for (int i=0; i<l; i++)
  {
    for (int j=i+1; j<l; j++)
    {
      gfan::ZMatrix equation = gfan::ZMatrix(0,n);
      equation.appendRow(exponents[i]-exponents[j]);
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      for (int k=0; k<l; k++)
        if (k!=i && k!=j) inequalities.appendRow(exponents[i]-exponents[k]);
      // std::cout << "equation:" << equation << std::endl;
      // std::cout << "inequalities:" << inequalities << std::endl;
      gfan::ZCone zc = gfan::ZCone(inequalities,equation);
      zf.insert(zc);
    }
  }
 return zf;
}

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args)
{
  leftv u = args;
  poly g = (poly) u->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  gfan::ZFan* zf = new gfan::ZFan(tropicalCurve(g,currRing));
  p_Delete(&g,currRing);
  res->rtyp = fanID;
  res->data = (char*) zf;
  return FALSE;
}
#endif

/***
 * Returns a strictly positive weight vector v with respect to whom
 * any x-homogeneous element is homogeneous to
 * if and only if it is homogeneous with respect to w.
 **/
static inline gfan::ZVector findPositiveWeight(const gfan::ZVector w)
// {
//   gfan::ZVector v=gfan::ZVector(w.size());
//   v[0]=-w[0];
//   gfan::Integer max=-1;
//   for (unsigned i=1; i<w.size(); i++)
//     if (max<w[i]) max=w[i];
//   for (unsigned i=1; i<w.size(); i++)
//     v[i]=-w[i]+max+1;
//   return v;
// }
{
  gfan::ZVector v=gfan::ZVector(w.size());
  gfan::Integer min=-1;
  for (unsigned i=1; i<w.size(); i++)
    if (w[i]<min) min=w[i];
  if (min<1)
  {
    for (unsigned i=1; i<w.size(); i++)
      v[i]=w[i]-min+1;
  }
  return v;
}


/***
 * Returns a weight vector v which coincides with a weight vector e
 * on any set of x-homogeneous elements that are also homogeneous with respect to w,
 * w containing only positive weights
 **/
static inline gfan::ZVector findPositiveWeight(const gfan::ZVector e, const gfan::ZVector w)
// {
//   gfan::ZVector v=e-(e[1]/w[1]+1)*w;
//   gfan::Integer min=1;
//   for (unsigned i=1; i<v.size(); i++)
//     if (v[i]<min) min=v[i];
//   for (unsigned i=1; i<v.size(); i++)
//     v[i]=v[i]-min+1;
//   return v;
// }
{
  gfan::ZVector v=gfan::ZVector(e.size());
  gfan::Integer min=-1;
  for (unsigned i=1; i<e.size(); i++)
    if (e[i]<min) min=w[i];
  if (min<1)
  {
    for (unsigned i=1; i<e.size(); i++)
      v[i]=e[i]-min+1;
  }
  return v;
}


/***
 * Given a relative interior point w of a cone in the weight space
 * and generators E of its span, returns a ring with an ordering that coincides
 * with a weighted ordering on the ideal with respect to a weight in the cone
 * which does not lie on a Groebner cone of lower dimension than that of the cone.
 **/
static ring ringWithGenericlyWeightedOrdering(const ring r,
                                              const gfan::ZVector w,
                                              const gfan::ZMatrix E)
{
  int n = r->N;
  int h = E.getHeight();

  // create a copy of r and delete its old ordering
  ring s = rCopy0(r);
  omFree(s->order);
  s->order  = (int*) omAlloc0((h+3)*sizeof(int));
  omFree(s->block0);
  s->block0 = (int*) omAlloc0((h+3)*sizeof(int));
  omFree(s->block1);
  s->block1 = (int*) omAlloc0((h+3)*sizeof(int));
  for (int j=0; s->wvhdl[j]; j++) omFree(s->wvhdl[j]);
  omFree(s->wvhdl);
  s->wvhdl  = (int**) omAlloc0((h+3)*sizeof(int*));

  // construct a new ordering and keep an eye out for weight overflows
  bool overflow;
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(w,overflow);
  for (int j=1; j<h; j++)
  {
    s->order[j] = ringorder_a;
    s->block0[j] = 1;
    s->block1[j] = n;
    s->wvhdl[j] = ZVectorToIntStar(findPositiveWeight(E[j-1],w),overflow);
  }
  s->order[h] = ringorder_wp;
  s->block0[h] = 1;
  s->block1[h] = n;
  s->wvhdl[h] = ZVectorToIntStar(findPositiveWeight(E[h-1],w),overflow);
  s->order[h+1] = ringorder_C;

  rComplete(s,1);
  // Return s.
  if (overflow)
    throw 0; //weightOverflow;
  return s;
}


/***
 * Computes the tropical curve of an x-homogeneous ideal I
 * which is weighted homogeneous with respect to weight w in ring r
 **/
gfan::ZFan tropicalCurve(const ideal I, const gfan::ZVector w, const ring r)
{
  int k = idSize(I);

  // Compute the common refinement of the tropical varieties
  // of the generating set
  gfan::ZFan C = tropicalCurve(I->m[0],r);
  for (int i=1; i<k; i++)
    C = commonRefinement(C,tropicalCurve(I->m[i],r));

  poly mon = NULL;
  do
  {
    // cycle through all maximal cones of the refinement
    // pick a monomial ordering corresponding to a generic weight vector in it
    // check if the initial ideal is monomial free
    for (int d=C.getAmbientDimension(); d>0; d--)
    {
      for (int i=0; i<C.numberOfConesOfDimension(d,0,1); i++)
      {
        gfan::ZCone zc = C.getCone(d,i,0,1);
        // std::cout << "equation:" << zc.getFacets() << std::endl;
        // std::cout << "inequalities:" << zc.getImpliedEquations() << std::endl;

        gfan::ZVector v = zc.getRelativeInteriorPoint();
        gfan::ZMatrix E = zc.generatorsOfSpan();

        // std::cout << "generators of span: " << E << std::endl;
        ideal inI = initial(I,r,E[E.getHeight()-1]);
        ring s = ringWithGenericlyWeightedOrdering(r,v,E);
        nMapFunc nMap = n_SetMap(r->cf,s->cf);
        ideal J = idInit(k);
        for (int j=0; j<k; j++)
        {
          J->m[j] = p_PermPoly(inI->m[j],NULL,r,s,nMap,NULL,0);
          p_Test(J->m[j],s);
        }

        poly mon = checkForMonomialViaSuddenSaturation(J,s);
        if (mon)
        {
          poly g = witness(mon,J,s);
          gfan::ZFan zf = tropicalCurve(g,s);
          // std::cout << "before refinement" << std::endl << C.toStringJustRaysAndMaximalCones;
          // std::cout << "tropical curve" << std::endl << zf.toStringJustRaysAndMaximalCones;
          C = commonRefinement(C,zf);
          // std::cout << "after refinement" << std::endl << C.toStringJustRaysAndMaximalCones;
          p_Delete(&g,s);
          d = 0; break;
        }
        id_Delete(&J,s);
        rDelete(s);
      }
    }
  } while (mon);
  return C;
}
/***
 * Computes the tropical curve of an x-homogeneous ideal I
 * which is weighted homogeneous with respect to weight w in ring r
 **/
void dummyTropicalCurve(const ideal I, const gfan::ZVector w, const ring r)
{
  int k = idSize(I);

  // Compute the common refinement of the tropical varieties
  // of the generating set
  gfan::ZFan C = tropicalCurve(I->m[0],r);
  for (int i=1; i<k; i++)
    C = commonRefinement(C,tropicalCurve(I->m[i],r));

  poly mon = NULL;
  // do
  // {
  //   // cycle through all maximal cones of the refinement
  //   // pick a monomial ordering corresponding to a generic weight vector in it
  //   // check if the initial ideal is monomial free
  //   for (int d=C.getAmbientDimension(); d>0; d--)
  //   {
  //     for (int i=0; i<C.numberOfConesOfDimension(d,0,1); i++)
  //     {
  //       gfan::ZCone zc = C.getCone(d,i,0,1);
  //       // std::cout << "equation:" << zc.getFacets() << std::endl;
  //       // std::cout << "inequalities:" << zc.getImpliedEquations() << std::endl;

  //       gfan::ZVector v = zc.getRelativeInteriorPoint();
  //       gfan::ZMatrix E = zc.generatorsOfSpan();

  //       // std::cout << "generators of span: " << E << std::endl;
  //       ideal inI = initial(I,r,E[E.getHeight()-1]);
  //       ring s = ringWithGenericlyWeightedOrdering(r,v,E);
  //       nMapFunc nMap = n_SetMap(r->cf,s->cf);
  //       ideal J = idInit(k);
  //       for (int j=0; j<k; j++)
  //       {
  //         J->m[j] = p_PermPoly(inI->m[j],NULL,r,s,nMap,NULL,0);
  //         p_Test(J->m[j],s);
  //       }

  //       poly mon = checkForMonomialViaSuddenSaturation(J,s);
  //       if (mon)
  //       {
  //         poly g = witness(mon,J,s);
  //         gfan::ZFan zf = tropicalCurve(g,s);
  //         // std::cout << "before refinement" << std::endl << C.toStringJustRaysAndMaximalCones;
  //         // std::cout << "tropical curve" << std::endl << zf.toStringJustRaysAndMaximalCones;
  //         C = commonRefinement(C,zf);
  //         // std::cout << "after refinement" << std::endl << C.toStringJustRaysAndMaximalCones;
  //         p_Delete(&g,s);
  //         d = 0; break;
  //       }
  //       id_Delete(&J,s);
  //       rDelete(s);
  //     }
  //   }
  // } while (mon);
  // return C;
}

#ifndef NDEBUG
BOOLEAN tropicalCurve1(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  bigintmat* w0 = (bigintmat*) v->Data();
  gfan::ZVector* w = bigintmatToZVector(w0);
  gfan::ZFan* zf = new gfan::ZFan(tropicalCurve(I,*w,currRing));
  id_Delete(&I,currRing);
  delete w;
  res->rtyp = fanID;
  res->data = (char*) zf;
  return FALSE;
}
BOOLEAN tropicalCurve2(leftv res, leftv args)
{
  leftv u = args;
  ideal I = (ideal) u->CopyD();
  leftv v = u->next;
  bigintmat* w0 = (bigintmat*) v->Data();
  gfan::ZVector* w = bigintmatToZVector(w0);
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  (void) dummyTropicalCurve(I,*w,currRing);
  id_Delete(&I,currRing);
  delete w;
  res->rtyp = NONE;
  res->data = NULL;
  return FALSE;
}
#endif
