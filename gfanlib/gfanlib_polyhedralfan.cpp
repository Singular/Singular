/*
 * gfanlib_polyhedralfan.cpp
 *
 *  Created on: Nov 16, 2010
 *      Author: anders
 */

#include <sstream>
#include "gfanlib_polyhedralfan.h"
#include "gfanlib_polymakefile.h"

using namespace std;
namespace gfan{

PolyhedralFan::PolyhedralFan(int ambientDimension):
  n(ambientDimension),
  symmetries(n)
{
}

PolyhedralFan::PolyhedralFan(SymmetryGroup const &sym):
  n(sym.sizeOfBaseSet()),
  symmetries(sym)
{

}

PolyhedralFan PolyhedralFan::fullSpace(int n)
{
  PolyhedralFan ret(n);

  ZCone temp(n);
  temp.canonicalize();
  ret.cones.insert(temp);

  return ret;
}


PolyhedralFan PolyhedralFan::facetsOfCone(ZCone const &c)
{
  ZCone C(c);
  C.canonicalize();
  PolyhedralFan ret(C.ambientDimension());

  ZMatrix halfSpaces=C.getFacets();

  for(int i=0;i<halfSpaces.getHeight();i++)
    {
      ZMatrix a(0,C.ambientDimension());
      ZMatrix b(0,C.ambientDimension());
      b.appendRow(halfSpaces[i]);
      ZCone N=intersection(ZCone(a,b),c);
      N.canonicalize();
      ret.cones.insert(N);
    }
  return ret;
}


int PolyhedralFan::getAmbientDimension()const
{
  return n;
}

bool PolyhedralFan::isEmpty()const
{
  return cones.empty();
}

int PolyhedralFan::getMaxDimension()const
{
  assert(!cones.empty());

  return cones.begin()->dimension();
}

int PolyhedralFan::getMinDimension()const
{
  assert(!cones.empty());

  return cones.rbegin()->dimension();
}

/*
PolyhedralFan refinement(const PolyhedralFan &a, const PolyhedralFan &b, int cutOffDimension, bool allowASingleConeOfCutOffDimension)
{
  //  fprintf(Stderr,"PolyhedralFan refinement: #A=%i #B=%i\n",a.cones.size(),b.cones.size());
  int conesSkipped=0;
  int numberOfComputedCones=0;
  bool linealityConeFound=!allowASingleConeOfCutOffDimension;
  assert(a.getAmbientDimension()==b.getAmbientDimension());

  PolyhedralFan ret(a.getAmbientDimension());

  for(PolyhedralConeList::const_iterator A=a.cones.begin();A!=a.cones.end();A++)
    for(PolyhedralConeList::const_iterator B=b.cones.begin();B!=b.cones.end();B++)
      {
        PolyhedralCone c=intersection(*A,*B);
        int cdim=c.dimension();
        //      assert(cdim>=linealitySpaceDimension);
        bool thisIsLinealityCone=(cutOffDimension>=cdim);
        if((!thisIsLinealityCone)||(!linealityConeFound))
          {
            numberOfComputedCones++;
            c.canonicalize();
            ret.cones.insert(c);
            linealityConeFound=linealityConeFound || thisIsLinealityCone;
          }
        else
          {
            conesSkipped++;
          }
      }
  //  fprintf(Stderr,"Number of skipped cones: %i, lineality cone found: %i\n",conesSkipped,linealityConeFound);
  //  fprintf(Stderr,"Number of computed cones: %i, number of unique cones: %i\n",numberOfComputedCones,ret.cones.size());

  return ret;
}
*/

/*
PolyhedralFan product(const PolyhedralFan &a, const PolyhedralFan &b)
{
  PolyhedralFan ret(a.getAmbientDimension()+b.getAmbientDimension());

  for(PolyhedralConeList::const_iterator A=a.cones.begin();A!=a.cones.end();A++)
    for(PolyhedralConeList::const_iterator B=b.cones.begin();B!=b.cones.end();B++)
      ret.insert(product(*A,*B));

  return ret;
}
*/

/*IntegerVectorList PolyhedralFan::getRays(int dim)
{
  IntegerVectorList ret;
  for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();i++)
    {
      if(i->dimension()==dim)
        ret.push_back(i->getRelativeInteriorPoint());
    }
  return ret;
}
*/

/*IntegerVectorList PolyhedralFan::getRelativeInteriorPoints()
{
  IntegerVectorList ret;
  for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();i++)
    {
      ret.push_back(i->getRelativeInteriorPoint());
    }
  return ret;
}
*/

/*PolyhedralCone const& PolyhedralFan::highestDimensionalCone()const
{
  return *cones.rbegin();
}
*/
void PolyhedralFan::insert(ZCone const &c)
{
  ZCone temp=c;
  temp.canonicalize();
  cones.insert(temp);
}

void PolyhedralFan::remove(ZCone const &c)
{
  cones.erase(c);
}

/*
void PolyhedralFan::removeAllExcept(int a)
{
  PolyhedralConeList::iterator i=cones.begin();
  while(a>0)
    {
      if(i==cones.end())return;
      i++;
      a--;
    }
  cones.erase(i,cones.end());
}
*/

void PolyhedralFan::removeAllLowerDimensional()
{
  if(!cones.empty())
    {
      int d=getMaxDimension();
      PolyhedralConeList::iterator i=cones.begin();
      while(i!=cones.end() && i->dimension()==d)i++;
      cones.erase(i,cones.end());
    }
}


PolyhedralFan PolyhedralFan::facetComplex()const
{
  //  fprintf(Stderr,"Computing facet complex...\n");
  PolyhedralFan ret(n);

  for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();i++)
    {
      PolyhedralFan a=facetsOfCone(*i);
      for(PolyhedralConeList::const_iterator j=a.cones.begin();j!=a.cones.end();j++)
        ret.insert(*j);
    }
  //  fprintf(Stderr,"Done computing facet complex.\n");
  return ret;
}


