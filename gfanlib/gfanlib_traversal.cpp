#include <stddef.h>
#include "gfanlib_traversal.h"
#include "gfanlib_symmetry.h"

#include <map>
#include <algorithm>
#include <iostream>

//#include "log.h"

using namespace std;
using namespace gfan;


static list<ZVector> rowsToList(ZMatrix const &m)
                {
                        list<ZVector> ret;
                        for(int i=0;i<m.getHeight();i++)
                                ret.push_back(m[i]);
                        return ret;
                }

bool FanTraverser::hasNoState()const
{
  return false;
}


/**
 * FanBuilder
 */

FanBuilder::FanBuilder(int n, SymmetryGroup const &sym):
        coneCollection(n)
{
}


bool FanBuilder::process(FanTraverser &traverser)
{
        ZCone cone2=traverser.refToPolyhedralCone();
        cone2.canonicalize();
        coneCollection.insert(cone2);
        return true;
}




/**
 * Classes
 */

/** The hypergraph of ridges and facets can be considered as a usual
    bipartite graph where the right nodes are the ridges and the left
    nodes are the facets.  We wish to make a traversal of this
    bipartite graph keeping track of the boundary edges of the
    traversed set. The ConeOrbit object represents the orbit of a
    ridge. The edges of the ridge are listed but only those which
    belong to the boundary of the set of ridges seen so far. When a
    ridge is discovered the ConeOrbit object will be created with all
    its edges present (except the one it was reached by). As progress
    in the computation is made these edges will be deleted.
 */


class Boundary
{
  typedef pair<ZVector,ZVector> EFirst;
  struct ESecond{// L2 maybe zero, in that case i1==i2
    list<ZVector>* L1;
    list<ZVector>::iterator i1;
    list<ZVector> *L2;
    list<ZVector>::iterator i2;
    ESecond():L1(0),L2(0){};

      ESecond(list<ZVector>* L1_,list<ZVector>::iterator i1_,list<ZVector>* L2_,list<ZVector>::iterator i2_):
      L1(L1_),
      i1(i1_),
      L2(L2_),
      i2(i2_)
    {
    }
  };
  SymmetryGroup const &sym;
  map<EFirst,ESecond > theSet;
  int theSetSize;
public:
  Boundary(SymmetryGroup const &sym_):
    sym(sym_),
    theSetSize(0)
  {
  }
  int size()const
  {
    return theSetSize;
  }
  pair<ZVector,ZVector> normalForm(ZVector const &ridge, ZVector const &ray)const
  {
    pair<ZVector,ZVector> ret;
    Permutation perm(ridge.size());
    ret.first=sym.orbitRepresentative(ridge,&perm);
    ret.second=sym.orbitRepresentativeFixing(perm.apply(ray),ret.first);
    return ret;
  }
  bool containsFlip(ZVector const &ridge, ZVector const &ray, list<ZVector> *storedInList_, list<ZVector>::iterator listIterator_, list<ZVector> *storedInList2_, list<ZVector>::iterator listIterator2_)
  {
    assert(ridge.size()==ray.size());
    EFirst p=normalForm(ridge,ray);
    if(theSet.count(p)==1)
      {
        theSet[p].L1->erase(theSet[p].i1);
        if(theSet[p].L2)theSet[p].L2->erase(theSet[p].i2);
        theSet.erase(p);
        theSetSize--;
        return true;
      }
    theSet[p]=ESecond(storedInList_,listIterator_,storedInList2_,listIterator2_);
    theSetSize++;
    return false;
  }
  /**
   * This routine remove rays from rays, such that only one ridge-ray pair is left for each orbit.
   * The routine allows an additional list of vectors with the same number of elements as rays to be passed.
   * The routine will remove those vectors from this set which correspond to rays removed from rays.
   *
   * To do this it must know the symmetry group.
   */
  void removeDuplicates(ZVector const &ridge, list<ZVector> &rays, list<ZVector> *normals=0)const
  {
          list<ZVector> ret;
          list<ZVector> normalsRet;
          set<ZVector> representatives;
          list<ZVector>::const_iterator I;
    if(normals)I=normals->begin();
    for(list<ZVector>::const_iterator i=rays.begin();i!=rays.end();i++)
      {
        ZVector rep=sym.orbitRepresentativeFixing(*i,ridge);
        if(representatives.count(rep)==0)
          {
            representatives.insert(rep);
            ret.push_back(*i);
            if(normals)normalsRet.push_back(*I);
          }
        if(normals)I++;
      }
    rays=ret;
    if(normals)*normals=normalsRet;
  }
  void print()const
  {
    cerr<< "Boundary" <<endl;
    for(map<EFirst,ESecond>::const_iterator i=theSet.begin();i!=theSet.end();i++)
      {
            cerr << i->first.first << i->first.second;
            cerr << endl;
      }
    cerr<<endl<<endl;
  }
};

