/*
 * gfanlib_symmetriccomplex.cpp
 *
 *  Created on: Nov 16, 2010
 *      Author: anders
 */

#include <stddef.h>
#include "gfanlib_symmetriccomplex.h"
#include "gfanlib_polymakefile.h"

#include <sstream>
#include <iostream>

namespace gfan{

SymmetricComplex::Cone::Cone(std::set<int> const &indices_, int dimension_, Integer multiplicity_, bool sortWithSymmetry, SymmetricComplex const &complex):
  isKnownToBeNonMaximalFlag(false),
  dimension(dimension_),
  multiplicity(multiplicity_),
  sortKeyPermutation(complex.n)
{
  indices=IntVector(indices_.size());
  int j=0;
  for(std::set<int>::const_iterator i=indices_.begin();i!=indices_.end();i++,j++)
    indices[j]=*i;

  ZMatrix const &vertices=complex.getVertices();
  ZVector sum(vertices.getWidth());
  for(unsigned i=0;i<indices.size();i++)
    sum+=vertices[indices[i]];

  if(sortWithSymmetry)
    {
      sortKey=complex.sym.orbitRepresentative(sum,&sortKeyPermutation);
    }
  else
    {
      sortKey=sum;
    }
}


int SymmetricComplex::indexOfVertex(ZVector const &v)const
{
//  std::cerr<<v<<std::endl<<"In";
//  for(std::map<ZVector,int>::const_iterator i =indexMap.begin();i!=indexMap.end();i++)std::cerr<<i->first;

  std::map<ZVector,int>::const_iterator it=indexMap.find(v);
  assert(it!=indexMap.end());
  return it->second;
}


void SymmetricComplex::Cone::remap(SymmetricComplex &complex)
{
  ZMatrix const &vertices=complex.getVertices();
  ZVector sum(vertices.getWidth());
  for(unsigned i=0;i<indices.size();i++)
    sum+=vertices[indices[i]];

  unsigned n=sum.size();
  Permutation const &bestPermutation=sortKeyPermutation;

  assert(bestPermutation.size()==n);

  IntVector indicesNew(indices.size());
  int I=0;
  for(unsigned i=0;i<indices.size();i++,I++)
    {
      ZVector ny=bestPermutation.apply(complex.vertices[indices[i]]);
      std::map<ZVector,int>::const_iterator it=complex.indexMap.find(ny);
      assert(it!=complex.indexMap.end());
      indicesNew[I]=it->second;
    }
  indices=indicesNew;
}


std::set<int> SymmetricComplex::Cone::indexSet()const
{
  std::set<int> ret;
  for(unsigned i=0;i<indices.size();i++)
    ret.insert(indices[i]);

  return ret;
}

bool SymmetricComplex::Cone::isSubsetOf(Cone const &c)const
{
  unsigned next=0;
  for(unsigned i=0;i<indices.size();i++)
    {
      while(1)
        {
          if(next>=c.indices.size())return false;
          if(indices[i]==c.indices[next])break;
          next++;
        }
    }
  return true;
}


SymmetricComplex::Cone SymmetricComplex::Cone::permuted(Permutation const &permutation, SymmetricComplex const &complex, bool withSymmetry)const
{
  std::set<int> r;
  for(unsigned i=0;i<indices.size();i++)
    {
      ZVector ny=permutation.apply(complex.vertices[indices[i]]);
      std::map<ZVector,int>::const_iterator it=complex.indexMap.find(ny);
      if(it==complex.indexMap.end())
        {
//          AsciiPrinter(Stderr).printVector(complex.vertices[indices[i]]);
//          AsciiPrinter(Stderr).printVector(ny);

          assert(0);
        }
      r.insert(it->second);
    }


  return Cone(r,dimension,multiplicity,withSymmetry,complex);
}


bool SymmetricComplex::Cone::operator<(Cone const & b)const
{
  return sortKey<b.sortKey;
}


bool SymmetricComplex::Cone::isSimplicial(int linealityDim)const
{
  return (indices.size()+linealityDim)==dimension;
}


ZMatrix SymmetricComplex::Cone::orthogonalComplement(SymmetricComplex &complex)const
{
  ZMatrix l;
  for(unsigned i=0;i<indices.size();i++)
    l.appendRow(complex.vertices[indices[i]]);

  return l.reduceAndComputeKernel();
//  FieldMatrix m=integerMatrixToFieldMatrix(rowsToIntegerMatrix(l,complex.n),Q);
//  return fieldMatrixToIntegerMatrixPrimitive(m.reduceAndComputeKernel()).getRows();
}


SymmetricComplex::SymmetricComplex(ZMatrix const &rays, ZMatrix const &linealitySpace_, SymmetryGroup const &sym_):
  n(rays.getWidth()),
  linealitySpace(canonicalizeSubspace(linealitySpace_)),
  sym(sym_),
  dimension(-1)
{
  assert(rays.getWidth()==linealitySpace.getWidth());
//  vertices=rowsToIntegerMatrix(v,n);
  vertices=rays;

  for(int i=0;i<vertices.getHeight();i++)indexMap[vertices[i]]=i;
}


bool SymmetricComplex::contains(Cone const &c)const
{
  Cone temp=c;
  return cones.find(temp)!=cones.end();///////////////////!!!!!!!!!!!!!!!!!!!!!!!
}


void SymmetricComplex::insert(Cone const &c)
{
        if(c.dimension>dimension)dimension=c.dimension;
  if(!contains(c))//#2
    {
      cones.insert(c);
    }
  else
    {
      if(c.isKnownToBeNonMaximal()){cones.erase(c);cones.insert(c);}// mark as non-maximal
    }
}


int SymmetricComplex::getLinDim()const
{
        return linealitySpace.getHeight();
}

int SymmetricComplex::getMaxDim()const
{
  return dimension;
}


int SymmetricComplex::getMinDim()const
{
  int ret=100000;
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      if(i->dimension<ret)ret=i->dimension;
    }
  return ret;
}