/*
PolyhedralFan PolyhedralFan::fullComplex()const
{
  PolyhedralFan ret=*this;

  while(1)
    {
      log2 debug<<"looping";
      bool doLoop=false;
      PolyhedralFan facets=ret.facetComplex();
      log2 debug<<"number of facets"<<facets.size()<<"\n";
      for(PolyhedralConeList::const_iterator i=facets.cones.begin();i!=facets.cones.end();i++)
        if(!ret.contains(*i))
          {
            ret.insert(*i);
            doLoop=true;
          }
      if(!doLoop)break;
    }
  return ret;
}
*/


#if 0
PolyhedralFan PolyhedralFan::facetComplexSymmetry(SymmetryGroup const &sym, bool keepRays, bool dropLinealitySpace)const
{
  log1 fprintf(Stderr,"Computing facet complex...\n");
  PolyhedralFan ret(n);

  vector<IntegerVector> relIntTable;
  vector<int> dimensionTable;

  if(keepRays)
    for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();i++)
      if(i->dimension()==i->dimensionOfLinealitySpace()+1)
        {
          relIntTable.push_back(i->getRelativeInteriorPoint());
          dimensionTable.push_back(i->dimension());
          ret.insert(*i);
        }

  for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();i++)
    {
      int iDim=i->dimension();
      if(dropLinealitySpace && (i->dimension()==i->dimensionOfLinealitySpace()+1))continue;

      //      i->findFacets();
      IntegerVectorList normals=i->getHalfSpaces();
      for(IntegerVectorList::const_iterator j=normals.begin();j!=normals.end();j++)
        {
          bool alreadyInRet=false;
          for(int l=0;l<relIntTable.size();l++)
            {
              if(dimensionTable[l]==iDim-1)
                for(SymmetryGroup::ElementContainer::const_iterator k=sym.elements.begin();k!=sym.elements.end();k++)
                  {
                    IntegerVector u=SymmetryGroup::compose(*k,relIntTable[l]);
                    if((dotLong(*j,u)==0) && (i->contains(u)))
                      {
                        alreadyInRet=true;
                        goto exitLoop;
                      }
                  }
            }
        exitLoop:
          if(!alreadyInRet)
            {
              IntegerVectorList equations=i->getEquations();
              IntegerVectorList inequalities=i->getHalfSpaces();
              equations.push_back(*j);
              PolyhedralCone c(inequalities,equations,n);
              c.canonicalize();
              ret.insert(c);
              relIntTable.push_back(c.getRelativeInteriorPoint());
              dimensionTable.push_back(c.dimension());
            }
        }
    }
  log1 fprintf(Stderr,"Done computing facet complex.\n");
  return ret;
}
#endif