/**
   Rewrite these comments.

   During traversal the path from the current facet to the starting
   facet is stored on a stack. The elements of the stack are objects
   of the class pathStep. The top element on the stack tells through
   which ridge the current facet was reached. This is done by the
   value parent ridge which is the unique ray of the ridge.  In order
   not to recompute the ridge the path facet contains rays of the link
   of the ridge represented by their unique vector. - or rather only
   the rays that are yet to be processed are stored in ridgeRays. In
   order to trace the path back the unique point of the ray from which
   the ridge was reached is stored in parentRay.
 */
struct pathStepRidge
{
  ZVector parentRidge;
  list<ZVector> rays;
  ZVector parentRay;
};

struct pathStepFacet
{
        list<ZVector> ridges;
        list<ZVector> ridgesRayUniqueVector;//stores the ray of the link that we came from
};

/**
  We need to simulate two mutually recursive functions. An actual
  implementation of these two functions would probably not work since
  the recursion depth could easily be 10000.

  Here follows a sketch of the simulation

lav kegle
find ridges
skriv ned i objekt
put paa stakken

L1:
if ridges in top element
  compute tropical curve
  construct stak object with rays; set parrentRidge,ridgeRays
  push ridge
else
  pop facet
  if empty break;

goto L2

L2:
if(ridgeRays not empty)
   change CONE
   <---entry point
   push facet
else
  pop ridge
  change CONE

goto L1


The strategy for marking is as follows Before a vertex is pushed the
edges that needs to be taken are written in its data. A edge is only
written if its orbit has not been marked. Each time an edge is written
it is simultaneously marked.

*/



static void printStack(list<pathStepFacet> const &facetStack, list<pathStepRidge> const &ridgeStack)
{
  list<pathStepFacet>::const_iterator i=facetStack.begin();
  list<pathStepRidge>::const_iterator j=ridgeStack.begin();
  cerr<<"STACK:"<<endl;
  if(facetStack.size()>ridgeStack.size())goto entry;
  do
    {
      cerr<<"RIDGE:"<<endl;
//      cerr<<j->parentRidge<<j->rays<<j->parentRay;
      cerr<<endl;

      j++;
    entry:
      cerr<<"FACET:"<<endl;
//      cerr<<i->ridges;
      cerr<<endl;
      i++;
    }
  while(i!=facetStack.end());

  int a;
  //cin >> a;
}



