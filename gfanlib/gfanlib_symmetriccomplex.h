/*
 * gfanlib_symmetriccomplex.h
 *
 *  Created on: Nov 16, 2010
 *      Author: anders
 */

#ifndef GFANLIB_SYMMETRICCOMPLEX_H_INCLUDED
#define GFANLIB_SYMMETRICCOMPLEX_H_INCLUDED

#include <set>
#include <string>
#include <map>

#include "gfanlib_symmetry.h"
#include "gfanlib_matrix.h"
#include "gfanlib_zcone.h"

namespace gfan{
  enum FanPrintingFlags{
    FPF_conesCompressed=1,
    FPF_conesExpanded=2,
    FPF_cones=4,
    FPF_maximalCones=8,
    FPF_boundedInfo=16,
    FPF_values=32,
    FPF_group=64,
    FPF_multiplicities=128,
    FPF_xml=256,
    FPF_tPlaneSort=512,
    FPF_primitiveRays=1024,

    FPF_default=2+4+8
  };

  class SymmetricComplex{
  int n;
  ZMatrix linealitySpace; // Has full row rank.
  ZMatrix vertices;
  std::map<ZVector,int> indexMap;
  SymmetryGroup sym;
  IntVector dimensionsAtInfinity()const;
 public:
   int getAmbientDimension()const{return n;}
   class Cone
  {
    bool isKnownToBeNonMaximalFlag;
  public:
    IntVector indices;//always sorted
    Cone(std::set<int> const &indices_, int dimension_, Integer multiplicity_, bool sortWithSymmetry, SymmetricComplex const &complex);
    std::set<int> indexSet()const;
    int dimension;
    Integer multiplicity;
    bool isKnownToBeNonMaximal()const{return isKnownToBeNonMaximalFlag;}
    void setKnownToBeNonMaximal(){isKnownToBeNonMaximalFlag=true;}
    bool isSubsetOf(Cone const &c)const;
    SymmetricComplex::Cone permuted(Permutation const &permutation, SymmetricComplex const &complex, bool withSymmetry)const;
    ZVector sortKey;
    Permutation sortKeyPermutation;
    bool operator<(const Cone & b)const;
    bool isSimplicial(int linealityDim)const;
    void remap(SymmetricComplex &complex);
/**
 * This routine computes a basis for the orthogonal complement of the cone.
 * Notice that the lineality space, which is unknown at the time, is ignored.
 * This routine is deterministic and used for orienting the faces when computing homology.
 */
    ZMatrix orthogonalComplement(SymmetricComplex &complex)const;
  };
  typedef std::set<Cone> ConeContainer;
  ConeContainer cones;
  int dimension;
  SymmetricComplex(ZMatrix const &rays, ZMatrix const &linealitySpace, SymmetryGroup const &sym_);
  /**
   * Returns a reference to the matrix of vertices on which the complex is build.
   * The reference is valid as the Symmetric complex object exists.
   */
  ZMatrix const &getVertices()const{return vertices;}
  bool contains(Cone const &c)const;
  void insert(Cone const &c);
  int getMaxDim()const;
  int getMinDim()const;
  int getLinDim()const;
  bool isMaximal(Cone const &c)const;
  bool isPure()const;
  ZVector fvector(bool boundedPart=false)const;
  void buildConeLists(bool onlyMaximal, bool compressed, std::vector<std::vector<IntVector > >*conelist, std::vector<std::vector<Integer > > *multiplicities=0)const;
  std::string toStringJustCones(int dimLow, int dimHigh, bool onlyMaximal, bool group, std::ostream *multiplicities=0, bool compressed=false, bool tPlaneSort=false)const;
  std::string toString(int flags=0)const;
  bool isSimplicial()const;
  /**
     Calling this function will change the representative of each cone
     orbit by "applying" the permutation which will give the sortkey to
     the set of indices of the cone.
   */
  void remap();
  /**
   * Looks up the index of the vector among the vertices.
   */
  int indexOfVertex(ZVector const &v)const;
  int numberOfConesOfDimension(int d)const;
  /**
   * Given a cone this returns its index among all cones of that dimension.
   * Used for assigning "names" to cones.
   */
  int dimensionIndex(Cone const &c);
#if 0
  /**
   * This routine is used for constructing the boundary map for homology computations.
   */
  void boundary(Cone const &c, IntVector &indices, IntVector &signs);
/**
 * This routine computes the ith boundary map for homology as a matrix.
 */
  ZMatrix boundaryMap(int i);
#endif
  ZCone makeZCone(IntVector const &indices)const;
};
}

#endif