bool SymmetricComplex::isMaximal(Cone const &c)const
{
  if(c.isKnownToBeNonMaximal())return false;
  if(c.dimension==dimension)return true;
  for(SymmetryGroup::ElementContainer::const_iterator k=sym.elements.begin();k!=sym.elements.end();k++)
    {
      Cone c2=c.permuted(*k,*this,false);
      for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
        {
          if(i->dimension>c.dimension)
            if(c2.isSubsetOf(*i) && !i->isSubsetOf(c2))return false;
        }
    }
  return true;
}

#if 0
IntVector SymmetricComplex::dimensionsAtInfinity()const
{
  /* Using a double description like method this routine computes the
     dimension of the intersection of each cone in the complex with
     the plane x_0=0 */
  IntVector ret(cones.size());

  int I=0;
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++,I++)
    {
      ZMatrix raysAtInfinity;
      for(int j=0;j<i->indices.size();j++)
        {
          if(vertices[i->indices[j]][0]==0)raysAtInfinity.push_back(vertices[i->indices[j]]);
          for(vector<int>::const_iterator k=j;k!=i->indices.end();k++)
            if(vertices[*j][0]*vertices[*k][0]<0)
              raysAtInfinity.push_back(((vertices[*j][0]>0)?1:-1)*(vertices[*j][0])*vertices[*k]+
                                       ((vertices[*k][0]>0)?1:-1)*(vertices[*k][0])*vertices[*j]);
        }
      ret[I]=rankOfMatrix(raysAtInfinity);
    }
  return ret;
}
#endif