ZMatrix PolyhedralFan::getRaysInPrintingOrder(bool upToSymmetry)const
{
  /*
   * The ordering changed in this version. Previously the orbit representatives stored in "rays" were
   * just the first extreme ray from the orbit that appeared. Now it is gotten using "orbitRepresentative"
   * which causes the ordering in which the different orbits appear to change.
   */
  if(cones.empty())return ZMatrix(0,n);
ZMatrix generatorsOfLinealitySpace=cones.begin()->generatorsOfLinealitySpace();//all cones have the same lineality space


  std::set<ZVector> rays;//(this->getAmbientDimension());
//  log1 fprintf(Stderr,"Computing rays of %i cones\n",cones.size());
  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      ZMatrix temp=i->extremeRays(&generatorsOfLinealitySpace);
 //     std::cerr<<temp;
      for(int j=0;j<temp.getHeight();j++)
        rays.insert(symmetries.orbitRepresentative(temp[j]));
    }
  ZMatrix ret(0,getAmbientDimension());
  if(upToSymmetry)
    for(set<ZVector>::const_iterator i=rays.begin();i!=rays.end();i++)ret.appendRow(*i);
  else
    for(set<ZVector>::const_iterator i=rays.begin();i!=rays.end();i++)
      {
        set<ZVector> thisOrbitsRays;
        for(SymmetryGroup::ElementContainer::const_iterator k=symmetries.elements.begin();k!=symmetries.elements.end();k++)
          {
            ZVector temp=k->apply(*i);
            thisOrbitsRays.insert(temp);
          }

        for(set<ZVector>::const_iterator i=thisOrbitsRays.begin();i!=thisOrbitsRays.end();i++)ret.appendRow(*i);
      }
  return ret;
}



/*MARKS CONES AS NONMAXIMAL IN THE SYMMETRIC COMPLEX IN WHICH THEY WILL BE INSERTED -not to be confused with the facet testing in the code
   */
 static list<SymmetricComplex::Cone> computeFacets(SymmetricComplex::Cone const &theCone, ZMatrix const &rays, ZMatrix const &facetCandidates, SymmetricComplex const &theComplex/*, int linealityDim*/)
{
  set<SymmetricComplex::Cone> ret;

  for(int i=0;i<facetCandidates.getHeight();i++)
    {
      set<int> indices;

      bool notAll=false;
      for(unsigned j=0;j<theCone.indices.size();j++)
        if(dot(rays[theCone.indices[j]],facetCandidates[i]).sign()==0)
          indices.insert(theCone.indices[j]);
        else
          notAll=true;

      SymmetricComplex::Cone temp(indices,theCone.dimension-1,Integer(),false,theComplex);
      /*      temp.multiplicity=0;
      temp.dimension=theCone.dimension-1;
      temp.setIgnoreSymmetry(true);
      */
      if(notAll)ret.insert(temp);

    }
  //  fprintf(Stderr,"HEJ!!!!\n");

  list<SymmetricComplex::Cone> ret2;
  for(set<SymmetricComplex::Cone>::const_iterator i=ret.begin();i!=ret.end();i++)
    {
      bool isMaximal=true;

      /*      if(i->indices.size()+linealityDim<i->dimension)//#3
        isMaximal=false;
        else*/
        for(set<SymmetricComplex::Cone>::const_iterator j=ret.begin();j!=ret.end();j++)
          if(i!=j && i->isSubsetOf(*j))
            {
              isMaximal=false;
              break;
            }
      if(isMaximal)
        {
          SymmetricComplex::Cone temp(i->indexSet(),i->dimension,i->multiplicity,true,theComplex);
          temp.setKnownToBeNonMaximal(); // THIS IS WHERE WE SET THE CONES NON-MAXIMAL FLAG
          //      temp.setIgnoreSymmetry(false);
          ret2.push_back(temp);
        }
    }
  return ret2;
}

