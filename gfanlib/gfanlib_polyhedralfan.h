/*
 * gfanlib_polyhedralfan.h
 *
 *  Created on: Nov 16, 2010
 *      Author: anders
 */

#ifndef GFANLIB_POLYHEDRALFAN_H_INCLUDED
#define GFANLIB_POLYHEDRALFAN_H_INCLUDED

#include <set>
#include <list>
#include <map>
#include "gfanlib_symmetry.h"
#include "gfanlib_matrix.h"
#include "gfanlib_zcone.h"
#include "gfanlib_symmetriccomplex.h"

namespace gfan{


typedef std::set<ZCone> PolyhedralConeList;
typedef std::list<IntVector> IntVectorList;
typedef std::map<int,IntVectorList> IncidenceList;

class PolyhedralFan;
PolyhedralFan refinement(const PolyhedralFan &a, const PolyhedralFan &b, int cutOffDimension=-1, bool allowASingleConeOfCutOffDimension=false);

/** A PolyhedralFan is simply a collection of canonicalized PolyhedralCones.
 * It contains no combinatorial information in the sense of a polyhedral complex.
 * A cone being present in the PolyhedralFan corresponds to the cone and all its facets being present
 * in the mathematical object.
 * The intersection of cones in the fan must be a face of both.
 * In particular all cones in a PolyhedralFan have the same lineality space.*/
class PolyhedralFan
{
  int n;
  SymmetryGroup symmetries;
  PolyhedralConeList cones;
 public:
  static class PolyhedralFan fullSpace(int n);
  static class PolyhedralFan facetsOfCone(ZCone const &c);
  PolyhedralFan(int ambientDimension);
  PolyhedralFan(SymmetryGroup const &sym);
  std::string toString(int flags=FPF_default)const;
  /* Read in a polyhedral fan, but with the cones containing w.  If
     present, only read in cones among coneIndices.  If sym is
     present, read COMPRESSED section and make w containment up to
     symmetry, taking all elements in the orbit that contains w into
     the fan.  If onlyMaximal is set then only maximal cones are read
     in.
   */
  int getAmbientDimension()const;
  int getMaxDimension()const;
  int getMinDimension()const;
  // friend PolyhedralFan refinement(const PolyhedralFan &a, const PolyhedralFan &b, int cutOffDimension=-1, bool allowASingleConeOfCutOffDimension=false);
  ZMatrix getRays(int dim=1);//This can be called for other dimensions than 1. The term "Rays" still makes sense modulo the common linearity space
  ZMatrix getRelativeInteriorPoints();
  void insert(ZCone const &c);
  void remove(ZCone const &c);
  void removeAllLowerDimensional();
  /**
     Since the cones stored in a PolyhedralFan are cones of a
     polyhedral fan, it is possible to identify non maximal cones by
     just checking containment of relative interior points in other
     cones. This routine removes all non-maximal cones.
   */
  void removeNonMaximal();
  /**
     Returns the number of cones stored in the fan. This is not the number of cones in the fan in a mathematical sense.
   */
  int size()const;
  int dimensionOfLinealitySpace()const;
  void makePure();
  bool contains(ZCone const &c)const;
  /**
   * For a vector contained in the support of the fan represented by the fan object, this function
   * computes the cone that contains the vector in its relative interior.
   */
  ZCone coneContaining(ZVector const &v)const;
  PolyhedralFan facetComplex()const;

  ZMatrix getRaysInPrintingOrder(bool upToSymmetry=false)const;
  IncidenceList getIncidenceList(SymmetryGroup *sym=0)const;
  bool isEmpty()const;


  /**
     Computes the link of the face containing w in its relative interior.
   */
  PolyhedralFan link(ZVector const &w)const;
  PolyhedralFan link(ZVector const &w, SymmetryGroup *sym)const;



  typedef PolyhedralConeList::const_iterator coneIterator;
  PolyhedralFan::coneIterator conesBegin()const;
  PolyhedralFan::coneIterator conesEnd()const;


  /**
   * Converts a PolyhedralFan into a SymmetricComplex. This is used for homology computations, but not for printing yet.
   */
  SymmetricComplex toSymmetricComplex()const;
//  static PolyhedralFan readFan(string const &filename, bool onlyMaximal=true, IntegerVector *w=0, set<int> const *conesIndice=0, SymmetryGroup const *sym=0, bool readCompressedIfNotSym=false);
};


void addFacesToSymmetricComplex(SymmetricComplex &c, ZCone const &cone, ZMatrix const &facetCandidates, ZMatrix const &generatorsOfLinealitySpace);
void addFacesToSymmetricComplex(SymmetricComplex &c, std::set<int> const &indices, ZMatrix const &facetCandidates, int dimension, Integer multiplicity);

}

#endif