void SymmetricComplex::buildConeLists(bool onlyMaximal, bool compressed, std::vector<std::vector<IntVector > >*conelist, std::vector<std::vector<Integer > > *multiplicities)const
{
  int dimLow=this->linealitySpace.getHeight();
  int dimHigh=this->getMaxDim();
  if(dimHigh<dimLow)dimHigh=dimLow-1;
  if(conelist)*conelist=std::vector<std::vector<IntVector> >(dimHigh-dimLow+1);
  if(multiplicities)*multiplicities=std::vector<std::vector<Integer> >(dimHigh-dimLow+1);
  for(int d=dimLow;d<=dimHigh;d++)
    {
      int numberOfOrbitsOutput=0;
      int numberOfOrbitsOfThisDimension=0;
      // bool newDimension=true;
        {
          int I=0;
          for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++,I++)
                  if(i->dimension==d)
                    {
                  numberOfOrbitsOfThisDimension++;
              if(!onlyMaximal || isMaximal(*i))
                {
                  numberOfOrbitsOutput++;
                  // bool isMax=isMaximal(*i);
                  // bool newOrbit=true;
                  std::set<std::pair<std::set<int>,Integer> > temp;
                    for(SymmetryGroup::ElementContainer::const_iterator k=sym.elements.begin();k!=sym.elements.end();k++)
                      {
                        Cone temp1=i->permuted(*k,*this,false);
                        temp.insert(std::pair<std::set<int>,Integer>(temp1.indexSet(),temp1.multiplicity));
                        if(compressed)break;
                    }
                  for(std::set<std::pair<std::set<int>,Integer> >::const_iterator j=temp.begin();j!=temp.end();j++)
                    {
                      IntVector temp;
                      for(std::set<int>::const_iterator k=j->first.begin();k!=j->first.end();k++)temp.push_back(*k);
                      if(conelist)(*conelist)[d-dimLow].push_back(temp);
                      if(multiplicities)(*multiplicities)[d-dimLow].push_back(j->second);
 /*                     if(isMax)if(multiplicities)
                        {

                          *multiplicities << i->multiplicity;
                          if(group)if(newOrbit)*multiplicities << "\t# New orbit";
                          if(newDimension)*multiplicities << "\t# Dimension "<<d;
                          *multiplicities << std::endl;
                        }*/
                      // newOrbit=false;
                      // newDimension=false;
                    }
              }
                    }
        }
    }

}

std::string SymmetricComplex::toStringJustCones(int dimLow, int dimHigh, bool onlyMaximal, bool group, std::ostream *multiplicities, bool compressed, bool tPlaneSort)const
{
  std::stringstream ret;

  ZVector additionalSortKeys(cones.size());
//  if(tPlaneSort)additionalSortKeys=dimensionsAtInfinity();
//  Integer lowKey=additionalSortKeys.min();
//  Integer highKey=additionalSortKeys.max();

  for(int d=dimLow;d<=dimHigh;d++)
    {
      int numberOfOrbitsOutput=0;
      int numberOfOrbitsOfThisDimension=0;
      bool newDimension=true;
 //     for(int key=lowKey;key<=highKey;key++)
        {
          int I=0;
          for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++,I++)
 //           if(additionalSortKeys[I]==key)
                  if(i->dimension==d)
                    {
                      numberOfOrbitsOfThisDimension++;
              if(!onlyMaximal || isMaximal(*i))
                {
                  numberOfOrbitsOutput++;
                  bool isMax=isMaximal(*i);
                  bool newOrbit=true;
                  std::set<std::set<int> > temp;
                    for(SymmetryGroup::ElementContainer::const_iterator k=sym.elements.begin();k!=sym.elements.end();k++)
                      {
                        Cone temp1=i->permuted(*k,*this,false);
                        temp.insert(temp1.indexSet());
                        if(compressed)break;
                    }
                  for(std::set<std::set<int> >::const_iterator j=temp.begin();j!=temp.end();j++)
                    {
                      ret << "{";
                      for(std::set<int>::const_iterator a=j->begin();a!=j->end();a++)
                        {
                          if(a!=j->begin())ret<<" ";
                          ret << *a;
                        }
                      ret << "}";
                      if(group)if(newOrbit)ret << "\t# New orbit";
                      if(newDimension)ret << "\t# Dimension "<<d;
                      ret <<std::endl;
                      if(isMax)if(multiplicities)
                        {
                          *multiplicities << i->multiplicity;
                          if(group)if(newOrbit)*multiplicities << "\t# New orbit";
                          if(newDimension)*multiplicities << "\t# Dimension "<<d;
                          *multiplicities << std::endl;
                        }
                      newOrbit=false;
                      newDimension=false;
                    }
              }
                    }
        }
    }

  return ret.str();
}