void addFacesToSymmetricComplex(SymmetricComplex &c, ZCone const &cone, ZMatrix const &facetCandidates, ZMatrix const &generatorsOfLinealitySpace)
{
  // ZMatrix const &rays=c.getVertices();
  std::set<int> indices;

  // for(int j=0;j<rays.getHeight();j++)if(cone.contains(rays[j]))indices.insert(j);

  ZMatrix l=cone.extremeRays(&generatorsOfLinealitySpace);
  for(int i=0;i<l.getHeight();i++)indices.insert(c.indexOfVertex(l[i]));

  addFacesToSymmetricComplex(c,indices,facetCandidates,cone.dimension(),cone.getMultiplicity());
}

void addFacesToSymmetricComplex(SymmetricComplex &c, std::set<int> const &indices, ZMatrix const &facetCandidates, int dimension, Integer multiplicity)
{
  ZMatrix const &rays=c.getVertices();
  list<SymmetricComplex::Cone> clist;
  {
    SymmetricComplex::Cone temp(indices,dimension,multiplicity,true,c);
    //    temp.dimension=cone.dimension();
    //   temp.multiplicity=cone.getMultiplicity();
    clist.push_back(temp);
  }

  //  int linealityDim=cone.dimensionOfLinealitySpace();

  while(!clist.empty())
    {
      SymmetricComplex::Cone &theCone=clist.front();

      if(!c.contains(theCone))
        {
          c.insert(theCone);
          list<SymmetricComplex::Cone> facets=computeFacets(theCone,rays,facetCandidates,c/*,linealityDim*/);
          clist.splice(clist.end(),facets);
        }
      clist.pop_front();
    }

}

#if 0
/**
   Produce strings that express the vectors in terms of rays of the fan modulo the lineality space. Symmetry is ignored??
 */
vector<string> PolyhedralFan::renamingStrings(IntegerVectorList const &theVectors, IntegerVectorList const &originalRays, IntegerVectorList const &linealitySpace, SymmetryGroup *sym)const
{
  vector<string> ret;
  for(IntegerVectorList::const_iterator i=theVectors.begin();i!=theVectors.end();i++)
    {
      for(PolyhedralConeList::const_iterator j=cones.begin();j!=cones.end();j++)
        {
          if(j->contains(*i))
            {
              vector<int> relevantIndices;
              IntegerVectorList relevantRays=linealitySpace;
              int K=0;
              for(IntegerVectorList::const_iterator k=originalRays.begin();k!=originalRays.end();k++,K++)
                if(j->contains(*k))
                  {
                    relevantIndices.push_back(K);
                    relevantRays.push_back(*k);
                  }

              FieldMatrix LFA(Q,relevantRays.size(),n);
              int J=0;
              for(IntegerVectorList::const_iterator j=relevantRays.begin();j!=relevantRays.end();j++,J++)
                LFA[J]=integerVectorToFieldVector(*j,Q);
              FieldVector LFB=concatenation(integerVectorToFieldVector(*i,Q),FieldVector(Q,relevantRays.size()));
              LFA=LFA.transposed();
              FieldVector LFX=LFA.solver().canonicalize(LFB);
              stringstream s;
              if(LFX.subvector(0,n).isZero())
                {
                  s<<"Was:";
                  FieldVector S=LFX.subvector(n+linealitySpace.size(),LFX.size());
                  for(int k=0;k<S.size();k++)
                    if(!S[k].isZero())
                      s<<"+"<<S[k].toString()<<"*["<<relevantIndices[k]<<"] ";
                }
              ret.push_back(s.str());
              break;
            }
        }
    }
  return ret;
}
#endif

