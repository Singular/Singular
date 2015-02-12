/*
 * gfanlib_zfan.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: anders
 */

#include "gfanlib_zfan.h"
#include "gfanlib_polymakefile.h"

using namespace std;

namespace gfan
{
  static int numberOf(std::vector<std::vector<IntVector> > T, int dimension)
  {
    assert(dimension>=0);
    if(dimension>=(int)T.size())return 0;
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
    this->ensureComplex();
    return numberOf(table(orbit,maximal),d);
  }
  ZCone ZFan::getCone(int dimension, int index, bool orbit, bool maximal)const
  {
    IntVector indices=getConeIndices(dimension,index,orbit,maximal);
    ZCone ret=this->complex->makeZCone(indices);
    if(maximal)ret.setMultiplicity(((orbit)?multiplicitiesOrbits:multiplicities)[dimension][index]);
    return ret;
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
        complex->buildConeLists(true,false,&maximalCones,&multiplicities);
        complex->buildConeLists(false,true,&coneOrbits);
        complex->buildConeLists(true,true,&maximalConeOrbits,&multiplicitiesOrbits);
      }
  }
  void ZFan::killComplex()const
  {
    if(complex)
      {
        delete complex;
        complex=0;
      }
  }

  ZFan::ZFan(std::istream &f):
    coneCollection(0),
    complex(0)
  {
//    PolyhedralFan PolyhedralFan::readFan(string const &filename, bool onlyMaximal, IntegerVector *w, set<int> const *coneIndices, SymmetryGroup const *sym, bool readCompressedIfNotSym)
    PolymakeFile inFile;
    //assert(0);
     inFile.open(f);

    int n=inFile.readCardinalProperty("AMBIENT_DIM").toInt();
    int nRays=inFile.readCardinalProperty("N_RAYS").toInt();
    ZMatrix rays=inFile.readMatrixProperty("RAYS",nRays,n);
    int linealityDim=inFile.readCardinalProperty("LINEALITY_DIM").toInt();
    ZMatrix linealitySpace=inFile.readMatrixProperty("LINEALITY_SPACE",linealityDim,n);

    SymmetryGroup sym(n);
    bool readingSymmetricComplex=false;
    if(inFile.hasProperty("SYMMETRY_GENERATORS"))
      {
        sym.computeClosure(ZToIntMatrix(inFile.readMatrixProperty("SYMMETRY_GENERATORS",-1,n)));
        readingSymmetricComplex=true;
      }


    const char *sectionName=0;
    const char *sectionNameMultiplicities=0;
    if(readingSymmetricComplex)
      {
        if(inFile.hasProperty("MAXIMAL_CONES_ORBITS"))
          {
            sectionName="MAXIMAL_CONES_ORBITS";
            sectionNameMultiplicities="MULTIPLICITIES_ORBITS";
          }
        else
          {
            sectionName="CONES_ORBITS";
          }
      }
    else
      {
        if(inFile.hasProperty("MAXIMAL_CONES"))
          {
            sectionName="MAXIMAL_CONES";
            sectionNameMultiplicities="MULTIPLICITIES";
          }
        else
          {
            sectionName="CONES";
          }
      }

    /*    if(sym || readCompressedIfNotSym)
      {
        sectionName=(onlyMaximal)?"MAXIMAL_CONES_ORBITS":"CONES_ORBITS";
        sectionNameMultiplicities=(onlyMaximal)?"MULTIPLICITIES_ORBITS":"DUMMY123";
      }
    else
*/
    /*{
        sectionName="MAXIMAL_CONES";//(onlyMaximal)?"MAXIMAL_CONES":"CONES";
        sectionNameMultiplicities="MULTIPLICITIES";//(onlyMaximal)?"MULTIPLICITIES":"DUMMY123";
      }
*/
//    ZVector w2(n);
//    if(w==0)w=&w2;

 //       SymmetryGroup sym2(n);
 //       if(sym==0)sym=&sym2;

/*  sectionName=0;
  if(inFile.hasProperty("MAXIMAL_CONES"))
    sectionName="MAXIMAL_CONES";
  else
    {  if(inFile.hasProperty("CONES"))
      sectionName="CONES";
    else
      assert(0);
    }*/

  vector<list<int> > cones=inFile.readMatrixIncidenceProperty(sectionName);
//        IntegerVectorList r;

        bool hasMultiplicities=inFile.hasProperty(sectionNameMultiplicities);
        ZMatrix multiplicities(0,0);
        if(hasMultiplicities)multiplicities=inFile.readMatrixProperty(sectionNameMultiplicities,cones.size(),1);

        ZFan ret(sym);

//        log2 cerr<< "Number of orbits to expand "<<cones.size()<<endl;
        for(unsigned i=0;i<cones.size();i++)
        //  if(coneIndices==0 || coneIndices->count(i))
            {
//              log2 cerr<<"Expanding symmetries of cone"<<endl;
              {
                ZMatrix coneRays(0,n);
                for(list<int>::const_iterator j=cones[i].begin();j!=cones[i].end();j++)
                  coneRays.appendRow((rays[*j]));
                ZCone C=ZCone::givenByRays(coneRays,linealitySpace);
                if(hasMultiplicities)C.setMultiplicity(multiplicities[i][0]);
//                for(SymmetryGroup::ElementContainer::const_iterator perm=sym->elements.begin();perm!=sym->elements.end();perm++)
                  {
//                    if(C.contains(perm.applyInverse(*w)))
//                      {
 //                       PolyhedralCone C2=C.permuted(*perm);
//                        C2.canonicalize();
//                        ret.insert(C2);
 //                     }
                    ret.insert(C);
                  }
              }
            }
//        return ret;
        *this=ret;
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
    coneCollection(0),
    complex(0),
    cones(f.table(0,0)),
    maximalCones(f.table(0,1)),
    coneOrbits(f.table(1,0)),
    maximalConeOrbits(f.table(1,1))
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
  int ZFan::getCodimension()const
  {
    if(complex)
      return complex->getAmbientDimension()-complex->getMaxDim();
    if(coneCollection)
      return coneCollection->getAmbientDimension()-coneCollection->getMaxDimension();
    assert(0);
    return 0;
  }
  int ZFan::getDimension()const
  {
    if(complex)
      return complex->getMaxDim();
    if(coneCollection)
      return coneCollection->getMaxDimension();
    assert(0);
    return 0;
  }
  int ZFan::getLinealityDimension()const
  {
    if(complex)
      return complex->getLinDim();
    if(coneCollection)
      return coneCollection->dimensionOfLinealitySpace();
    assert(0);
    return 0;
  }
  ZVector ZFan::getFVector()const
  {
    ensureComplex();
    return complex->fvector();
  }
  bool ZFan::isSimplicial()const
  {
    ensureComplex();
    return complex->isSimplicial();
  }
  bool ZFan::isPure()const
  {
    ensureComplex();
    return complex->isPure();
  }
  void ZFan::insert(ZCone const &c)
  {
    ensureConeCollection();
    killComplex();
    coneCollection->insert(c);
  }
  void ZFan::remove(ZCone const &c)
  {
    ensureConeCollection();
    killComplex();
    coneCollection->remove(c);
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

  //  std::string s=complex->toString(flags);
//  killComplex();
//  return s;//complex->getMinDim(),complex->getMaxDim(),0,0);
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
