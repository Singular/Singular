/***************************************************************
 *
 * File:       gitfan.cc
 * Purpose:    Computationally intensive procedures for gitfan.lib,
 *             outsourced to improve the performance.
 * Authors:    Janko Boehm    boehm@mathematik.uni-kl.de
 *             Simon Keicher  keicher@mail.mathematik.uni-tuebingen.de
 *             Yue Ren        ren@mathematik.uni-kl.de
 *
 ***************************************************************/

#include <kernel/mod2.h>

#if HAVE_GFANLIB

#include <callgfanlib_conversion.h>
#include <bbcone.h>
#include <bbfan.h>
#include <gitfan.h>

#include <Singular/ipid.h>
#include <Singular/lists.h>
#include <Singular/ipshell.h>

#include <coeffs/bigintmat.h>


namespace gitfan
{

  facet::facet():
    eta(gfan::ZCone()),
    interiorPoint(gfan::ZVector()),
    facetNormal(gfan::ZVector())
  {
  }

  facet::facet(const gitfan::facet &f):
    eta(f.eta),
    interiorPoint(f.interiorPoint),
    facetNormal(f.facetNormal)
  {
#ifndef SING_NDEBUG
    gfan::ZCone c = f.eta;
    gfan::ZVector v = f.interiorPoint;
    gfan::ZVector w = f.facetNormal;
    assume(c.ambientDimension() == (int)v.size());
    assume(c.ambientDimension() == (int)w.size());
    assume(c.contains(v));
    assume(!c.contains(w));
#endif
  }

  facet::facet(const gfan::ZCone &c, const gfan::ZVector &v, const gfan::ZVector &w):
    eta(c),
    interiorPoint(v),
    facetNormal(w)
  {
#ifndef SING_NDEBUG
    assume(c.ambientDimension() == (int)v.size());
    assume(c.ambientDimension() == (int)w.size());
    assume(c.contains(v));
    assume(!c.contains(w));
#endif
  }

  facet::~facet()
  {
#ifndef SING_NDEBUG
    gfan::ZCone c = this->eta;
    gfan::ZVector v = this->interiorPoint;
    gfan::ZVector w = this->facetNormal;
    assume(c.ambientDimension() == (int)v.size());
    assume(c.ambientDimension() == (int)w.size());
    assume(c.contains(v));
    assume(!c.contains(w));
#endif
  }

  void mergeFacets(facets &F, const facets &newFacets)
  {
    std::pair<facets::iterator,bool> check(newFacets.begin(),false);
    for(facets::iterator p=newFacets.begin(); p!=newFacets.end(); p++)
    {
      check = F.insert(*p);
      if(!check.second)
        F.erase(check.first);
    }
  }

}


static gfan::ZCone subcone(const lists &cones, const gfan::ZVector &point)
{
  gfan::ZCone sigma = gfan::ZCone(gfan::ZMatrix(1,point.size()), gfan::ZMatrix(1,point.size()));
  gfan::ZCone* zc;
  for (int i=0; i<=cones->nr; i++)
  {
    zc = (gfan::ZCone*) cones->m[i].Data();
    if (zc->contains(point))
      sigma = gfan::intersection(sigma,*zc);
  }
  return(sigma);
}

