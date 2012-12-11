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

/***
 * constructing a list of facets of zc lying in the relative interior of bound
 **/
static lists listOfInteriorFacets(const gfan::ZCone &zc, const gfan::ZCone &bound)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  lists L = (lists) omAllocBin(slists_bin);
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();
  L->Init(r);

  int index = -1;
  /* next we iterate over each of the r facets, build the respective cone and add it to the list */
  /* this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations; 
  newEquations.appendRow(inequalities[0]);

  gfan::ZCone* eta = new gfan::ZCone(newInequalities,newEquations);
  eta->canonicalize();
  gfan::ZVector* v = new gfan::ZVector(eta->getRelativeInteriorPoint());
  gfan::ZVector* w = new gfan::ZVector(inequalities[0]);

  if (bound.containsRelatively(*v))
  {
    index++;
    lists l = (lists) omAllocBin(slists_bin);
    l->Init(3);
    l->m[0].rtyp = coneID; 
    l->m[0].data = (void*) eta;
    l->m[1].rtyp = coneID; 
    l->m[1].data = (void*) v;
    l->m[2].rtyp = coneID; 
    l->m[2].data = (void*) w;
    L->m[index].rtyp = LIST_CMD; 
    L->m[index].data = (void*) l;
    eta = NULL;
    v = NULL;
  }
  else
  {
    delete eta;
    delete v;
    delete w;
  }
  
  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    eta = new gfan::ZCone(newInequalities,newEquations);
    eta->canonicalize();
    v = new gfan::ZVector(eta->getRelativeInteriorPoint());
    w = new gfan::ZVector(inequalities[i]);
    if (bound.containsRelatively(*v))
    {    
      index++;
      lists l = (lists) omAllocBin(slists_bin);
      l->Init(3);
      l->m[0].rtyp = coneID; 
      l->m[0].data = (void*) eta;
      l->m[1].rtyp = coneID; 
      l->m[1].data = (void*) v;
      l->m[2].rtyp = coneID; 
      l->m[2].data = (void*) w;
      L->m[index].rtyp = LIST_CMD; 
      L->m[index].data = (void*) l;
      eta = NULL;
      v = NULL;
    }
    else
    {
      delete eta;
      delete v;
      delete w;
    }
  }
  
  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  eta = new gfan::ZCone(newInequalities,newEquations);
  eta->canonicalize();

  v = new gfan::ZVector(eta->getRelativeInteriorPoint());
  w = new gfan::ZVector(inequalities[r-1]);
  if (bound.containsRelatively(*v))
  {    
    index++;
    lists l = (lists) omAllocBin(slists_bin);
    l->Init(3);
    l->m[0].rtyp = coneID; 
    l->m[0].data = (void*) eta;
    l->m[1].rtyp = coneID; 
    l->m[1].data = (void*) v;
    l->m[2].rtyp = coneID; 
    l->m[2].data = (void*) w;
    L->m[index].rtyp = LIST_CMD; 
    L->m[index].data = (void*) l;
    eta = NULL;
    v = NULL;
  }
  else
  {
    delete eta;
    delete v;
    delete w;
  }

  if (index==-1)
  {
    L->m[0].rtyp = INT_CMD;         // when there are no interior facets
    L->m[0].data = (void*) (int) 0; // we'll just write an integer as zeroth entry
    return L;
  }
  if (index!=L->nr)
  {
    L->m = (leftv)omRealloc(L->m,(index+1)*sizeof(sleftv));
    L->nr = index;
  }
  return L;
}

/***
 * delete the i-th entry of ul
 **/
static void deleteEntry(lists &ul, int i)
{
  lists cache = (lists) ul->m[i].Data();
  gfan::ZVector* v = (gfan::ZVector*) cache->m[1].Data();
  delete v;
  cache->m[1].rtyp = 0;
  cache->m[1].data = NULL;
  gfan::ZVector* w = (gfan::ZVector*) cache->m[2].Data();
  delete w;
  cache->m[2].rtyp = 0;
  cache->m[2].data = NULL;

  ul->m[i].CleanUp();
}