SymmetricComplex PolyhedralFan::toSymmetricComplex()const
{

          ZMatrix rays=getRaysInPrintingOrder();

          ZMatrix generatorsOfLinealitySpace=cones.empty()?ZMatrix::identity(getAmbientDimension()):cones.begin()->generatorsOfLinealitySpace();
          SymmetricComplex symCom(rays,generatorsOfLinealitySpace,symmetries);

          for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
            {
              addFacesToSymmetricComplex(symCom,*i,i->getFacets(),generatorsOfLinealitySpace);
            }

//          log1 cerr<<"Remapping";
          symCom.remap();
//          log1 cerr<<"Done remapping";

          return symCom;
}

std::string PolyhedralFan::toString(int flags)const
//void PolyhedralFan::printWithIndices(class Printer *p, bool printMultiplicities, SymmetryGroup *sym, bool group, bool ignoreCones, bool xml, bool tPlaneSort, vector<string> const *comments)const
{
  stringstream ret;

  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      ret<<"Cone\n"<<endl;
    ret<<*i;
    }  return ret.str();
#if 0
  PolymakeFile polymakeFile;
  polymakeFile.create("NONAME","PolyhedralFan","PolyhedralFan",flags&FPF_xml);

//  if(!sym)sym=&symm;

  if(cones.empty())
    {
//      p->printString("Polyhedral fan is empty. Printing not supported.\n");
      ret<<"Polyhedral fan is empty. Printing not supported.\n";
      return ret.str();
    }

  int h=cones.begin()->dimensionOfLinealitySpace();

//  log1 fprintf(Stderr,"Computing rays.\n");
  ZMatrix rays=getRaysInPrintingOrder();

  SymmetricComplex symCom(rays,cones.begin()->generatorsOfLinealitySpace(),symmetries);

  polymakeFile.writeCardinalProperty("AMBIENT_DIM",n);
  polymakeFile.writeCardinalProperty("DIM",getMaxDimension());
  polymakeFile.writeCardinalProperty("LINEALITY_DIM",h);
  polymakeFile.writeMatrixProperty("RAYS",rays,true,comments);
  polymakeFile.writeCardinalProperty("N_RAYS",rays.size());
  IntegerVectorList linealitySpaceGenerators=highestDimensionalCone().linealitySpace().dualCone().getEquations();
  polymakeFile.writeMatrixProperty("LINEALITY_SPACE",rowsToIntegerMatrix(linealitySpaceGenerators,n));
  polymakeFile.writeMatrixProperty("ORTH_LINEALITY_SPACE",rowsToIntegerMatrix(highestDimensionalCone().linealitySpace().getEquations(),n));

  if(flags & FPF_primitiveRays)
  {
         ZMatrix primitiveRays;
         for(IntegerVectorList::const_iterator i=rays.begin();i!=rays.end();i++)
                 for(PolyhedralConeList::const_iterator j=cones.begin();j!=cones.end();j++)
                         if(j->contains(*i)&&(j->dimensionOfLinealitySpace()+1==j->dimension()))
                                         primitiveRays.push_back(j->semiGroupGeneratorOfRay());

          polymakeFile.writeMatrixProperty("PRIMITIVE_RAYS",rowsToIntegerMatrix(primitiveRays,n));
  }


  ZMatrix generatorsOfLinealitySpace=cones.begin()->generatorsOfLinealitySpace();

  log1 fprintf(Stderr,"Building symmetric complex.\n");
  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      {
        static int t;
//        log1 fprintf(Stderr,"Adding faces of cone %i\n",t++);
      }