void traverse(FanTraverser &traverser, Target &target, SymmetryGroup const *symmetryGroup)
{
  //TO DO: at the moment a conetraverser can only report that it has no state if it is traversing a complete fan.
  //This is because symmetricTraverse needs go BACK to compute the links of previously seen facets.
  //Alternative the links should be computed and stored the first time a facet is seen.
  //Or the conetraverse should be given more info about the ridge to make computations quicker.
        int lastNumberOfEdges=0;
        float averageEdge=0;
        int n=traverser.refToPolyhedralCone().ambientDimension();//symmetryGroup->sizeOfBaseSet();
        SymmetryGroup localSymmetryGroup(n);
        if(!symmetryGroup)symmetryGroup=&localSymmetryGroup;

        ZMatrix linealitySpaceGenerators=traverser.refToPolyhedralCone().generatorsOfLinealitySpace();

        int d=traverser.refToPolyhedralCone().dimension();

          Boundary boundary(*symmetryGroup);
          list<pathStepFacet> facetStack;
          list<pathStepRidge> ridgeStack;

          int numberOfCompletedFacets=0;
          int numberOfCompletedRidges=0;
          int stackSize=0;

          ZVector facetUniqueVector;
          goto entry;
          while(1)
            {
            L1:
//            printStack(facetStack,ridgeStack);
            //if we have more ProcessRidge calls to do
              if(!facetStack.front().ridges.empty())
                {
                      //ProcessRidge "called"
                  pathStepRidge top;


                  if(traverser.hasNoState())
                    top.parentRay=facetStack.front().ridgesRayUniqueVector.front();
                  else
                    {
                      ZCone link=traverser.refToPolyhedralCone().link(facetStack.front().ridges.front());
                      link.canonicalize();
                      top.parentRay=link.getUniquePoint();
                    }

                  top.parentRidge=facetStack.front().ridges.front();
//                  AsciiPrinter(Stderr)<<top.parentRay<<"--------------------------------++++\n";
                  list<ZVector> rays;
                  if(traverser.hasNoState())
                    {
                      rays.push_back(top.parentRay);
                      rays.push_back(-top.parentRay);
                    }
                  else
                    rays=traverser.link(facetStack.front().ridges.front());

                  assert(!rays.empty());
                  boundary.removeDuplicates(top.parentRidge,rays);
                  ridgeStack.push_front(top);stackSize++;
                  ZVector ridgeRidgeRidge=facetStack.front().ridges.front();
                  for(list<ZVector>::const_iterator i=rays.begin();i!=rays.end();i++)
                    {
                      ridgeStack.front().rays.push_front(*i);
                      if(boundary.containsFlip(ridgeRidgeRidge,*i,&ridgeStack.front().rays,ridgeStack.front().rays.begin(),0,ridgeStack.front().rays.begin()))
                        ridgeStack.front().rays.pop_front();
                    }
                  // "state saved" ready to do calls to ProcessFacet
                  numberOfCompletedRidges++;
                }
              else
                {
                      // No more calls to do - we now return from ProcessFacet
                      //THIS IS THE PLACE TO CHANGE CONE BACK
                  facetStack.pop_front();stackSize--;
                  if(facetStack.empty())break;
//             log1 cerr<<"BACK"<<endl;
          if(!traverser.hasNoState())traverser.changeCone(ridgeStack.front().parentRidge,ridgeStack.front().parentRay);
                }
            L2:
//            printStack(facetStack,ridgeStack);
            //check if ProcessRidge needs to make more ProcessFacet calls
              if(!ridgeStack.front().rays.empty())
                {
//                      log1 cerr<<"FORWARD"<<endl;
                      traverser.changeCone(ridgeStack.front().parentRidge,ridgeStack.front().rays.front());
                entry:
                //ProcessFacet()
                averageEdge=0.99*averageEdge+0.01*(boundary.size()-lastNumberOfEdges);
//                log1 fprintf(Stderr,"\n-------------------------------------\n");
//                  log1 fprintf(Stderr,"Boundary edges in bipartite graph: %i, Completed ridges: %i, Completed facets: %i, Recursion depth:%i Average new edge/facet:%0.2f\n",boundary.size(),numberOfCompletedRidges,numberOfCompletedFacets,stackSize,averageEdge);
//                  log1 fprintf(Stderr,"-------------------------------------\n");
                  lastNumberOfEdges=boundary.size();

//                  target.process(traverser);//Postponed until extrem rays have been computed
                  ZMatrix extremeRays=traverser.refToPolyhedralCone().extremeRays(&linealitySpaceGenerators);
                  target.process(traverser);

//                  IntegerVectorList inequalities=traverser.refToPolyhedralCone().getHalfSpaces();
                  ZMatrix equations=traverser.refToPolyhedralCone().getEquations();
//                  facetUniqueVector=traverser.refToPolyhedralCone().getUniquePoint();
                  facetUniqueVector=traverser.refToPolyhedralCone().getUniquePointFromExtremeRays(extremeRays);
                  list<ZVector> facetNormals=rowsToList(traverser.refToPolyhedralCone().getFacets());

                  pathStepFacet stepFacet;
                  list<ZVector> ridges;

                  for(list<ZVector>::iterator i=facetNormals.begin();i!=facetNormals.end();i++)
                    {
                          ZVector v(n);
//                          for(IntegerVectorList::const_iterator j=extremeRays.begin();j!=extremeRays.end();j++)
                          for(int j=0;j<extremeRays.getHeight();j++)
                                  if(dot(*i,extremeRays[j]).isZero())v+=extremeRays[j];
                          ridges.push_back(v);
                    }

                  ZVector temp(n);
//                  boundary.removeDuplicates(temp,ridges);//use facetUniqueVector instead
                  boundary.removeDuplicates(facetUniqueVector,ridges,&facetNormals);//use facetUniqueVector instead

                  facetStack.push_front(stepFacet);stackSize++;
                  list<ZVector>::const_iterator I=facetNormals.begin();
                  for(list<ZVector>::const_iterator i=ridges.begin();i!=ridges.end();i++,I++)
                    {
                          ZVector rayUniqueVector;

                          if(d==n)
                          {
                                rayUniqueVector =I->normalized();
//                                if(dotLong(rayUniqueVector,*I)
                          }
                          else
                          {
                                  ZCone rayCone=traverser.refToPolyhedralCone().link(*i);
                                  rayCone.canonicalize();
                                  rayUniqueVector=rayCone.getUniquePoint();
//                                  debug<<traverser.refToPolyhedralCone();
//                                  debug<<rayCone;
                          }
                      facetStack.front().ridges.push_front(*i);
                      if(traverser.hasNoState())facetStack.front().ridgesRayUniqueVector.push_front(rayUniqueVector);


                      if(!traverser.hasNoState())
                        {
                        if(boundary.containsFlip(*i,rayUniqueVector,&facetStack.front().ridges,facetStack.front().ridges.begin(),0,facetStack.front().ridges.begin()))
                        {
                          facetStack.front().ridges.pop_front();
                        }
                        }
                      else
                        {
                      if(boundary.containsFlip(*i,rayUniqueVector,&facetStack.front().ridges,facetStack.front().ridges.begin(),&facetStack.front().ridgesRayUniqueVector,facetStack.front().ridgesRayUniqueVector.begin()))
                        {
                          facetStack.front().ridges.pop_front();
                          facetStack.front().ridgesRayUniqueVector.pop_front();
                        }
                        }
                    }
                  //"State pushed" ready to call ProcessRidge

                  numberOfCompletedFacets++;
                }
              else
                {
                      //ProcessRidge is done making its calls to ProcessFacet so we can return from ProcessRidge
//                      cerr<<"BACK"<<endl;
//                  traverser.changeCone(ridgeStack.front().parentRidge,ridgeStack.front().parentRay);
                  ridgeStack.pop_front();stackSize--;
                }
            }//goto L1
          //          log1 fprintf(Stderr,"\n-------------------------------------\n");
//          log1 fprintf(Stderr,"Boundary edges in bipartite graph: %i, Completed ridges: %i, Completed facets: %i, Recursion depth:%i\n",boundary.size(),numberOfCompletedRidges,numberOfCompletedFacets,stackSize);
//          log1 fprintf(Stderr,"-------------------------------------\n");
}