ZVector SymmetricComplex::fvector(bool boundedPart)const
{
  int min=getMinDim();
  int dimHigh=getMaxDim();
  if(dimHigh<min)dimHigh=min-1;
  ZVector ret(dimHigh-min+1);

  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      bool doAdd=!boundedPart;
      if(boundedPart)
        {
          bool isBounded=true;
          for(unsigned j=0;j<i->indices.size();j++)
            if(vertices[i->indices[j]][0].sign()==0)isBounded=false;
          doAdd=isBounded;
        }
      if(doAdd)
        ret[i->dimension-min]+=Integer(sym.orbitSize(i->sortKey));
    }
  return ret;
}


bool SymmetricComplex::isPure()const
{
  int dim=-1;
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    {
      if(isMaximal(*i))
        {
          int dim2=i->dimension;
          if(dim==-1)dim=dim2;
          if(dim!=dim2)return false;
        }
    }
  return true;
}


bool SymmetricComplex::isSimplicial()const
{
  int linealityDim=getMinDim();
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    if(!i->isSimplicial(linealityDim))
      return false;
  return true;
}


void SymmetricComplex::remap()
{
  for(ConeContainer::iterator i=cones.begin();i!=cones.end();i++)
    {
      Cone const&j=*i;
      Cone &j2=const_cast<Cone&>(j);//DANGER: cast away const. This does not change the sort key in the container, so should be OK.
      j2.remap(*this);
    }
}


int SymmetricComplex::numberOfConesOfDimension(int d)const
{
  assert(sym.isTrivial());

  int ret=0;
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    if(d==i->dimension)
      {
        ret++;
      }
  return ret;
}


int SymmetricComplex::dimensionIndex(Cone const &c)
{
  assert(sym.isTrivial());

  int ret=0;
  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    if(c.dimension==i->dimension)
      {
        if(!(c<*i)&&!(*i<c))
          return ret;
        else
          ret++;
      }
  return ret;
}

#if 0
void SymmetricComplex::boundary(Cone const &c, vector<int> &indices_, vector<int> &signs)
{
  indices_=vector<int>();
  signs=vector<int>();
  int d=c.dimension;


  IntegerVectorList l;
  for(int i=0;i<c.indices.size();i++)
    l.push_back(vertices[c.indices[i]]);
  IntegerVectorList facetNormals=PolyhedralCone(l,IntegerVectorList(),n).extremeRays();
  IntegerVectorList complementBasis=c.orthogonalComplement(*this);
  for(IntegerVectorList::const_iterator i=facetNormals.begin();i!=facetNormals.end();i++)
    {
      IntegerVectorList complementBasis1=complementBasis;
      complementBasis1.push_back(*i);
      FieldMatrix m=integerMatrixToFieldMatrix(rowsToIntegerMatrix(complementBasis1,n),Q);
      IntegerVectorList completion=fieldMatrixToIntegerMatrixPrimitive(m.reduceAndComputeKernel()).getRows();
      for(IntegerVectorList::const_iterator j=completion.begin();j!=completion.end();j++)complementBasis1.push_back(*j);
      int sign=determinantSign(complementBasis1);

      set<int> indices;
      for(vector<int>::const_iterator j=c.indices.begin();j!=c.indices.end();j++)if(dotLong(vertices[*j],*i)==0)indices.insert(*j);
      Cone facet(indices,d-1,1,true,*this);
      IntegerVectorList complementBasis2=facet.orthogonalComplement(*this);
      for(IntegerVectorList::const_iterator j=completion.begin();j!=completion.end();j++)complementBasis2.push_back(*j);
      indices_.push_back(dimensionIndex(facet));
      signs.push_back(sign*determinantSign(complementBasis2));
    }
}