//      log2 fprintf(Stderr,"Dim: %i\n",i->dimension());

      addFacesToSymmetricComplex(symCom,*i,i->getHalfSpaces(),generatorsOfLinealitySpace);
    }

//  log1 cerr<<"Remapping";
  symCom.remap();
//  log1 cerr<<"Done remapping";


  PolyhedralFan f=*this;

//  log1 fprintf(Stderr,"Computing f-vector.\n");
  ZVector fvector=symCom.fvector();
  polymakeFile.writeCardinalVectorProperty("F_VECTOR",fvector);
//  log1 fprintf(Stderr,"Done computing f-vector.\n");

  if(flags&FPF_boundedInfo)
    {
//      log1 fprintf(Stderr,"Computing bounded f-vector.\n");
      ZVector fvectorBounded=symCom.fvector(true);
      polymakeFile.writeCardinalVectorProperty("F_VECTOR_BOUNDED",fvectorBounded);
//      log1 fprintf(Stderr,"Done computing bounded f-vector.\n");
    }
  {
    Integer euler;
    int mul=-1;
    for(int i=0;i<fvector.size();i++,mul*=-1)euler+=Integer(mul)*fvector[i];
    polymakeFile.writeCardinalProperty("MY_EULER",euler);
  }

//  log1 fprintf(Stderr,"Checking if complex is simplicial and pure.\n");
  polymakeFile.writeCardinalProperty("SIMPLICIAL",symCom.isSimplicial());
  polymakeFile.writeCardinalProperty("PURE",symCom.isPure());
//  log1 fprintf(Stderr,"Done checking.\n");


  if(flags&FPF_conesCompressed)
  {
//    log1 fprintf(Stderr,"Producing list of cones up to symmetry.\n");
    polymakeFile.writeStringProperty("CONES_ORBITS",symCom.toString(symCom.getMinDim(),symCom.getMaxDim(),false,flags&FPF_group,0,true,flags&FPF_tPlaneSort));
//    log1 fprintf(Stderr,"Done producing list of cones up to symmetry.\n");
//    log1 fprintf(Stderr,"Producing list of maximal cones up to symmetry.\n");
    stringstream multiplicities;
    polymakeFile.writeStringProperty("MAXIMAL_CONES_ORBITS",symCom.toString(symCom.getMinDim(),symCom.getMaxDim(),true,flags&FPF_group, &multiplicities,true,flags&FPF_tPlaneSort));
    if(flags&FPF_multiplicities)polymakeFile.writeStringProperty("MULTIPLICITIES_ORBITS",multiplicities.str());
//    log1 fprintf(Stderr,"Done producing list of maximal cones up to symmetry.\n");
  }

  if(flags&FPF_conesExpanded)
    {
      if(flags&FPF_cones)
        {
//          log1 fprintf(Stderr,"Producing list of cones.\n");
          polymakeFile.writeStringProperty("CONES",symCom.toString(symCom.getMinDim(),symCom.getMaxDim(),false,flags&FPF_group,0,false,flags&FPF_tPlaneSort));
//          log1 fprintf(Stderr,"Done producing list of cones.\n");
        }
      if(flags&FPF_maximalCones)
        {
//          log1 fprintf(Stderr,"Producing list of maximal cones.\n");
          stringstream multiplicities;
          polymakeFile.writeStringProperty("MAXIMAL_CONES",symCom.toString(symCom.getMinDim(),symCom.getMaxDim(),true,flags&FPF_group, &multiplicities,false,flags&FPF_tPlaneSort));
          if(flags&FPF_multiplicities)polymakeFile.writeStringProperty("MULTIPLICITIES",multiplicities.str());
//          log1 fprintf(Stderr,"Done producing list of maximal cones.\n");
        }
    }
