/*
 * gfanlib_zfan.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: anders
 */

#include "gfanlib_zfan.h"

namespace gfan
{
  static int numberOf(std::vector<std::vector<IntVector> > T, int dimension)
  {
    assert(dimension>=0);
    if(dimension>=T.size())return 0;
    return T[dimension].size();
  }
  std::vector<std::vector<IntVector> > &ZFan::table(bool orbit, bool maximal)const
  {
    if(orbit)
      {
        if(maximal)return maximalConeOrbits;
        return coneOrbits;
      }
    if(maximal)return maximalCones;
    return cones;
  }
  int ZFan::numberOfConesOfDimension(int d, bool orbit, bool maximal)const
  {
    return numberOf(table(orbit,maximal),d);
  }
  ZCone ZFan::getCone(int dimension, int index, bool orbit, bool maximal)const
  {
    IntVector indices=getConeIndices(dimension,index,orbit,maximal);
    return this->complex->makeZCone(indices);
  }
  IntVector ZFan::getConeIndices(int dimension, int index, bool orbit, bool maximal)const
  {
    assert(index>=0);
    assert(index<numberOfConesOfDimension(dimension,orbit,maximal));
    return table(orbit,maximal)[dimension][index];
  }
  void ZFan::ensureConeCollection()const
  {
    if(!coneCollection)
      {
        assert(0);
      }
  }
  void ZFan::ensureComplex()const
  {
    if(!complex)
      {
        assert(coneCollection);
        complex = new SymmetricComplex(coneCollection->toSymmetricComplex());
        complex->buildConeLists(false,false,&cones);
        complex->buildConeLists(true,false,&maximalCones);
        complex->buildConeLists(false,true,&coneOrbits);
        complex->buildConeLists(true,true,&maximalConeOrbits);
      }
  }
  ZFan::~ZFan()
  {
    if(coneCollection)
      {
        delete coneCollection;
        coneCollection=0;
      }
    if(complex)
      {
        delete complex;
        complex=0;
      }
  }
  ZFan::ZFan(ZFan const& f):
    complex(0),
    coneCollection(0)
  {
    if(f.coneCollection)
      {
        coneCollection=new PolyhedralFan(*f.coneCollection);
      }
    if(f.complex)
      {
        complex=new SymmetricComplex(*f.complex);
      }
  }
  ZFan& ZFan::operator=(ZFan const &f)
  {
    if(this!=&f)
      {
        if(complex)
          {
            delete complex;
            complex=0;
          }
        if(coneCollection)
          {
            delete coneCollection;
            coneCollection=0;
          }
        if(f.coneCollection)
          {
            coneCollection=new PolyhedralFan(*f.coneCollection);
          }
        if(f.complex)
          {
            complex=new SymmetricComplex(*f.complex);
          }
      }
    return *this;
  }
  ZFan::ZFan(int ambientDimension):
    complex(0)
  {
    coneCollection=new PolyhedralFan(ambientDimension);
  }
  ZFan::ZFan(SymmetryGroup const &sym_):
    complex(0)
  {
    coneCollection=new PolyhedralFan(sym_);
  }
  ZFan ZFan::fullFan(int n)
  {
    ZFan ret(n);
    ret.insert(ZCone(ZMatrix(0,n),ZMatrix(0,n)));
    return ret;
  }
  ZFan ZFan::fullFan(SymmetryGroup const &sym_)
  {
    ZFan ret(sym_);
    ret.insert(ZCone(ZMatrix(0,sym_.sizeOfBaseSet()),ZMatrix(0,sym_.sizeOfBaseSet())));
    return ret;
  }
  int ZFan::getAmbientDimension()const
  {
    if(complex)
      return complex->getAmbientDimension();
    if(coneCollection)
      return coneCollection->getAmbientDimension();
    assert(0);
    return 0;
  }
  void ZFan::insert(ZCone const &c)
  {
    ensureConeCollection();
    coneCollection->insert(c);
  }

/*  ZFan::ZFan(int ambientDimension):
    theFan(ambientDimension)
  {

  }*/
/*
ZFan::ZFan(SymmetryGroup const &sym):
  theFan(sym)
{
}
*/

std::string ZFan::toString(int flags)const
{
  ensureComplex();
  return complex->toString(flags);//complex->getMinDim(),complex->getMaxDim(),0,0);
//  return "NEEDTOFIXTHIS";
  //return theFan.toString();
}

/*int ZFan::getAmbientDimension()const
{
  return theFan.getAmbientDimension();
}


void ZFan::insert(ZCone const &c)
{
  theFan.insert(c);
}
*/
/*
void ZFan::remove(ZCone const &c)
{
  theFan.remove(c);
}
*/
/*
ZFan::coneIterator ZFan::conesBegin()const
{
  return theFan.conesBegin();
}

ZFan::coneIterator ZFan::conesEnd()const
{
  return theFan.conesEnd();
}
*/
  //  static PolyhedralFan readFan(string const &filename, bool onlyMaximal=true, IntegerVector *w=0, set<int> const *conesIndice=0, SymmetryGroup const *sym=0, bool readCompressedIfNotSym=false);

}