static gitfan::facets interiorFacets(const gfan::ZCone &zc, const gfan::ZCone &bound)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();
  gitfan::facets F;
  if (r*c == 0)
    /***
     * this is the trivial case where either we are in a zerodimensional ambient space,
     * or the cone has no facets.
     **/
    return F;

  // int index = 0;
  /* next we iterate over each of the r facets, build the respective cone and add it to the list */
  /* this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone eta = gfan::ZCone(newInequalities,newEquations);
  eta.canonicalize();
  gfan::ZVector v = eta.getRelativeInteriorPoint();
  gfan::ZVector w = inequalities[0];

  if (bound.containsRelatively(v))
    F.insert(gitfan::facet(eta,v,w));

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    eta = gfan::ZCone(newInequalities,newEquations);
    eta.canonicalize();
    v = eta.getRelativeInteriorPoint();
    w = inequalities[i];
    if (bound.containsRelatively(v))
      F.insert(gitfan::facet(eta,v,w));
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  eta = gfan::ZCone(newInequalities,newEquations);
  eta.canonicalize();

  v = eta.getRelativeInteriorPoint();
  w = inequalities[r-1];
  if (bound.containsRelatively(v))
    F.insert(gitfan::facet(eta,v,w));

  return F;
}

BOOLEAN refineCones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == BIGINTMAT_CMD))
    {
      lists cones = (lists) u->Data();
      bigintmat* bim = (bigintmat*) v->Data();
      gfan::ZMatrix* zm = bigintmatToZMatrix(bim->transpose());
      gfan::ZCone support = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
      delete zm;

      /***
       * Randomly compute a first full-dimensional cone and insert it into the fan.
       * Compute a list of facets and relative interior points.
       * The relative interior points are unique, assuming the cone is stored in canonical form,
       * which is the case in our algorithm, as we supply no redundant inequalities.
       * Hence we can decide whether a facet need to be traversed by crosschecking
       * its relative interior point with this list.
       **/
      gfan::ZCone lambda; gfan::ZVector point;
      do
      {
        point = randomPoint(&support);
        lambda = subcone(cones, point);
      }
      while (lambda.dimension() < lambda.ambientDimension());
      int iterationNumber = 1;
      std::cout << "cones found: " << iterationNumber++ << std::endl;

      lambda.canonicalize();
      gfan::ZFan* Sigma = new gfan::ZFan(lambda.ambientDimension());
      Sigma->insert(lambda);
      gitfan::facets F = interiorFacets(lambda, support);
      if (F.empty())
      {
        res->rtyp = fanID;
        res->data = (void*) Sigma;
        return FALSE;
      }
      int mu = 1024;

      gitfan::facet f;
      gfan::ZCone eta;
      gfan::ZVector interiorPoint;
      gfan::ZVector facetNormal;
      gitfan::facets newFacets;
      while (!F.empty())
      {
        /***
         * Extract a facet to traverse and its relative interior point.
         **/
        f = *(F.begin());
        eta = f.getEta();
        interiorPoint = f.getInteriorPoint();
        facetNormal = f.getFacetNormal();

        /***
         * construct a point, which lies on the other side of the facet.
         * make sure it lies in the known support of our fan
         * and that the cone around the point is maximal, containing eta.
         **/
        point = mu * interiorPoint - facetNormal;
        while (!support.containsRelatively(point))
        {
          mu = mu * 16;
          point = mu * interiorPoint - facetNormal;
        }

        lambda = subcone(cones,point);
        while ((lambda.dimension() < lambda.ambientDimension()) && !(lambda.contains(interiorPoint)))
        {
          mu = mu * 16;
          point = mu * interiorPoint - facetNormal;
          lambda = subcone(cones,point);
        }
        std::cout << "cones found: " << iterationNumber++ << std::endl;

        /***
         * insert lambda into Sigma, and create a list of facets of lambda.
         * merge the two lists of facets
         **/
        lambda.canonicalize();
        Sigma->insert(lambda);
        newFacets = interiorFacets(lambda, support);
        mergeFacets(F,newFacets);
        newFacets.clear();
      }
      res->rtyp = fanID;
      res->data = (void*) Sigma;
      return FALSE;
    }
  }
  WerrorS("refineCones: unexpected parameters");
  return TRUE;
}


static int binomial(int n, int k)
{
  if (n<k)
    return(0);
  gfan::Integer num = 1;
  gfan::Integer den = 1;
  for (int i=1; i<=k; i++)
    den = den*i;
  for (int j=n-k+1; j<=n; j++)
    num = num*j;
  gfan::Integer bin = num/den;
  return(bin.toInt());
}


intvec* intToAface(unsigned int v0, int n, int k)
{
  intvec* v = new intvec(k);
  int j = 0;
  for (int i=0; i<n; i++)
  {
    if (v0 & (1<<i))
      (*v)[j++] = i+1;
  }
  return v;
}


BOOLEAN listOfAfacesToCheck(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      int n = (int)(long) u->Data();
      int k = (int)(long) v->Data();
      unsigned int v = 0;
      for (int i=0; i<k; i++)
        v |= 1<<i;  // sets the first k bits of v as 1

      lists L = (lists)omAllocBin(slists_bin);
      int count = (int) binomial(n,k); L->Init(count);
      unsigned int t;
      while (!(v & (1<<n)))
      {
        L->m[--count].rtyp = INTVEC_CMD;
        L->m[count].data = (void*) intToAface(v,n,k);

        // t gets v's least significant 0 bits set to 1
        t = v | (v - 1);
        // Next set to 1 the most significant bit to change,
        // set to 0 the least significant ones, and add the necessary 1 bits.
        v = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
      }
      res->rtyp = LIST_CMD;
      res->data = (void*) L;
      return FALSE;
    }
  }
  WerrorS("listOfAfacesToCheck: unexpected parameter");
  return TRUE;
}


BOOLEAN nextAfaceToCheck(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTVEC_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        intvec* aface = (intvec*) u->Data();
        int ambientDimension = (int)(long) v->Data();
        int dimension = (int)(long) w->Data();

        unsigned int af = 0;
        for (int i=0; i<aface->length(); i++)
          af |= 1<<((*aface)[i]-1);

        unsigned int t = af | (af - 1);
        af = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(af) + 1));

        if (af & (1<<ambientDimension))
        {
          res->rtyp = INTVEC_CMD;
          res->data = (void*) new intvec(1);
          return FALSE;
        }

        res->rtyp = INTVEC_CMD;
        res->data = (void*) intToAface(af,ambientDimension,dimension);
        return FALSE;
      }
    }
  }
  WerrorS("nextAfaceToCheck: unexpected parameter");
  return TRUE;
}


void gitfan_setup(SModulFunctions* p)
{
  p->iiAddCproc("","refineCones",FALSE,refineCones);
  p->iiAddCproc("","listOfAfacesToCheck",FALSE,listOfAfacesToCheck);
  p->iiAddCproc("","nextAfaceToCheck",FALSE,nextAfaceToCheck);
}

#endif