#endif
  #if 0
  if(flags&FPF_values)
    {
      {
        ZMatrix values;
        for(int i=0;i<linealitySpaceGenerators.getHeight();i++)
          {
            ZVector v(1);
            v[0]=evaluatePiecewiseLinearFunction(linealitySpaceGenerators[i]);
            values.appendRow(v);
          }
        polymakeFile.writeMatrixProperty("LINEALITY_VALUES",rowsToIntegerMatrix(values,1));
      }
      {
        ZMatrix values;
        for(IntegerVectorList::const_iterator i=rays.begin();i!=rays.end();i++)
          {
            ZVector v(1);
            v[0]=evaluatePiecewiseLinearFunction(*i);
            values.push_back(v);
          }
        polymakeFile.writeMatrixProperty("RAY_VALUES",rowsToIntegerMatrix(values,1));
      }
    }
#endif


//  log1 fprintf(Stderr,"Producing final string for output.\n");
/*  stringstream s;
  polymakeFile.writeStream(s);
  string S=s.str();
//  log1 fprintf(Stderr,"Printing string.\n");
  p->printString(S.c_str());
*///  log1 fprintf(Stderr,"Done printing string.\n");
}

#if 0
PolyhedralFan PolyhedralFan::readFan(string const &filename, bool onlyMaximal, IntegerVector *w, set<int> const *coneIndices, SymmetryGroup const *sym, bool readCompressedIfNotSym)
{
    PolymakeFile inFile;
    inFile.open(filename.c_str());

    int n=inFile.readCardinalProperty("AMBIENT_DIM");
    int nRays=inFile.readCardinalProperty("N_RAYS");
    IntegerMatrix rays=inFile.readMatrixProperty("RAYS",nRays,n);
    int linealityDim=inFile.readCardinalProperty("LINEALITY_DIM");
    IntegerMatrix linealitySpace=inFile.readMatrixProperty("LINEALITY_SPACE",linealityDim,n);


    const char *sectionName=0;
    const char *sectionNameMultiplicities=0;
    if(sym || readCompressedIfNotSym)
    {
      sectionName=(onlyMaximal)?"MAXIMAL_CONES_ORBITS":"CONES_ORBITS";
      sectionNameMultiplicities=(onlyMaximal)?"MULTIPLICITIES_ORBITS":"DUMMY123";
    }
      else
      {  sectionName=(onlyMaximal)?"MAXIMAL_CONES":"CONES";
      sectionNameMultiplicities=(onlyMaximal)?"MULTIPLICITIES":"DUMMY123";
      }


    IntegerVector w2(n);
    if(w==0)w=&w2;

    SymmetryGroup sym2(n);
    if(sym==0)sym=&sym2;

    vector<list<int> > cones=inFile.readMatrixIncidenceProperty(sectionName);
    IntegerVectorList r;

    bool hasMultiplicities=inFile.hasProperty(sectionNameMultiplicities);
    IntegerMatrix multiplicities(0,0);
    if(hasMultiplicities)multiplicities=inFile.readMatrixProperty(sectionNameMultiplicities,cones.size(),1);


    PolyhedralFan ret(n);

    log2 cerr<< "Number of orbits to expand "<<cones.size()<<endl;
    for(int i=0;i<cones.size();i++)
      if(coneIndices==0 || coneIndices->count(i))
        {
          log2 cerr<<"Expanding symmetries of cone"<<endl;
          {
            IntegerVectorList coneRays;
            for(list<int>::const_iterator j=cones[i].begin();j!=cones[i].end();j++)
              coneRays.push_back((rays[*j]));
            PolyhedralCone C=PolyhedralCone::givenByRays(coneRays,linealitySpace.getRows(),n);
            if(hasMultiplicities)C.setMultiplicity(multiplicities[i][0]);
            for(SymmetryGroup::ElementContainer::const_iterator perm=sym->elements.begin();perm!=sym->elements.end();perm++)
              {
                if(C.contains(SymmetryGroup::composeInverse(*perm,*w)))
                  {
                    PolyhedralCone C2=C.permuted(*perm);
                    C2.canonicalize();
                    ret.insert(C2);
                  }
              }
          }
        }
    return ret;
}
#endif

