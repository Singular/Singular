/*
 * gfanlib_zfan.h
 *
 *  Created on: Nov 17, 2010
 *      Author: anders
 */

#ifndef GFANLIB_ZFAN_H_
#define GFANLIB_ZFAN_H_

/*
 * gfanlib_polyhedralfan.h
 *
 *  Created on: Nov 16, 2010
 *      Author: anders
 */

#ifndef GFANLIB_ZFAN_H_INCLUDED
#define GFANLIB_ZFAN_H_INCLUDED

#include <set>
#include <list>
#include <map>
#include "gfanlib_polyhedralfan.h"
#include "gfanlib_symmetriccomplex.h"

namespace gfan{

/**
 * This class represents a Polyhedral fan with the combined features of a SymmetricComplex, and
 * a PolyhedralFan (which we regards as some collection of cones of the fan with the property that every cone
 * of the fan is a face of these (up to action by the group)).
 * It is important to distinguis between the cones of the collection, the cones of the fan as a mathematical object, and the orbits of cones of the fan.
 *
 *
 * TEMPLATES SHOULD BE HIDDEN
 */
class ZFan
{
  mutable PolyhedralFan *coneCollection;
  mutable SymmetricComplex *complex;
  mutable std::vector<std::vector<IntVector > > cones;
  mutable std::vector<std::vector<IntVector > > maximalCones;
  mutable std::vector<std::vector<Integer> > multiplicities; // for maximal cones only
  mutable std::vector<std::vector<IntVector > > coneOrbits;
  mutable std::vector<std::vector<IntVector > > maximalConeOrbits;
  mutable std::vector<std::vector<Integer> > multiplicitiesOrbits; // for maximal cones orbits only


  //  SymmetryGroup sym;

  /**
   * Functions that makes the symmetric complex invalid:
   * insert() and construction
   *
   * Functions that make the cone collection invalid:
   * readFan()
   *
   */
  void ensureConeCollection()const;
  void ensureComplex()const;
  void killComplex()const;
public:
  std::vector<std::vector<IntVector> > &table(bool orbit, bool maximal)const;
  ~ZFan();
  ZFan():
    coneCollection(0),
    complex(0)
  {}
  ZFan(ZFan const& f);
  /**
   *
   * To read from string, do the following:
   *       std::string test="TEST";
   *   std::istringstream s(test);
   *   ZFan G(s);
   *
   */
  ZFan(std::istream  &f);
  ZFan& operator=(ZFan const &f);
  /**
   * Creates an empty fan in the ambient space of dimension n.
   * It is a mistake to:
   *  specify a negative ambientDimension.
   */
  ZFan(int ambientDimension);
  /**
   * Creates an empty fan in the ambient space with dimension equal to the number of elements being permuted by the group.
   * The fan will have the symmmetries given by sym_ associated.
   */
  ZFan(SymmetryGroup const &sym_);
  /**
   * Creates the fan in dimension n consisting of the n-dimensional space.
   */
  static ZFan fullFan(int n);
  /**
   * Creates the full space as a fan in the ambient space with dimension equal to the number of elements being permuted by the group.
   * The fan will have the symmetries given by sym_ associated.
   */
  static ZFan fullFan(SymmetryGroup const &sym_);
  /**
   * Reads from stream
   */
//  static ZFan readFan(string const &filename, bool onlyMaximal=true, IntegerVector *w=0, set<int> const *conesIndice=0, SymmetryGroup const *sym=0, bool readCompressedIfNotSym=false);
  /**
   * Writes to string
   */
  std::string toString(int flags=0)const;
  /**
   * Returns the dimension of the ambient space.
   */
  int getAmbientDimension()const;
  /**
   * Returns the largest dimension of a cone in the fan. If the fan is empty, then -1 is returned.
   */
  int getDimension()const;
  /**
   * Returns the smallest codimension of a cone in the fan. If the fan is empty, then -1 is returned.
   */
  int getCodimension()const;
  /**
   * Returns the dimension of the lineality space of the fan. Notice that the lineality space of the
   * empty fan is the ambient space.
   */
  int getLinealityDimension()const;
  /**
   * Returns the f-Vector of the fan.
   */
  ZVector getFVector()const;
  /**
   * Returns true, if the fan is simplicial. False otherwise.
   */
  bool isSimplicial()const;
  /**
   * Returns true, if the fan is pure. False otherwise.
   */
  bool isPure()const;
  /**
   * Inserts c into the fan.
   * It is a mistake to insert a cone which live in a space of the wrong dimension.
   * It is a mistake to insert a cone which does not result in a fan satisfying the nice intersection properties of a fan.
   * However, the second mistake will not cause the code to crash, but only give undefined results.
   *
   * The method insert() is expensive in the sense that calling it may require part of the representation
   * of the fan to be recomputed. The recomputation only happens on request. Therefore it is expensive
   * to have alternating calls to for example "insert()" and "numberOfConesOfDimension()".
   *
   * Notice that insert() has the effect of reordering cones, orbits and rays of the fan.
   */
  void insert(ZCone const &c);
  void remove(ZCone const &c);
  /**
   * Returns the number of cones of dimension d in the collection.
   */
  int numberOfConesOfDimension(int d, bool orbit, bool maximal)const;
  int numberOfConesInCollection()const;
  /**
   * Returns the cone in the collection given by the index. It is a mistake to specify an index which
   * is out of range.
   */
  //  ZCone const &getConeInCollection(int index)const;
  /**
   * Returns the cone in the cone of the fan
   */
  ZCone getCone(int dimension, int index, bool orbit, bool maximal)const;
  IntVector getConeIndices(int dimension, int index, bool orbit, bool maximal)const;

//  ZFan expand()const;
//  SymmetryGorup recoverAllSymmetries()const;

/*  typedef PolyhedralConeList::const_iterator coneIterator;
  PolyhedralFan::coneIterator conesBegin()const;
  PolyhedralFan::coneIterator conesEnd()const;
*/

};

}

#endif

#endif /* GFANLIB_ZFAN_H_ */