IntegerMatrix SymmetricComplex::boundaryMap(int d)
{
  assert(sym.isTrivial());

  IntegerMatrix ret(numberOfConesOfDimension(d-1),numberOfConesOfDimension(d));

  for(ConeContainer::const_iterator i=cones.begin();i!=cones.end();i++)
    if(d==i->dimension)
      {
        int I=dimensionIndex(*i);
        vector<int> indices;
        vector<int> signs;
        boundary(*i,indices,signs);
        for(int j=0;j<indices.size();j++)
          {
              ret[indices[j]][I]+=signs[j];
          }
      }
  return ret;
}
#endif


  std::string SymmetricComplex::toString(int flags)const
  {
    PolymakeFile polymakeFile;
    polymakeFile.create("NONAME","PolyhedralFan","PolyhedralFan",flags&FPF_xml);





    polymakeFile.writeCardinalProperty("AMBIENT_DIM",n);
    polymakeFile.writeCardinalProperty("DIM",getMaxDim());
    polymakeFile.writeCardinalProperty("LINEALITY_DIM",linealitySpace.getHeight());
    //    polymakeFile.writeMatrixProperty("RAYS",rays,true,comments);
        polymakeFile.writeMatrixProperty("RAYS",vertices,true);
    polymakeFile.writeCardinalProperty("N_RAYS",vertices.getHeight());


    polymakeFile.writeMatrixProperty("LINEALITY_SPACE",linealitySpace,n);
    polymakeFile.writeMatrixProperty("ORTH_LINEALITY_SPACE",kernel(linealitySpace),n);

/*
    if(flags & FPF_primitiveRays)
    {
      ZMatrix primitiveRays;
      for(int i=0;i<rays.getHeight();i++)
        for(PolyhedralConeList::const_iterator j=cones.begin();j!=cones.end();j++)
          if(j->contains(*i)&&(j->dimensionOfLinealitySpace()+1==j->dimension()))
            primitiveRays.push_back(j->semiGroupGeneratorOfRay());

      polymakeFile.writeMatrixProperty("PRIMITIVE_RAYS",rowsToIntegerMatrix(primitiveRays,n));
    }
*/
#if 0
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
#endif

  //  log1 fprintf(Stderr,"Computing f-vector.\n");
    ZVector fvector=this->fvector();
    polymakeFile.writeCardinalVectorProperty("F_VECTOR",fvector);
  //  log1 fprintf(Stderr,"Done computing f-vector.\n");

    if(flags&FPF_boundedInfo)
      {
  //      log1 fprintf(Stderr,"Computing bounded f-vector.\n");
        ZVector fvectorBounded=this->fvector(true);
        polymakeFile.writeCardinalVectorProperty("F_VECTOR_BOUNDED",fvectorBounded);
  //      log1 fprintf(Stderr,"Done computing bounded f-vector.\n");
      }
#if 0
    {
      Integer euler;
      int mul=-1;
      for(int i=0;i<fvector.size();i++,mul*=-1)euler+=Integer(mul)*fvector[i];
      polymakeFile.writeCardinalProperty("MY_EULER",euler);
    }
#endif
  //  log1 fprintf(Stderr,"Checking if complex is simplicial and pure.\n");
    polymakeFile.writeCardinalProperty("SIMPLICIAL",isSimplicial());
    polymakeFile.writeCardinalProperty("PURE",isPure());
  //  log1 fprintf(Stderr,"Done checking.\n");


    if(flags&FPF_cones)polymakeFile.writeStringProperty("CONES",toStringJustCones(getMinDim(),getMaxDim(),false,flags&FPF_group, 0,false,flags&FPF_tPlaneSort));
    if(flags&FPF_maximalCones)polymakeFile.writeStringProperty("MAXIMAL_CONES",toStringJustCones(getMinDim(),getMaxDim(),true,flags&FPF_group, 0,false,flags&FPF_tPlaneSort));
    if(flags&FPF_conesCompressed)polymakeFile.writeStringProperty("CONES_ORBITS",toStringJustCones(getMinDim(),getMaxDim(),false,flags&FPF_group, 0,true,flags&FPF_tPlaneSort));
    if((flags&FPF_conesCompressed) && (flags&FPF_maximalCones))polymakeFile.writeStringProperty("MAXIMAL_CONES_ORBITS",toStringJustCones(getMinDim(),getMaxDim(),true,flags&FPF_group, 0,true,flags&FPF_tPlaneSort));

    if(!sym.isTrivial())
      {
        polymakeFile.writeMatrixProperty("SYMMETRY_GENERATORS",IntToZMatrix(sym.getGenerators()));
      }

    std::stringstream s;
    polymakeFile.writeStream(s);
    return s.str();

    #if 0

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

  ZCone SymmetricComplex::makeZCone(IntVector const &indices)const
  {
    ZMatrix generators(indices.size(),getAmbientDimension());
    for(unsigned i=0;i<indices.size();i++)
      generators[i]=vertices[indices[i]];
    return ZCone::givenByRays(generators,linealitySpace);
  }
}
