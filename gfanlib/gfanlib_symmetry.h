/*
 * gfan_symmetry.h
 *
 *  Created on: Oct 22, 2010
 *      Author: anders
 */

#ifndef GFANLIB_SYMMETRY_H_INCLUDED
#define GFANLIB_SYMMETRY_H_INCLUDED

#include <set>
#include "gfanlib_vector.h"
#include "gfanlib_matrix.h"

namespace gfan{

/**
 * The permutation class represents an element in the symmetric group S_n.
 */
class Permutation:public IntVector
{
//  IntVector data;
public:
  /**
   * Returns true if a contains the elements from 0 up to a.size()-1.
   */
  static bool isPermutation(IntVector const &a);
  /**
   * Returns true if all rows of the matrix are pemutations
   */
  static bool arePermutations(IntMatrix const &m);
  /**
   * Generates the identity permutation on n elements.
   */
  Permutation(int n):
    IntVector(n)
    {
      for(int i=0;i<n;i++)(*this)[i]=i;
    }
  /**
   * Generates a permutation from the vector v. The ith entry of v tells
   * If the check flag is set to true, then it is checked whether the vector represents
   * a permutation. If not, the code fails with an assertion.
   */
  Permutation(IntVector const &v, bool check=true):
    IntVector(v)
    {
      if(check)assert(isPermutation(v));
    }

  static Permutation transposition(int n, int i, int j)
  {
    IntVector ret(n);
    for(int k=0;k<n;k++)ret[k]=k;
    ret[i]=j;
    ret[j]=i;
    return Permutation(ret);
  }
  static Permutation cycle(int n)
  {
    IntVector a(n);
    for(int i=0;i<n-1;i++)a[i]=i+1;
    a[n-1]=0;
    return Permutation(a);
  }
  IntVector toIntVector()const
  {
    return IntVector(*this);
  }

  int sizeOfBaseSet()const
  {
    return size();
  }
  Permutation inverse()const;

  /**
   * Apply the permutation
   */
  Permutation apply(Permutation const &p)const;
  IntVector apply(IntVector const &v)const;
  ZVector apply(ZVector const &v)const;
  ZMatrix apply(ZMatrix const &m)const;
  Permutation applyInverse(Permutation const &p)const;
  IntVector applyInverse(IntVector const &v)const;
  ZVector applyInverse(ZVector const &v)const;
  ZMatrix applyInverse(ZMatrix const &m)const;

  /**
     The set of vectors which are not improved lexicographically when
     perm is applied to them is convex. Its closure is a
     halfspace. This routine returns the inner normal of this
     halfspace. The only exception is if perm is the identity then the
     zero vector is returned.
   */
  ZVector fundamentalDomainInequality()const;
};

/**
 * This object represents a subgroup of the symmetric group S_n.
 */

class SymmetryGroup{
//  unsigned char *byteTable;
  int byteTableHeight;
  class Trie *trie;
public:
  typedef std::set<Permutation/*,LexicographicTermOrder*/> ElementContainer;
   ElementContainer elements;//Make this private
   int size()const
   {
     return elements.size();
   }
   int sizeOfBaseSet()const;
  /**
     The set of vectors which cannot be improved lexicographically by
     applying an element from the group is a convex set. Its closure
     is a polyhedral cone. This routine returns a set of inequalities
     The returned list does not contain the zero vector.
   */
  ZMatrix fundamentalDomainInequalities()const;
  SymmetryGroup(int n);
  void computeClosure(Permutation const &v);
  void computeClosure(IntMatrix const &l);
  IntMatrix getGenerators()const;
  int orbitSize(ZVector const &stable)const;
  bool isTrivial()const;
  /**
     The symmetry group acts on vectors by permuting the entries. The
     following routine returns a unique representative for the orbit
     containing v. This makes it easy to check if two elements are in
     the same orbit. The permutation used to get this representative
     is stored in *usedPermutation (if pointer not 0).
   */
  ZVector orbitRepresentative(ZVector const &v, Permutation *usedPermutation=0)const;
  /**
     This routine works as orbitRepresentative() except that the
     symmetry group considered is only the subgroup keeping the vector
     fixed fixed.
   */
  ZVector orbitRepresentativeFixing(ZVector const &v, ZVector const &fixed)const;

  // Methods for highly optimized symmetry group computations:
//  void createByteTable();//Can only be called once. SymmetryGroup is not allowed to be changed afterwards or to be copied. Leaks memory at destruction.
  void createTrie();
//  unsigned char *getByteTable()const;
//  int getByteTableHeight()const;
};
/**
 * Sorts v and returns the number of swaps performed.
 */
int mergeSort(IntVector &v);
}




#endif /* GFAN_SYMMETRY_H_ */
