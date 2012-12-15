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

#include <Singular/mod2.h>
#ifdef HAVE_FANS

#include <Singular/lists.h>
#include <Singular/ipshell.h>
#include <kernel/bigintmat.h>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/gitfan.h>

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
#ifndef NDEBUG
    gfan::ZCone c = f.eta;
    gfan::ZVector v = f.interiorPoint;
    gfan::ZVector w = f.facetNormal;
    assume(c.ambientDimension() == v.size());
    assume(c.ambientDimension() == w.size());
    assume(c.contains(v));
    assume(!c.contains(w));
#endif
  }
  
  facet::facet(const gfan::ZCone &c, const gfan::ZVector &v, const gfan::ZVector &w):
    eta(c),
    interiorPoint(v),
    facetNormal(w)
  {
#ifndef NDEBUG
    assume(c.ambientDimension() == v.size());
    assume(c.ambientDimension() == w.size());
    assume(c.contains(v));
    assume(!c.contains(w));
#endif
  }

  facet::~facet()
  {
#ifndef NDEBUG
    gfan::ZCone c = this->eta;
    gfan::ZVector v = this->interiorPoint;
    gfan::ZVector w = this->facetNormal;
    assume(c.ambientDimension() == v.size());
    assume(c.ambientDimension() == w.size());
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

  int index = 0;
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
    if ((u != NULL) && (v->Typ() == BIGINTMAT_CMD))
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

void gitfan_setup()
{
  iiAddCproc("","refineCones",FALSE,refineCones);
}

#endif