#if 0
IncidenceList PolyhedralFan::getIncidenceList(SymmetryGroup *sym)const //fan must be pure
{
  IncidenceList ret;
  SymmetryGroup symm(n);
  if(!sym)sym=&symm;
  assert(!cones.empty());
  int h=cones.begin()->dimensionOfLinealitySpace();
  IntegerVectorList rays=getRaysInPrintingOrder(sym);
  PolyhedralFan f=*this;

  while(f.getMaxDimension()!=h)
    {
      IntegerVectorList l;
      PolyhedralFan done(n);
      IntegerVector rayIncidenceCounter(rays.size());
      int faceIndex =0;
      for(PolyhedralConeList::const_iterator i=f.cones.begin();i!=f.cones.end();i++)
        {
          if(!done.contains(*i))
            {
              for(SymmetryGroup::ElementContainer::const_iterator k=sym->elements.begin();k!=sym->elements.end();k++)
                {
                  PolyhedralCone cone=i->permuted(*k);
                  if(!done.contains(cone))
                    {
                      int rayIndex=0;
                      IntegerVector indices(0);
                      for(IntegerVectorList::const_iterator j=rays.begin();j!=rays.end();j++)
                        {
                          if(cone.contains(*j))
                            {
                              indices.grow(indices.size()+1);
                              indices[indices.size()-1]=rayIndex;
                              rayIncidenceCounter[rayIndex]++;
                            }
                          rayIndex++;
                        }
                      l.push_back(indices);
                      faceIndex++;
                      done.insert(cone);
                    }
                }
            }
        }
      ret[f.getMaxDimension()]=l;
      f=f.facetComplex();
    }
  return ret;
}
#endif

void PolyhedralFan::makePure()
{
  if(getMaxDimension()!=getMinDimension())removeAllLowerDimensional();
}

bool PolyhedralFan::contains(ZCone const &c)const
{
  return cones.count(c);
}


#if 0
PolyhedralCone PolyhedralFan::coneContaining(ZVector const &v)const
{
  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    if(i->contains(v))return i->faceContaining(v);
  debug<<"Vector "<<v<<" not contained in support of fan\n";
  assert(0);
}
#endif

PolyhedralFan::coneIterator PolyhedralFan::conesBegin()const
{
  return cones.begin();
}


PolyhedralFan::coneIterator PolyhedralFan::conesEnd()const
{
  return cones.end();
}



PolyhedralFan PolyhedralFan::link(ZVector const &w, SymmetryGroup *sym)const
{
  SymmetryGroup symL(n);
  if(!sym)sym=&symL;

  PolyhedralFan ret(n);

  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      for(SymmetryGroup::ElementContainer::const_iterator perm=sym->elements.begin();perm!=sym->elements.end();perm++)
        {
          ZVector w2=perm->applyInverse(w);
          if(i->contains(w2))
            {
              ret.insert(i->link(w2));
            }
        }
    }
  return ret;
}

PolyhedralFan PolyhedralFan::link(ZVector const &w)const
{
  PolyhedralFan ret(n);

  for(PolyhedralConeList::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      if(i->contains(w))
        {
          ret.insert(i->link(w));
        }
    }
  return ret;
}


int PolyhedralFan::size()const
{
  return cones.size();
}

int PolyhedralFan::dimensionOfLinealitySpace()const
{
  assert(cones.size());//slow!
  return cones.begin()->dimensionOfLinealitySpace();
}




void PolyhedralFan::removeNonMaximal()
{
  for(PolyhedralConeList::iterator i=cones.begin();i!=cones.end();)
    {
      ZVector w=i->getRelativeInteriorPoint();
      bool containedInOther=false;
      for(PolyhedralConeList::iterator j=cones.begin();j!=cones.end();j++)
        if(j!=i)
          {
            if(j->contains(w)){containedInOther=true;break;}
          }
      if(containedInOther)
        {
          PolyhedralConeList::iterator k=i;
          i++;
          cones.erase(k);
        }
      else i++;
    }
}


}