static void swap(intvec* v, int i, int j)
{
  int k = (*v)[j];
  (*v)[j] = (*v)[i];
  (*v)[i] = k;
}

static void gnomeSort(intvec* v)
{
  int i=1;
  while (i<v->length())
  {
    if ((*v)[i-1] <= (*v)[i])
      ++i;
    else
    {
      swap(v,i-1,i);
 
      if (i > 1)
        --i;
    }
  }
}

static int findLastUsableEntry(lists &ul, int start)
{
  int i = start;
  while (ul->m[i].data == NULL && i>-1)
  {
    --i;
  }
  return i;
}

/***
 * Here, ul and vl ar lists of lists of facets and their relative interior point.
 * The lists ul and vl always contain at least one element.
 * This functions appends vl to ul with the following constraint:
 * Should a facet in vl already exist in ul, then both are to be deleted.
 * The merged list will be stored in ul and vl will be deleted.
 **/
static void mergeListsOfFacets(lists &ul, lists &vl)
{
  int ur = ul->nr;
  int vr = vl->nr;

  /***
   * We will know go through all vectors q in vl and compare them to all vectors p in ul.
   * If p and q coincide, we will delete q and p right away.
   * If q does not coincide with any p, we will mark it as to be preserved.
   * Two intvecs will keep track of entries in ul that have been deleted 
   * and all entries of vl that must be preserved.
   * countUl and countVl keep track of the number of elements to be deleted or preserved.
   * The reason why we count j up and i down is because the first entry of vl
   * tends to be equal the last entry of ul, due to the way vl and ul are created.
   **/ 
  intvec* entriesOfUlToBeFilled = new intvec(vr+1);
  intvec* entriesOfVlToBePreserved = new intvec(vr+1);
  int countUl = 0;
  int countVl = 0;
  
  gfan::ZVector* q = NULL;
  gfan::ZVector* p = NULL;
  bool toBePreserved = 1;
  lists cache0;
  lists cache1;
  for (int j=0; j<=vr; j++)
  {
    toBePreserved = 1;
    cache0 = (lists) vl->m[j].Data();
    q = (gfan::ZVector*) cache0->m[1].Data();
    for (int i=ur; i>=0; i--)
    {
      if (ul->m[i].data != NULL) // in case that entry of ul was deleted earlier
      {
        cache1 = (lists) ul->m[i].Data();
        p = (gfan::ZVector*) cache1->m[1].Data();
        if ((*q) == (*p))
        {
          deleteEntry(vl,j);
          deleteEntry(ul,i);
          (*entriesOfUlToBeFilled)[countUl] = i;
          countUl++; 
          toBePreserved = 0; 
          break;
        }
        cache1 = NULL;
      }
    }
    if (toBePreserved)
    {
      (*entriesOfVlToBePreserved)[countVl] = j;
      countVl++;
    }
    cache0 = NULL;
  }

  /***
   * In case the resulting merged list is empty,
   * we are finished now.
   **/
  if (ur+1-countUl+countVl == 0)
  {
    vl->nr = -1;
    vl->Clean();
    ul->nr = -1;
    ul->Clean();
    ul = NULL;
    delete entriesOfUlToBeFilled; 
    delete entriesOfVlToBePreserved;
    return;
  }
  
  /***
   * We fill the holes in ul, the algorithm works as follows:
   * - repeat the following until it breaks or all holes are fixed:
   *   - determine the first empty entry to be filled
   *   - determine the last usable entry
   *   - if firstEmptyEntry < lastUsableEntry, fill
   *   - else break
   * (If it breaks that means that the remaining empty slots need not to be filled)
   * Note that entriesOfUlToBeFilled is unordered.
   **/
  entriesOfUlToBeFilled->resize(countUl);
  gnomeSort(entriesOfUlToBeFilled);
  int i = 0;
  int firstEmptyEntry = (*entriesOfUlToBeFilled)[i];
  int lastUsableEntry = findLastUsableEntry(ul,ur); 
  while (1)
  {
    if (firstEmptyEntry < lastUsableEntry)
    {
      ul->m[firstEmptyEntry] = ul->m[lastUsableEntry];
      ul->m[lastUsableEntry].rtyp = 0;
      ul->m[lastUsableEntry].data = NULL; 
      lastUsableEntry = findLastUsableEntry(ul,lastUsableEntry);
    }
    else
      break;
    
    if (i < countUl-1)
    {
      ++i; firstEmptyEntry = (*entriesOfUlToBeFilled)[i];
    }
    else
      break;
  }
  
  /***
   * We resize ul accordingly
   * and append the to be preserved entries of vl to it.
   **/
  int k;
  ul->m = (leftv) omRealloc0(ul->m, (ur+1-countUl+countVl)*sizeof(sleftv));  
  ul->nr = ur-countUl+countVl;
  for (int j=0; j<countVl; j++)
  {
    k = (*entriesOfVlToBePreserved)[j];
    ul->m[lastUsableEntry+j+1] = vl->m[k];
    vl->m[k].rtyp = 0;
    vl->m[k].data = NULL;
  }
  
  /***
   * Now we delete of vl which is empty by now,
   * because its elements have either been deleted or moved to ul.
   **/
  vl->Clean();
  
  delete entriesOfUlToBeFilled;
  delete entriesOfVlToBePreserved;
  return;
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
      bigintmat* bim0 = (bigintmat*) v->Data();
      bigintmat* bim1 = bim0->transpose();
      gfan::ZMatrix* zm = bigintmatToZMatrix(bim1);
      gfan::ZCone* support = new gfan::ZCone;
      *support = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
      delete bim1;
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
        point = randomPoint(support);
        lambda = subcone(cones, point);
      }
      while (lambda.dimension() < lambda.ambientDimension());

      lambda.canonicalize();
      gfan::ZFan* Sigma = new gfan::ZFan(lambda.ambientDimension());
      Sigma->insert(lambda);
      int iterationNumber = 1;
      std::cout << "cones found: " << iterationNumber++ << std::endl;
      lists interiorFacets = listOfInteriorFacets(lambda, *support);
      if (interiorFacets->m[0].rtyp == INT_CMD)
      {
        res->rtyp = fanID;
        res->data = (void*) Sigma;
        return FALSE;
      }
      int mu = 1000; 

      gfan::ZCone* eta; gfan::ZVector* interiorPoint; gfan::ZVector* facetNormal;
      while (interiorFacets->nr > -1)
      {
        std::cout << "cones found: " << iterationNumber++ << std::endl;
        /***
         * Extract a facet to traverse and its relative interior point.
         **/
        lists cache = (lists) interiorFacets->m[interiorFacets->nr].Data();
        eta = (gfan::ZCone*) cache->m[0].Data();
        interiorPoint = (gfan::ZVector*) cache->m[1].Data();
        facetNormal = (gfan::ZVector*) cache->m[2].Data(); 

        /***
         * construct a point, which lies on the other side of the facet. 
         * make sure it lies in the known support of our fan
         * and that the cone around the point is maximal, containing eta.
         **/
        point = mu * (*interiorPoint) - (*facetNormal);
        while (!support->containsRelatively(point))
        {
          mu = mu * 10;
          point = mu * (*interiorPoint) - (*facetNormal);
        }

        lambda = subcone(cones,point);
        while ((lambda.dimension() < lambda.ambientDimension()) && !(lambda.contains(*interiorPoint)))
        {
          mu = mu * 10;
          point = mu * (*interiorPoint) - (*facetNormal);
          lambda = subcone(cones,point);
        }

        /*** 
         * insert lambda into Sigma, and create a list of facets of lambda. 
         * merge the two lists of facets
         **/
        lambda.canonicalize();
        Sigma->insert(lambda);
        lists newFacets = listOfInteriorFacets(lambda, *support);
        mergeListsOfFacets(interiorFacets, newFacets);

        if (interiorFacets == NULL)
          break;

        eta = NULL;
        interiorPoint = NULL;
        facetNormal = NULL;
        cache = NULL;
      }
      std::cout << "cones found: " << iterationNumber++ << std::endl;
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
