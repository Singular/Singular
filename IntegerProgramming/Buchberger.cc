/// Buchberger.cc

/// implementation of Buchberger's Algorithm.

#ifndef BUCHBERGER_CC
#define BUCHBERGER_CC

#include "ideal.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// S-pair computation ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOLEAN ideal::unnecessary_S_pair(list_iterator& first_iter,
                                  list_iterator& second_iter) const
{
/// This function checks several criteria to discard th S-pair of the
/// binomials referenced by the iterators. The criteria depend on the
/// settings of the ideal큦 S-pair flags.

/// The arguments are iterators instead of the referenced binomials
/// because we have to do some equality tests. These are more efficient on
/// iterators than on binomials.

/////////////////// criterion of relatively prime leading terms ////////////////////////////

  /// An S-pair can discarded if the leading terms of the two binomials are
  /// relatively prime.

  if(rel_primeness)
    if(relatively_prime(first_iter.get_element(),second_iter.get_element())
       ==TRUE)
      return TRUE;

////////////////// criterion M ////////////////////////////////////////////////////////////////////////////

  if(M_criterion)
  {

    list_iterator iter;
    binomial& bin1=first_iter.get_element();
    binomial& bin2=second_iter.get_element();

    /// The M-criterion of Gebauer/Moeller checks binomial triples as
    /// explained in binomial.h; these are built of the elements referenced
    /// by the argument iterators and a third element appearing before the
    /// element referenced by second_iter in the generator lists.


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    iter.set_to_list(generators);

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

    /// The support of the lcm of two monomials is the union of their supports.
    /// To test criterion M, we then only have to consider lists whose support
    /// is a subset of the union of (first_iter.get_element()).head_support and
    /// (second_iter.get_element()).head_support. As only elements before
    /// second_iter.get_element() are tested, we can stop iteraton as soon as
    /// we reach this element.

    int supp2=bin2.head_support%Number_of_Lists;
    int supp_union=(bin1.head_support%Number_of_Lists)|supp2;
    /// supp_union (read as binary vector) is the union of the supports of
    /// first_iter.get_element() and second_iter.get_element()
    /// (restricted to List_Support_Variables variables).

    for(int i=0;i<S.number_of_subsets[supp_union];i++)
      /// Go through the lists that contain elements whose support is a
      /// subset of supp_union.
    {
      int actual_list=S.subsets_of_support[supp_union][i];
      iter.set_to_list(generators[actual_list]);
      /// This is the i-th list among the generator list with elements
      /// whose support is a subset of supp_union.

      if(actual_list==supp2)
        break;
      /// The iteration has reached the list referenced by second_iter,
      /// this is handled alone to avoid unnecessary checks.
      /// Before breakin the loop, iter has to be set to this list.

      while(iter.is_at_end()==FALSE)
        /// Iterate over the list with three iterators according to the
        /// description of criterion M.
      {
        if(M(iter.get_element(),bin1,bin2)==TRUE)
          return TRUE;
        iter.next();
      }
    }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


    /// Now, iter references second_iter's list,
    /// if SUPPORT_DRIVEN_METHODS_EXTENDED are enabled or not.

    while(iter!=second_iter)
    {
      if(M(iter.get_element(),bin1,bin2)==TRUE)
        return TRUE;
      iter.next();
    }
  }

////////////////////////////////// criterion F ////////////////////////////////////////////////////////////

  if(F_criterion)
  {

    list_iterator iter;
    binomial& bin1=first_iter.get_element();
    binomial& bin2=second_iter.get_element();

    /// The F-criterion of Gebauer/Moeller checks binomial triples as
    /// explained in binomial.h; these are built of the elements referenced
    /// by the argument iterators and a third element appearing before the
    /// element referenced by first_iter in the generator lists.

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    iter.set_to_list(generators);

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

    /// Again, we only have to consider lists whose support is a subset of the
    /// union of (first_iter.get_element()).head_support and
    /// (second_iter.get_element()).head_support.
    /// Additionally,we can override lists whose support is to small.

    int supp1=bin1.head_support%Number_of_Lists;
    int supp2=bin2.head_support%Number_of_Lists;
    int supp_union=supp1|supp2;
    /// supp_union (read as binary vector) is the union of the supports of
    /// first_iter.get_element() and second_iter.get_element()
    /// (restricted to List_Support_Variables variables).

    for(int i=0;i<S.number_of_subsets[supp_union];i++)
      /// Go through the lists that contain elements whose support is a
      /// subset of supp_union.
    {
      int actual_list=S.subsets_of_support[supp_union][i];

      if((actual_list|supp2) != supp_union)
        continue;
      /// The support of the actual list is too small, so its elements cannot
      /// satisfie criterion F.

      iter.set_to_list(generators[actual_list]);
      /// This is the i-th list among the generator list with elements
      /// whose support is a subset of supp_union.

      if(actual_list==supp1)
        break;
      /// The iteration has reached the list referenced by first_iter;
      /// this is handled alone to avoid unnecessary checks.
      /// iter has to be set to that list before breaking the loop.

      while(iter.is_at_end()==FALSE)
        /// Iterate over the list with three iterators according to the
        /// description of criterion F.
      {
        if(F(iter.get_element(),bin1,bin2)==TRUE)
          return TRUE;
        iter.next();
      }
    }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED

    /// Now, iter references first_iter's list,
    /// if SUPPORT_DRIVEN_METHODS_EXTENDED are enabled or not.

    while(iter!=first_iter)
    {
      if(F(iter.get_element(),bin1,bin2)==TRUE)
        return TRUE;
      iter.next();
    }
  }

////////////////////////////////// criterion B /////////////////////////////////////////////////////////////

  if(B_criterion)
  {

    list_iterator iter;
    binomial& bin1=first_iter.get_element();
    binomial& bin2=second_iter.get_element();

    /// The B-criterion of Gebauer/Moeller checks binomial triples as
    /// explained in binomial.h; these are built of the elements referenced
    /// by the argument iterators and a third element appearing after the
    /// element referenced by second_iter in the generator lists.

    iter=second_iter;
    iter.next();

    /// First test second_iter's list.
    /// This is the only list if NO_SUPPORT_DRIVEN_METHODS are enabled.

    while(iter.is_at_end()==FALSE)
    {
      if(B(iter.get_element(),bin1,bin2)==TRUE)
        return(TRUE);
      iter.next();
    }

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

    /// Now consider the other lists.
    /// Again, we only have to consider lists whose support is a subset of the
    /// union of (first_iter.get_element()).head_support and
    /// (second_iter.get_element()).head_support.

    int supp2=bin2.head_support%Number_of_Lists;
    int supp_union=(bin1.head_support%Number_of_Lists)|supp2;
    /// supp_union (read as binary vector) is the union of the supports of
    /// first_iter.get_element() and second_iter.get_element()
    /// (restricted to List_Support_Variables variables).

    for(int i=0;i<S.number_of_subsets[supp_union];i++)
      /// Go through the lists that contain elements whose support is a
      /// subset of supp_union.
    {
      int actual_list=S.subsets_of_support[supp_union][i];

      if(actual_list<=supp2)
        continue;
      /// Only lists after second_iter's list have to be considered.

      iter.set_to_list(generators[actual_list]);
      /// This is the i-th list among the generator list with elements
      /// whose support is a subset of supp_union.

      while(iter.is_at_end()==FALSE)
      {
        /// Iterate over the list with three iterators according to the
        /// description of criterion B.
        if(B(iter.get_element(),bin1,bin2)==TRUE)
          return TRUE;
        iter.next();
      }
    }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED
  }

///////////////////////////////// Buchberger큦 second criterion ////////////////////////////////////

  if(second_criterion)
  {
    list_iterator iter;
    binomial& bin1=first_iter.get_element();
    binomial& bin2=second_iter.get_element();

    /// The Buchberger큦 second criterion checks binomial triples as
    /// explained in binomial.h; these are built of the elements referenced
    /// by the argument iterators and a third element appearing anywhere
    /// in the generator lists.

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    iter.set_to_list(generators);

    while(iter.is_at_end()==FALSE)
      /// Iterate over the list with three iterators according to the
      /// description of the second criterion.
    {
      if((iter!=first_iter) && (iter!=second_iter))
        /// Else the second criterion must not be applied
        /// (lcm(a,b) is, of course, divisible by a and by b).
        if(second_crit(iter.get_element(),bin1,bin2)==TRUE)
          return TRUE;
      iter.next();
    }

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

    /// Again, we only have to consider lists whose support is a subset of
    /// the union of (first_iter.get_element()).head_support
    /// and (second_iter.get_element()).head_support.

    int supp1=bin1.head_support%Number_of_Lists;
    int supp2=bin2.head_support%Number_of_Lists;
    int supp_union=supp1|supp2;
    /// supp_union (read as binary vector) is the union of the supports of
    /// first_iter.get_element() and second_iter.get_element()
    /// (restricted to List_Support_Variables variables)

    for(int i=0;i<S.number_of_subsets[supp_union];i++)
      /// Go through the lists that contain elements whose support is a
      /// subset of supp_union.
    {
      int actual_list=S.subsets_of_support[supp_union][i];

      if((actual_list==supp1) || (actual_list==supp2))
        continue;
      /// The lists containing the elements referenced by the argument
      /// iterators are tested separately to avoid unnecessary checks for
      /// equality.

      iter.set_to_list(generators[actual_list]);
      /// This is the i-th list among the generator list with elements
      /// whose support is a subset of supp_union.

      while(iter.is_at_end()==FALSE)
        /// Iterate over the list with three iterators according to the
        /// description of the second criterion.
      {
        if(second_crit(iter.get_element(),bin1,bin2)==TRUE)
          return TRUE;
        iter.next();
      }
    }

    if(supp1==supp2)
      /// The elements referenced by first_iter and second_iter appear in the
      /// same list.
    {
      iter.set_to_list(generators[supp1]);
      while(iter.is_at_end()==FALSE)
      {
        if((iter!=first_iter) && (iter!=second_iter))
          /// Else the second criterion must not be applied
          /// (lcm(a,b) is, of course, divisible by a and by b).
          if(second_crit(iter.get_element(),bin1,bin2)==TRUE)
            return TRUE;
        iter.next();
      }
    }
    else
      /// The elements referenced by first_iter and second_iter appear in
      /// different lists.
    {

      /// Test first_iter큦 list.
      iter.set_to_list(generators[supp1]);
      while(iter.is_at_end()==FALSE)
      {
        if(iter!=first_iter)
          /// Else the second criterion must not be applied
          /// (lcm(a,b) is, of course, divisible by a and by b).
          if(second_crit(iter.get_element(),bin1,bin2)==TRUE)
            return TRUE;
        iter.next();
      }

      /// Test second_iter큦 list.
      iter.set_to_list(generators[supp2]);
      while(iter.is_at_end()==FALSE)
      {
        if(iter!=second_iter)
          /// Else the second criterion must not be applied
          /// (lcm(a,b) is, of course, divisible by a and by b).
          if(second_crit(iter.get_element(),bin1,bin2)==TRUE)
            return TRUE;
        iter.next();
      }
    }
#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED
  }

  /// no criterion found to discard the S-Pair to compute
  return FALSE;
}

ideal& ideal::compute_actual_S_pairs_1()
{
/// This routine implements the simplest method for the S-pair computation
/// (and one of the most efficient methods). We simply iterate over the
/// generator list(s) with two pointers to look at each binomial pair.
/// The "done"-mark of each list element tells us if this element was
/// already considered in a previous S-pair computation; pairs of such
/// "old" binomials do not have to be computed anymore (but pairs of an old
/// and a new one have to be computed, of course). As the generator list are
/// ordered with respect to the "done"-flag (all undone elements preceed all
/// done elements), we can avoid unnecessary iteration steps by breaking
/// the iteration at the right point.

/// The computed S-pairs are stored in the aux_list for further computations.

/// For a better overview, the code for NO_SUPPORT_DRIVEN_METHODS_EXTENDED
/// and SUPPORT_DRIVEN_METHODS_EXTENDED is completetly separated in this
/// function.

/// Note that the "next()"-operations in the following routine do not reach a
/// NULL pointer because of the implementation of the "is_at_end()"-function
/// and because the "done"-component of the dummy element is set to zero.

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter(generators);

  while(first_iter.element_is_marked_done()==FALSE)
    /// new generator, compute S-pairs with all following generators
    /// Notice that the new generators are always at the beginning,
    /// the old generators at the end of the generator list.
  {
    binomial& bin=first_iter.get_element();
    first_iter.mark_element_done();

    list_iterator second_iter(first_iter);
    second_iter.next();
    /// This may be the dummy element.

    while(second_iter.is_at_end()==FALSE)
    {
      if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
         aux_list._insert(S_binomial(bin,second_iter.get_element(),w));
      second_iter.next();
    }

    first_iter.next();
  }

  /// Now, first_iter references an old generator or the end of the generator
  /// list. As all following generators are old ones, we are done.

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;

  for(int i=0;i<Number_of_Lists;i++)
  {
    first_iter.set_to_list(generators[i]);

    while(first_iter.element_is_marked_done()==FALSE)
      /// new generator, compute S-pairs with all following elements
      /// Notice that the new generators are always at the beginning,
      /// the old generators at the end of the generator lists.
    {
      binomial& bin=first_iter.get_element();
      first_iter.mark_element_done();

      /// First search over the actual list with the second iterator.

      list_iterator second_iter(first_iter);
      second_iter.next();

      while(second_iter.is_at_end()==FALSE)
      {
        if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          aux_list._insert(S_binomial(bin,second_iter.get_element(),w));
      second_iter.next();
      }

      /// Then search over the remaining lists.

      for(int j=i+1;j<Number_of_Lists;j++)
      {
        second_iter.set_to_list(generators[j]);

        while(second_iter.is_at_end()==FALSE)
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
            aux_list._insert(S_binomial(bin,second_iter.get_element(),w));
          second_iter.next();
        }
      }
      first_iter.next();
    }

    while(first_iter.is_at_end()==FALSE)
      /// old generator, compute only S-pairs with the following new generators
      /// (S-pairs with the following old generators were already computed
      /// before, first_iter.element_is_marked_done()==TRUE)
      /// As all generators in the actual list are old ones, we can
      /// start iteration with the next list.
    {
      binomial& bin=first_iter.get_element();

      list_iterator second_iter;

      for(int j=i+1;j<Number_of_Lists;j++)
        /// search over remaining lists
      {
        second_iter.set_to_list(generators[j]);

        while(second_iter.element_is_marked_done()==FALSE)
          /// consider only new generators
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
            aux_list._insert(S_binomial(bin,second_iter.get_element(),w));
          second_iter.next();
        }
      }
      first_iter.next();
    }

  }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED
  return(*this);
}

ideal& ideal::compute_actual_S_pairs_1a()
{
/// The only difference to the previous routine is that the aux_list is kept
/// ordered with respect to the ideal큦 term ordering, i.e. the inserts are
/// replaced by ordered inserts.

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter(generators);

  while(first_iter.element_is_marked_done()==FALSE)
    /// new generator, compute S-pairs with all following generators
    /// Notice that the new generators are always at the beginning,
    /// the old generators at the end of the generator list.
  {
    binomial& bin=first_iter.get_element();
    first_iter.mark_element_done();

    list_iterator second_iter(first_iter);
    second_iter.next();
    /// This may be the dummy element.

    while(second_iter.is_at_end()==FALSE)
    {
      if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
         aux_list._ordered_insert
           (S_binomial(bin,second_iter.get_element(),w),w);
      second_iter.next();
    }
    first_iter.next();
  }

  /// Now, first_iter references an old generator or the end of the generator
  /// list. As all following generators are old ones, we are done.

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;

  for(int i=0;i<Number_of_Lists;i++)
  {
    first_iter.set_to_list(generators[i]);

    while(first_iter.element_is_marked_done()==FALSE)
      /// new generator, compute S-pairs with all following elements
      /// Notice that the new generators are always at the beginning,
      /// the old generators at the end of the generator lists.
    {
      binomial& bin=first_iter.get_element();
      first_iter.mark_element_done();

      /// First search over the actual list with the second iterator.

      list_iterator second_iter(first_iter);
      second_iter.next();

      while(second_iter.is_at_end()==FALSE)
      {
        if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          aux_list._ordered_insert
            (S_binomial(bin,second_iter.get_element(),w),w);
      second_iter.next();
      }

      /// Then search over the remaining lists.

      for(int j=i+1;j<Number_of_Lists;j++)
      {
        second_iter.set_to_list(generators[j]);

        while(second_iter.is_at_end()==FALSE)
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
            aux_list._ordered_insert
              (S_binomial(bin,second_iter.get_element(),w),w);
          second_iter.next();
        }
      }
      first_iter.next();
    }

    while(first_iter.is_at_end()==FALSE)
      /// old generator, compute only S-pairs with the following new generators
      /// (S-pairs with the following old generators were already computed
      /// before, first_iter.element_is_marked_done()==TRUE)
      /// As all generators in the actual list are old ones, we can
      /// start iteration with the next list.
    {
      binomial& bin=first_iter.get_element();

      list_iterator second_iter;

      for(int j=i+1;j<Number_of_Lists;j++)
        /// search over remaining lists
      {
        second_iter.set_to_list(generators[j]);

        while(second_iter.element_is_marked_done()==FALSE)
          /// consider only new generators
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
            aux_list._ordered_insert
              (S_binomial(bin,second_iter.get_element(),w),w);
          second_iter.next();
        }
      }
      first_iter.next();
    }

  }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED

  return(*this);
}

ideal& ideal::compute_actual_S_pairs_2()
{
/// This routine implements are mor dynamic S-pair-computation. As in the
/// previous routines, we iterate over the generator list(s) with two
/// iterators, forming pairs under the consideration of the "done"-flags.

/// But before inserting into the aux_list, these S-pairs are reduced by
/// the ideal generators. This seems to be clever, but shows to be a
/// disadvantage:
/// In the previous S-pair routines, the computed S-bionomials are not reduced
/// at all. This is done in the appropriate Groebner basis routine
/// (reduced_Groebner_basis_1 or ..._1a) when moving them from the aux_list
/// to the generator lists. This meens that S-binomials cannot only be reduced
/// by the generators known at the time of their computation, but also by
/// the S-pairs that where already treated.

/// The advantage of the current routine is that the immediately reduced
/// S-binomial can be used to reduce the ideal itself. This strategy keeps
/// the ideal almost reduced, so the minimalization will be faster.
/// Furthermore, the computation of S-pairs with unreduced generators is
/// avoided.
/// To provide a possibility to compensate the mentionned disadvantage,
/// I have written the routine minimalize_S_pairs() that interreduces the
/// binomials stored in aux_list.

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter(generators);

  Integer first_reduced=0;
  Integer second_reduced=0;
  /// When reducing the ideal immediately by newly found generators, it
  /// can happen that the binomials referenced by the iterators are
  /// reduced to zero and then deleted. We need to make sure that the
  /// iterators do not reference freed memory in such a case. These two
  /// flags help us with this task.

  while(first_iter.element_is_marked_done()==FALSE)
    /// new generator, compute S-pairs with all following generators
  {
    binomial& bin1=first_iter.get_element();
    first_iter.mark_element_done();

    list_iterator second_iter(first_iter);
    second_iter.next();
    /// This may be the dummy element.

    while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
    {
      if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
      {
        binomial& bin2=second_iter.get_element();

        /// compute S-binomial
        binomial& S_bin=S_binomial(bin1,bin2,w);

        /// reduce S-binomial by the actual ideal generators
        reduce(S_bin,FALSE);

        if(S_bin!=0)
        {
          /// reduce the ideal generators by the S-binomial
          first_reduced=bin1.head_reductions_by(S_bin);
          second_reduced=bin2.head_reductions_by(S_bin);
          reduce_by(S_bin,first_iter,second_iter);
          aux_list._insert(S_bin);
        }
        else
          delete &S_bin;

      }

      /// Move second_iter to the next element if its referenced binomial
      /// has not changed (if it has changed, the binomial was moved to the
      /// aux_list during the reduce_by(...)-procedure, and second_iter
      /// already references a new binomial).
      if(second_reduced<=0)
        second_iter.next();
      else
        second_reduced=0;

    }

    /// same procedure for first_iter
    if(first_reduced<=0)
      first_iter.next();
    else
      first_reduced=0;
  }

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;

  Integer first_reduced=0;
  Integer second_reduced=0;
  /// When reducing the ideal immediately by newly found generators, it
  /// can happen that the binomials referenced by the iterators are
  /// changed (including their support!) or even reduced to zero and then
  /// deleted. We need to make sure that the iterators do not reference
  /// freed memory in such a case. These two flags help us with this task.


  for(int i=0;i<Number_of_Lists;i++)
  {
    first_iter.set_to_list(generators[i]);

    while(first_iter.element_is_marked_done()==FALSE)
      /// new generator, compute S-pairs with all following elements
    {
      binomial& bin1=first_iter.get_element();
      first_iter.mark_element_done();

      list_iterator second_iter(first_iter);
      second_iter.next();

      /// First search over the actual list.

      while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
      {
        if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
        {
          binomial& bin2=second_iter.get_element();

          /// compute S-binomial
          binomial& S_bin=S_binomial(bin1,bin2,w);

          /// reduce S-binomial by the actual ideal generators
          reduce(S_bin,FALSE);

          if((S_bin)!=0)
          {
            /// reduce the ideal generators by the S-binomial
            first_reduced=bin1.head_reductions_by(S_bin);
            second_reduced=bin2.head_reductions_by(S_bin);
            reduce_by(S_bin,first_iter,second_iter);
            aux_list._insert(S_bin);
          }
          else
            delete &S_bin;

        }

        /// Move second_iter to the next element if its referenced binomial
        /// has not changed (if it has changed, the binomial was moved to the
        /// aux_list during the reduce_by(...)-procedure, and second_iter
        /// already references a new binomial).
        if(second_reduced<=0)
          second_iter.next();
        else
          second_reduced=0;

      }

      /// Then search over the remaining lists.

      for(int j=i+1;(j<Number_of_Lists) && (first_reduced<=0);j++)
      {
        second_iter.set_to_list(generators[j]);

        while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          {
            binomial& bin2=second_iter.get_element();

            /// compute S-binomial
            binomial& S_bin=S_binomial(bin1,bin2,w);

            /// reduce S-binomial by the actual ideal generators
            reduce(S_bin,FALSE);

            if((S_bin)!=0)
            {
              /// reduce the ideal generators by the S-binomial
              first_reduced=bin1.head_reductions_by(S_bin);
              second_reduced=bin2.head_reductions_by(S_bin);
              reduce_by(S_bin,first_iter,second_iter);
              aux_list._insert(S_bin);
            }
            else
              delete& S_bin;

          }

          /// Move second_iter to the next element if its referenced binomial
          /// has not changed.
          if(second_reduced<=0)
            second_iter.next();
          else
            second_reduced=0;
        }
      }

      /// same procedure for first_iter
      if(first_reduced<=0)
        first_iter.next();
      else
        first_reduced=0;

    }


    while(first_iter.is_at_end()==FALSE)
      /// old generator, compute only S-pairs with the following new generators
      /// As all generators in the actual list are old ones, we can
      /// start iteration with the next list.
    {
      binomial& bin1=first_iter.get_element();

      list_iterator second_iter(first_iter);
      second_iter.next();

     for(int j=i+1;(j<Number_of_Lists) && (first_reduced<=0);j++)
      {
        second_iter.set_to_list(generators[j]);

        while((second_iter.element_is_marked_done()==FALSE) &&
              (first_reduced<=0))
          /// consider only new generators
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          {
            binomial& bin2=second_iter.get_element();

            /// compute S-binomial
            binomial& S_bin=S_binomial(bin1,bin2,w);

            /// reduce S-binomial by the actual ideal generators
            reduce(S_bin,FALSE);

            if((S_bin)!=0)
            {
              /// reduce the ideal generators by the S-binomial
              first_reduced=bin1.head_reductions_by(S_bin);
              second_reduced=bin2.head_reductions_by(S_bin);
              reduce_by(S_bin,first_iter,second_iter);
              aux_list._insert(S_bin);
            }
            else
              delete& S_bin;

          }

          /// Move second_iter to the next element if its referenced binomial
          /// has not changed.
          if(second_reduced<=0)
            second_iter.next();
          else
            second_reduced=0;
        }
      }

      /// same procedure for first_iter
      if(first_reduced<=0)
        first_iter.next();
      else
        first_reduced=0;

    }
  }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


  return(*this);
}





ideal& ideal::compute_actual_S_pairs_3()
{
/// This routine is quite similar to the preceeding.
/// The main difference is that the computed S-binomials are not stored in the
/// aux_list, but in new_generators. This makes a difference when minimalizing
/// the S-binomials in the appropriate Groebner basis routine
/// (reduced_Groebner_basis_3) with the help of the procedure
/// minimalize_new_generators(...).
/// If NO_SUPPORT_DRIVEN_METHODS_EXTENDED are enabled, only the organization
/// of the minimalization is different.
/// If SUPPORT_DRIVEN_METHODS_EXTENDED are enabled, the minimalization can
/// use the support information.


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter(generators);

  Integer first_reduced=0;
  Integer second_reduced=0;
  /// When reducing the ideal immediately by newly found generators, it
  /// can happen that the binomials referenced by the iterators are
  /// reduced to zero and then deleted. We need to make sure that the
  /// iterators do not reference freed memory in such a case. These two
  /// flags help us with this task.

  while(first_iter.element_is_marked_done()==FALSE)
    /// new generator, compute S-pairs with all following generators
  {
    binomial& bin1=first_iter.get_element();
    first_iter.mark_element_done();

    list_iterator second_iter(first_iter);
    second_iter.next();
    /// This may be the dummy element.

    while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
    {
      if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
      {
        binomial& bin2=second_iter.get_element();

        /// compute S-binomial
        binomial& S_bin=S_binomial(bin1,bin2,w);

        /// reduce S-binomial by the actual ideal generators
        reduce(S_bin,FALSE);

        if(S_bin!=0)
        {
          /// reduce the ideal generators by the S-binomial
          first_reduced=bin1.head_reductions_by(S_bin);
          second_reduced=bin2.head_reductions_by(S_bin);
          reduce_by(S_bin,first_iter,second_iter);
          add_new_generator(S_bin);
        }
        else
          delete &S_bin;

      }

      /// Move second_iter to the next element if its referenced binomial
      /// has not changed (if it has changed, the binomial was moved to the
      /// aux_list during the reduce_by(...)-procedure, and second_iter
      /// already references a new binomial).
      if(second_reduced<=0)
        second_iter.next();
      else
        second_reduced=0;

    }

    /// same procedure for first_iter
    if(first_reduced<=0)
      first_iter.next();
    else
      first_reduced=0;
  }

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;

  Integer first_reduced=0;
  Integer second_reduced=0;
  /// When reducing the ideal immediately by newly found generators, it
  /// can happen that the binomials referenced by the iterators are
  /// changed (including their support!) or even reduced to zero and then
  /// deleted. We need to make sure that the iterators do not reference
  /// freed memory in such a case. These two flags help us with this task.


  for(int i=0;i<Number_of_Lists;i++)
  {
    first_iter.set_to_list(generators[i]);

    while(first_iter.element_is_marked_done()==FALSE)
      /// new generator, compute S-pairs with all following elements
    {
      binomial& bin1=first_iter.get_element();
      first_iter.mark_element_done();

      list_iterator second_iter(first_iter);
      second_iter.next();

      /// First search over the actual list.

      while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
      {
        if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
        {
          binomial& bin2=second_iter.get_element();

          /// compute S-binomial
          binomial& S_bin=S_binomial(bin1,bin2,w);

          /// reduce S-binomial by the actual ideal generators
          reduce(S_bin,FALSE);

          if((S_bin)!=0)
          {
            /// reduce the ideal generators by the S-binomial
            first_reduced=bin1.head_reductions_by(S_bin);
            second_reduced=bin2.head_reductions_by(S_bin);
            reduce_by(S_bin,first_iter,second_iter);
            add_new_generator(S_bin);
          }
          else
            delete &S_bin;

        }

        /// Move second_iter to the next element if its referenced binomial
        /// has not changed (if it has changed, the binomial was moved to the
        /// aux_list during the reduce_by(...)-procedure, and second_iter
        /// already references a new binomial).
        if(second_reduced<=0)
          second_iter.next();
        else
          second_reduced=0;

      }

      /// Then search over the remaining lists.

      for(int j=i+1;(j<Number_of_Lists) && (first_reduced<=0);j++)
      {
        second_iter.set_to_list(generators[j]);

        while((second_iter.is_at_end()==FALSE) && (first_reduced<=0))
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          {
            binomial& bin2=second_iter.get_element();

            /// compute S-binomial
            binomial& S_bin=S_binomial(bin1,bin2,w);

            /// reduce S-binomial by the actual ideal generators
            reduce(S_bin,FALSE);

            if((S_bin)!=0)
            {
              /// reduce the ideal generators by the S-binomial
              first_reduced=bin1.head_reductions_by(S_bin);
              second_reduced=bin2.head_reductions_by(S_bin);
              reduce_by(S_bin,first_iter,second_iter);
              add_new_generator(S_bin);
            }
            else
              delete& S_bin;

          }

          /// Move second_iter to the next element if its referenced binomial
          /// has not changed.
          if(second_reduced<=0)
            second_iter.next();
          else
            second_reduced=0;
        }
      }

      /// same procedure for first_iter
      if(first_reduced<=0)
        first_iter.next();
      else
        first_reduced=0;

    }


    while(first_iter.is_at_end()==FALSE)
      /// old generator, compute only S-pairs with the following new generators
      /// As all generators in the actual list are old ones, we can
      /// start iteration with the next list.
    {
      binomial& bin1=first_iter.get_element();

      list_iterator second_iter(first_iter);
      second_iter.next();

     for(int j=i+1;(j<Number_of_Lists) && (first_reduced<=0);j++)
      {
        second_iter.set_to_list(generators[j]);

        while((second_iter.element_is_marked_done()==FALSE) &&
              (first_reduced<=0))
          /// consider only new generators
        {
          if(unnecessary_S_pair(first_iter,second_iter)==FALSE)
          {
            binomial& bin2=second_iter.get_element();

            /// compute S-binomial
            binomial& S_bin=S_binomial(bin1,bin2,w);

            /// reduce S-binomial by the actual ideal generators
            reduce(S_bin,FALSE);

            if((S_bin)!=0)
            {
              /// reduce the ideal generators by the S-binomial
              first_reduced=bin1.head_reductions_by(S_bin);
              second_reduced=bin2.head_reductions_by(S_bin);
              reduce_by(S_bin,first_iter,second_iter);
              add_new_generator(S_bin);
            }
            else
              delete& S_bin;

          }

          /// Move second_iter to the next element if its referenced binomial
          /// has not changed.
          if(second_reduced<=0)
            second_iter.next();
          else
            second_reduced=0;
        }
      }

      /// same procedure for first_iter
      if(first_reduced<=0)
        first_iter.next();
      else
        first_reduced=0;

    }
  }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


  /// During the reduce_by(...)-routines, some reduced generators were
  /// perhaps moved to the aux_list. This list is emptied now: Like the
  /// computed S-pairs, its elements are moved to the list(s) new_generators.
  /// To avoid this construction, we would have to write a second version
  /// of the reduce_by(...)-procedure. The efficiency gains would however
  /// not be considerable.

  list_iterator iter(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    add_new_generator(iter.get_element());
    iter.extract_element();
  }


  return(*this);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// minimalization / autoreduction ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





ideal& ideal::reduce_by(const binomial& bin, list_iterator& first_iter,
                        list_iterator& second_iter)
{
/// This routine reduces the ideal by the argument binomial and takes
/// care that the argument list iterators are not corrupted.
/// Only head reductions are performed.


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator iter(generators);
  Integer reduced;

  while(iter.is_at_end()==FALSE)
  {
    binomial& actual=iter.get_element();

    /// reduce actual binomial by bin
    reduced=actual.head_reductions_by(bin);

    if(reduced<=0)
      iter.next();

    else
      /// the actual binomial has changed and will be removed or
      /// moved to the aux_list
    {

#ifdef SL_LIST

      /// If we use a simply linked list, we have to take care of the
      /// following binomial.
      if(iter.next_is(first_iter)==TRUE)
        first_iter=iter;
      if(iter.next_is(second_iter)==TRUE)
        second_iter=iter;

#endif  /// SL_LIST


#ifdef DL_LIST

      /// If we use a doubly linked list, we have to take care of the
      /// binomial itself.
      if(iter==first_iter)
        first_iter.next();
      if(iter==second_iter)
        second_iter.next();

#endif  /// DL_LIST


      /// move changed generator to the aux_list or delete it
      if(actual==0)
        iter.delete_element();
      else
      {
        aux_list._insert(actual);
        iter.extract_element();
      }

      size--;
    }
  }

#endif  /// NO_SUPPORT_DRIVEN_METHOD_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  int supp1=(first_iter.get_element()).head_support%Number_of_Lists;
  int supp2=(second_iter.get_element()).head_support%Number_of_Lists;

  /// Determine the lists over which we have to iterate.
  /// These are the lists with elements whose support contains the support
  /// of bin.
  /// List i has to be checked iff the set of bits in i that are 1 contains
  /// the set of bits in supp that are 1.
  /// Equivalent: List i has to be checked iff the set of bits in i
  /// that are 0 is contained in the set of bits of supp that are 0.
  /// With this formulation, we can use the subset tree as follows:

  int supp=bin.head_support%Number_of_Lists;
  int inv_supp=Number_of_Lists-supp-1;
  /// This bit vector is the bitwise inverse of bin.head_support (restricted
  /// to the variables considered in the list indices.

  list_iterator iter;
  Integer reduced;


  for(int i=0;i<S.number_of_subsets[inv_supp];i++)
  {
    int actual_list=Number_of_Lists-S.subsets_of_support[inv_supp][i]-1;
    /// the actual list for iteration
    /// The support of S.subsets_of_support[inv_supp][i] as a bit vector
    /// is contained in that of inv_supp.
    /// I.e. the support of
    /// Number_of_Lists-S.subsets_of_support[inv_supp][i]-1
    /// - which is the bitwise inverse of S.subsets_of_support[inv_supp][i] -
    /// contains the support of the bitwise inverse of inv_supp, hence the
    /// support of supp.

    if((actual_list==supp1) || (actual_list==supp2))
      /// The lists referenced by first_iter and second_iter are tested
      /// separately to avoid unnecessary checks.
      continue;

    iter.set_to_list(generators[actual_list]);

    while(iter.is_at_end()==FALSE)
    {
      binomial& actual=iter.get_element();

      /// reduce actual binomial by bin
      reduced=actual.head_reductions_by(bin);

      if(reduced<=0)
        iter.next();

      else
        /// the actual binomial has changed and will be removed or
      {
        if(actual==0)
          iter.delete_element();

        else
        {
          aux_list._insert(actual);
          iter.extract_element();
        }

        size--;
      }
    }
  }

  /// Now test the lists referenced by first_iter and second_iter.

  if(supp1==supp2)
    /// first_iter and second_iter reference the same list
  {
    if((supp1|supp)==supp1)
      /// support of bin contained in that of the element referenced by
      /// first_iter, else this list has not to be tested
    {
      iter.set_to_list(generators[supp1]);

      while(iter.is_at_end()==FALSE)
      {
        binomial& actual=iter.get_element();
        reduced=actual.head_reductions_by(bin);
        if(reduced<=0)
          iter.next();
        else
        {

#ifdef SL_LIST

          if(iter.next_is(first_iter)==TRUE)
            first_iter=iter;
          if(iter.next_is(second_iter)==TRUE)
            second_iter=iter;

#endif  /// SL_LIST

#ifdef DL_LIST

          if(iter==first_iter)
            first_iter.next();
          if(iter==second_iter)
            second_iter.next();

#endif  /// DL_LIST

          if(actual==0)
            iter.delete_element();
          else
          {
            aux_list._insert(actual);
            iter.extract_element();
          }

          size--;
        }
      }
    }
  }

  else
    /// first_iter and second_iter reference different lists
  {
    if((supp1|supp)==supp1)
      /// support of bin contained in that of the element referenced by
      /// first_iter, else this list has not to be tested
    {
      iter.set_to_list(generators[supp1]);

      while(iter.is_at_end()==FALSE)
      {
        binomial& actual=iter.get_element();
        reduced=actual.head_reductions_by(bin);
        if(reduced<=0)
          iter.next();
        else
        {

#ifdef SL_LIST

          if(iter.next_is(first_iter)==TRUE)
            first_iter=iter;

#endif  /// SL_LIST

#ifdef DL_LIST

          if(iter==first_iter)
            first_iter.next();

#endif  /// DL_LIST

          if(actual==0)
            iter.delete_element();
          else
          {
            aux_list._insert(actual);
            iter.extract_element();
          }

          size--;
        }
      }
    }

    if((supp2|supp)==supp2)
      /// support of bin contained in that of the element referenced by
      /// second_iter, else this list has not to be tested
    {
      iter.set_to_list(generators[supp2]);

      while(iter.is_at_end()==FALSE)
      {
        binomial& actual=iter.get_element();
        reduced=actual.head_reductions_by(bin);
        if(reduced<=0)
          iter.next();
        else
        {

#ifdef SL_LIST

          if(iter.next_is(second_iter)==TRUE)
            second_iter=iter;

#endif  /// SL_LIST

#ifdef DL_LIST

          if(iter==second_iter)
            second_iter.next();

#endif  /// DL_LIST

          if(actual==0)
            iter.delete_element();
          else
          {
            aux_list._insert(actual);
            iter.extract_element();
          }

          size--;
        }
      }
    }
  }

#endif

  return *this;

}





ideal& ideal::minimalize_S_pairs()
{
/// This routine implements a very simple minimalization method. We iterate
/// over the S-pair lists with two iterators, interreducing the two referenced
/// binomials. Remember that the S-pair list does not use the "head_reduced"-
/// flags. The iteration is repeated as long as some interreduction is done.

  list_iterator first_iter;
  int found;
  /// to control if a reduction has occurred during the actual iteration

  do
  {

    first_iter.set_to_list(aux_list);
    found=0;
    /// no reduction occurred yet

    while(first_iter.is_at_end()==FALSE)
    {

      binomial& bin1=first_iter.get_element();
      int first_changed=0;
      /// to control if the first element has been reduced

      /// look at all following binomials
      list_iterator second_iter(first_iter);
      second_iter.next();
      /// this may be the dummy element

      while(second_iter.is_at_end()==FALSE)
      {
        binomial& bin2=second_iter.get_element();
        int second_changed=0;

        if(bin1.reduce_head_by(bin2,w)!=0)
          /// head of first binomial can be reduced by second
        {

          if(bin1!=0)
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials)

          else
            /// the binomial referenced by bin1 is zero
          {

#ifdef SL_LIST

            if(first_iter.next_is(second_iter))
              second_iter.next();

#endif  /// SL_LIST

            first_iter.delete_element();
            first_changed=1;
          }

          break;
          /// Breaks the while-loop.
          /// As the element referenced by first_iter has changed,
          /// the iteration with the second iterator can be restarted.
          /// (We try to reduce as many elements as possible in one iteration.)
        }


        if(bin2.reduce_head_by(bin1,w)!=0)
           /// head of second binomial can be reduced by first
        {

          if(bin2!=0)
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials)

          else
            /// binomial referenced by bin2 is zero
          {
            second_iter.delete_element();
            second_changed=1;
          }

          /// As the second iterator always references an element coming
          /// after first_iter's element in the generator list, we do not
          /// pay attention to the deletion...
        }

        if(second_changed==0)
          second_iter.next();
      }


      /// Now second_iter has reached the end of the generator list or the
      /// element referenced by first_iter has been reduced...The iteration
      /// is continued with a new (or changed) first binomial.

      if(first_changed==0)
        first_iter.next();
    }

 }
  while(found==1);

  /// When leaving the loop, no generators have been interreduced during
  /// the last iteration.

  return *this;
}





ideal& ideal::minimalize_new_generators()
{
/// This routine is very similar to the following one, minimalize().
/// The only difference is that we interreduce the elements stored in
/// new_generators instead of those stored in generators.
/// The size of the ideal has not to be manipulated hereby.


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;
  int found;
  /// to control if a reduction has occurred during the actual iteration

  do
  {
    first_iter.set_to_list(new_generators);
    found=0;
    /// no reduction occurred yet

    while(first_iter.element_is_marked_head_reduced()==FALSE)
      /// only the first element is tested for this flag
      /// the second may be an old one
    {
      binomial& bin1=first_iter.get_element();
      int first_changed=0;
      /// to control if the first element has been reduced

      /// look at all following binomials
      list_iterator second_iter(first_iter);
      second_iter.next();
      /// this may be the dummy element

      while(second_iter.is_at_end()==FALSE)
      {
        binomial& bin2=second_iter.get_element();

        if(bin1.reduce_head_by(bin2,w)!=0)
          /// head of first binomial can be reduced by second
        {

#ifdef SL_LIST

          /// The binomial referenced by first_iter will be deleted or
          /// extracted. When using a simply linked list, this also affects
          /// the following element. We need to assure that this element does
          /// not reference freed memory.
          if(first_iter.next_is(second_iter))
              second_iter.next();

#endif  /// SL_LIST

          if(bin1!=0)
          {
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials to insert)

            aux_list._insert(bin1);
            first_iter.extract_element();
            /// moved changed binomial to aux_list
          }

          else
            first_iter.delete_element();


          first_changed=1;
          break;
          /// As the binomial referenced by first_iter has changed,
          /// the iteration with the second iterator can be restarted.
          /// (We try to reduce as many elements as possible in one iteration.)
        }


        if(bin2.reduce_head_by(bin1,w)!=0)
           /// head of second binomial can be reduced by first
        {
          /// Here we do not have to pay attention to the deletion or the
          /// extraction of the element referenced by second_iter because
          /// the element referenced by second_iter always comes after the
          /// element referenced by first_iter in new_generators.

          if(bin2!=0)
          {
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials to insert)

            aux_list._insert(bin2);
            second_iter.extract_element();
            /// move the element referenced by second_iter to aux_list
          }

          else
            second_iter.delete_element();

          /// Here it makes not sense to restart iteration because the
          /// deletion sets the pointers as desired.
        }

        else
          /// no reduction possible
          second_iter.next();

      }

      /// Now second_iter has reached the end of the list new_generators or the
      /// binomial referenced first_iter has been reduced...The iteration
      /// is continued with a new (or changed) first binomial.

      if(first_changed==0)
        first_iter.next();
    }


    /// Now we have found all currently possible reductions.
    /// The elements remaining in new_generators cannot be interreduced
    /// and are marked head_reduced.

    first_iter.set_to_list(new_generators);
    ///   if(first_iter.is_at_end()==FALSE)
      while(first_iter.element_is_marked_head_reduced()==FALSE)
      {
        first_iter.mark_element_head_reduced();
        first_iter.next();
      }


    /// Now reinsert reduced elements.

    first_iter.set_to_list(aux_list);

    while(first_iter.is_at_end()==FALSE)
    {
      binomial& bin=first_iter.get_element();

      reduce(bin,FALSE);
      /// The binomial was only reduced by one other binomial before it was
      /// moved to aux_list. To reduce it by all other binomials now can
      /// diminish the number of iterations (do-while-loop).

      if(bin==0)
        first_iter.delete_element();
      else
      {
        add_new_generator(bin);
        first_iter.extract_element();
      }

    }

  }
  while(found==1);

  /// When leaving the loop, no generators have been interreduced during
  /// the last iteration; we are done.

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;
  list_iterator second_iter;
  int found;
  /// to control if a reduction has occurred during the actual iteration

  do
  {
    found=0;
    /// no reduction occurred yet

    for(int i=0;i<Number_of_Lists;i++)
    {
      first_iter.set_to_list(new_generators[i]);

/// First try to reduce the binomials that are marked unreduced.

      while(first_iter.element_is_marked_head_reduced()==FALSE)
        /// all other elements have to be tested for interreduction
      {

        binomial& bin=first_iter.get_element();
        Integer changed=0;
        /// binomial referenced by bin not yet reduced

/// Look for head reductions:
/// Iterate over the lists that could contain reducers for the head of bin.
/// The list containing bin is tested separately to avoid an interreduction
/// of a binomial by itself respectively unnecessary checks for this when the
/// two iterators reference different lists.

        for(int j=0;(j<S.number_of_subsets[i]-1)&&(changed==0);j++)
        {
          second_iter.set_to_list(new_generators[S.subsets_of_support[i][j]]);
          /// This is the j-th list among the new_generator lists with elements
          /// whose support is a subset of that of i.
          /// The support of i is just the head support of bin (restricted
          /// to the corresponding variables).

          while((second_iter.is_at_end()==FALSE)&&(changed==0))
          {
            changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {
              if(bin!=0)
              {
                found=1;
                /// found has only to be set if the binomial has not been
                /// reduced to zero (else there are no new binomials)

                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();
            }
            second_iter.next();
          }
        }

        /// The list new_generators[i]
        /// =new_generators[S.subsets_of_support[i][S.number_of_subsets[i]-1]]
        /// has to be tested separately to avoid a reduction of the actual
        /// binomial by itself.

        if(changed==0)
          /// binomial referenced by first_iter has not yet been reduced
        {
          second_iter.set_to_list(new_generators[i]);

          while((second_iter.is_at_end()==FALSE) && (changed==0))
          {
            if(second_iter!=first_iter)
              /// the two iterators do not reference the same element
              changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {

#ifdef SL_LIST

              /// bin will be deleted ar extracted - maybe dangerous for
              /// second_iter
              if(first_iter.next_is(second_iter))
                second_iter.next();

#endif  /// SL_LIST

              if(bin!=0)
              {
                found=1;
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();
            }

            else
              /// bin has not been reduced
              second_iter.next();
          }
        }

      if(changed==0)
        first_iter.next();
      /// Else first_iter has already been set to the next element by deletion
      /// or extraction.
      }


///  Now try to reduce the binomials that are marked reduced.

      while(first_iter.is_at_end()==FALSE)
        /// only unreduced elements have to be tested for interreduction
      {
        binomial& bin=first_iter.get_element();
        Integer changed=0;
        /// binomial referenced by bin not yet reduced

/// Look for head reductions:
/// Iterate over the lists that could contain reducers for the head of bin.
/// The list containing bin is tested separately to avoid an interreduction
/// of a binomial by itself respectively unnecessary checks for this when the
/// two iterators reference different lists.

        for(int j=0;(j<S.number_of_subsets[i]-1)&&(changed==0);j++)
        {
          second_iter.set_to_list(new_generators[S.subsets_of_support[i][j]]);
          /// This is the j-th list among the new_generators lists with elements
          /// whose support is a subset of that of i.
          /// The support of i is just the head support of bin (restricted
          /// to the corresponding variables).

          while((second_iter.element_is_marked_head_reduced()==FALSE) &&
                (changed==0))
          {
            changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {
              if(bin!=0)
              {
                found=1;
                /// found has only to be set if the binomial has not been
                /// reduced to zero (else there are no new binomials)
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();
            }
            second_iter.next();
          }
        }

        /// The list new_generators[i]
        /// =new_generators[S.subsets_of_support[i][S.number_of_subsets[i]-1]]
        /// has to be tested separately to avoid a reduction of the actual
        /// binomial by itself.

        if(changed==0)
          /// binomial referenced by first_iter has not yet been reduced
        {
          second_iter.set_to_list(new_generators[i]);

          while((second_iter.element_is_marked_head_reduced()==FALSE) &&
                (changed==0))
          {
            if(second_iter!=first_iter)
              /// the two iterators do not reference the same element
              changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {

#ifdef SL_LIST

              /// bin will be deleted ar extracted - maybe dangerous for
              /// second_iter
              if(first_iter.next_is(second_iter))
                second_iter.next();

#endif  /// SL_LIST

              if(bin!=0)
              {
                found=1;
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();
            }

            else
              /// bin has not been reduced
              second_iter.next();
          }
        }

      if(changed==0)
        first_iter.next();
      /// Else first_iter has already been set to the next element by deletion
      /// or extraction.
      }

    }


    /// Now we have found all currently possible reductions.
    /// The elements remaining in the new_generator lists cannot be interreduced
    /// and are marked reduced.

    for(int i=0;i<Number_of_Lists;i++)
    {
      first_iter.set_to_list(new_generators[i]);
      if(first_iter.is_at_end()==FALSE)
        while(first_iter.element_is_marked_head_reduced()==FALSE)
        {
          first_iter.mark_element_head_reduced();
          first_iter.next();
        }
    }


    /// Now reinsert reduced elements
    /// It seems to be quite unimportant for the performance if an element is
    /// completely reduced before reinsertion or not.

    first_iter.set_to_list(aux_list);
    while(first_iter.is_at_end()==FALSE)
    {
      binomial& bin=first_iter.get_element();
      reduce(bin,FALSE);

      if(bin==0)
        first_iter.delete_element();
      else
      {
        add_new_generator(bin);
        first_iter.extract_element();
      }
    }

  }
  while(found==1);

  /// When leaving the loop, no generators have been interreduced during
  /// the last iteration; we are done.

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


  return(*this);

}





ideal& ideal::minimalize()
{
/// For a better overview, the code for NO_SUPPORT_DRIVEN_METHODS_EXTENDED
/// and SUPPORT_DRIVEN_METHODS_EXTENDED is completetly separated in this
/// function. Note that th iteration methods are quite different for those
/// two possibilities.


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

/// For technical simplicity, the interreduction is done as follows:
/// We iterate over the generators with two iterators.
/// For each binomial pair, we examine if one binomial큦 head can be
/// reduced by the other. If this is the case, the reducible binomial is
/// reduced and moved to the aux_list if it is not 0, else deleted.
/// After one iteration, the elements of the aux_list are reinserted;
/// then the interreduction is restarted until no new elements are found.

/// The above method of deleting and inserting is chosen for the following
/// reasons:
/// - The order undone elements - done elements in the generator lists
///   is not destroyed. Newly found elements are automatically marked
///   undone when they are reinserted.
///   The S-pair computation can as usually make use of this fact.
/// - In analogy, the order head_unreduced elements - head_reduced elements
///   is not destroyed. Remark that an undone element can never be reduced, as
///   reduction is only done after an S-pair computation. Newly found
///   elements are automatically marked unreduced. With the "head_reduced"-
///   flag we make sure that any binomial pair is tested only once for
///   interreduction during the whole algorithm.

  list_iterator first_iter;
  int found;
  /// to control if a reduction has occurred during the actual iteration

  do
  {
    first_iter.set_to_list(generators);
    found=0;
    /// no reduction occurred yet

    while(first_iter.element_is_marked_head_reduced()==FALSE)
      /// only the first element is tested for this flag
      /// the second may be an old one
    {
      binomial& bin1=first_iter.get_element();
      int first_changed=0;
      /// to control if the first element has been reduced

      /// look at all following binomials
      list_iterator second_iter(first_iter);
      second_iter.next();
      /// this may be the dummy element

      while(second_iter.is_at_end()==FALSE)
      {
        binomial& bin2=second_iter.get_element();

        if(bin1.reduce_head_by(bin2,w)!=0)
          /// head of first binomial can be reduced by second
        {

#ifdef SL_LIST

          /// The binomial referenced by first_iter will be deleted or
          /// extracted. When using a simply linked list, this also affects
          /// the following element. We need to assure that this element does
          /// not reference freed memory.
          if(first_iter.next_is(second_iter))
              second_iter.next();

#endif  /// SL_LIST

          if(bin1!=0)
          {
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials to insert)

            aux_list._insert(bin1);
            first_iter.extract_element();
            /// moved changed binomial to aux_list
          }

          else
            first_iter.delete_element();


          first_changed=1;
          size--;
          break;
          /// As the binomial referenced by first_iter has changed,
          /// the iteration with the second iterator can be restarted.
          /// (We try to reduce as many elements as possible in one iteration.)
        }


        if(bin2.reduce_head_by(bin1,w)!=0)
           /// head of second binomial can be reduced by first
        {
          /// Here we do not have to pay attention to the deletion or the
          /// extraction of the element referenced by second_iter because
          /// the element referenced by second_iter always comes after the
          /// element referenced by first_iter in the generator list.

          if(bin2!=0)
          {
            found=1;
            /// found has not to be set if a binomial is reduced to zero
            /// (then there are no new binomials to insert)

            aux_list._insert(bin2);
            second_iter.extract_element();
            /// move the element referenced by second_iter to aux_list
          }

          else
            second_iter.delete_element();

          size--;
          /// Here it makes not sense to restart iteration because the
          /// deletion sets the pointers as desired.
        }

        else
          /// no reduction possible
          second_iter.next();

      }

      /// Now second_iter has reached the end of the generator list or the
      /// binomial referenced first_iter has been reduced...The iteration
      /// is continued with a new (or changed) first binomial.

      if(first_changed==0)
        first_iter.next();
    }


    /// Now we have found all currently possible reductions.
    /// The elements remaining in the generator list cannot be interreduced
    /// and are marked head_reduced.

    first_iter.set_to_list(generators);
    ///   if(first_iter.is_at_end()==FALSE)
      while(first_iter.element_is_marked_head_reduced()==FALSE)
      {
        first_iter.mark_element_head_reduced();
        first_iter.next();
      }


    /// Now reinsert reduced elements.

    first_iter.set_to_list(aux_list);

    while(first_iter.is_at_end()==FALSE)
    {
      binomial& bin=first_iter.get_element();

      reduce(bin,FALSE);
      /// The binomial was only reduced by one other generator before it was
      /// moved to aux_list. To reduce it by all other generators now can
      /// diminish the number of iterations (do-while-loop).

      if(bin==0)
        first_iter.delete_element();
      else
      {
        generators.insert(bin);
        /// We do not call the add_generator(...)-routine because we do not
        /// want number_of_new_binomials to be incremented.
        size++;
        first_iter.extract_element();
      }

    }

  }
  while(found==1);

  /// When leaving the loop, no generators have been interreduced during
  /// the last iteration; we are done.

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

/// In this case, the reduction has to be organized in a different way to
/// use the support information to its full extend. Without the support
/// methods, we test interreduction symmetrically: if we have a pair of
/// generators, we test if generator 1 can be reduced by generator 2 AND
/// if generator 2 can be reduced by generator 1. So each unordered pair is
/// considered only once.
/// If we would implement the same for the support methods, the second head
/// reduction could not use the support information.
/// For this reason, we test ordered pairs: For a given generator, we
/// try to reduce his head by all other generators, considering its head
/// support vector.

/// The method of moving changed binomials to aux_list and later reinserting
/// them is kept. When using SUPPORT_DRIVEN_METHODS_EXTENDED, it is even
/// necessary to choose such a method because the head support of some
/// binomials may change, too, a fact that requires the list structure to be
/// rebuilt.

  list_iterator first_iter;
  list_iterator second_iter;
  int found;
  /// to control if a reduction has occurred during the actual iteration

  do
  {
    found=0;
    /// no reduction occurred yet

    for(int i=0;i<Number_of_Lists;i++)
    {
      first_iter.set_to_list(generators[i]);

/// First try to reduce the binomials that are marked unreduced.

      while(first_iter.element_is_marked_head_reduced()==FALSE)
        /// all other elements have to be tested for interreduction
      {

        binomial& bin=first_iter.get_element();
        Integer changed=0;
        /// binomial referenced by bin not yet reduced

/// Look for head reductions:
/// Iterate over the lists that could contain reducers for the head of bin.
/// The list containing bin is tested separately to avoid an interreduction
/// of a binomial by itself respectively unnecessary checks for this when the
/// two iterators reference different lists.

        for(int j=0;(j<S.number_of_subsets[i]-1)&&(changed==0);j++)
        {
          second_iter.set_to_list(generators[S.subsets_of_support[i][j]]);
          /// This is the j-th list among the generator lists with elements
          /// whose support is a subset of that of i.
          /// The support of i is just the head support of bin (restricted
          /// to the corresponding variables).

          while((second_iter.is_at_end()==FALSE)&&(changed==0))
          {
            changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {
              if(bin!=0)
              {
                found=1;
                /// found has only to be set if the binomial has not been
                /// reduced to zero (else there are no new binomials)

                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();

              size--;
            }
            second_iter.next();
          }
        }

        /// The list generators[i]
        /// =generators[S.subsets_of_support[i][S.number_of_subsets[i]-1]]
        /// has to be tested separately to avoid a reduction of the actual
        /// binomial by itself.

        if(changed==0)
          /// binomial referenced by first_iter has not yet been reduced
        {
          second_iter.set_to_list(generators[i]);

          while((second_iter.is_at_end()==FALSE) && (changed==0))
          {
            if(second_iter!=first_iter)
              /// the two iterators do not reference the same element
              changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {

#ifdef SL_LIST

              /// bin will be deleted ar extracted - maybe dangerous for
              /// second_iter
              if(first_iter.next_is(second_iter))
                second_iter.next();

#endif  /// SL_LIST

              if(bin!=0)
              {
                found=1;
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();

              size--;
            }

            else
              /// bin has not been reduced
              second_iter.next();
          }
        }

      if(changed==0)
        first_iter.next();
      /// Else first_iter has already been set to the next element by deletion
      /// or extraction.
      }


///  Now try to reduce the binomials that are marked reduced.

      while(first_iter.is_at_end()==FALSE)
        /// only unreduced elements have to be tested for interreduction
      {
        binomial& bin=first_iter.get_element();
        Integer changed=0;
        /// binomial referenced by bin not yet reduced

/// Look for head reductions:
/// Iterate over the lists that could contain reducers for the head of bin.
/// The list containing bin is tested separately to avoid an interreduction
/// of a binomial by itself respectively unnecessary checks for this when the
/// two iterators reference different lists.

        for(int j=0;(j<S.number_of_subsets[i]-1)&&(changed==0);j++)
        {
          second_iter.set_to_list(generators[S.subsets_of_support[i][j]]);
          /// This is the j-th list among the generator lists with elements
          /// whose support is a subset of that of i.
          /// The support of i is just the head support of bin (restricted
          /// to the corresponding variables).

          while((second_iter.element_is_marked_head_reduced()==FALSE) &&
                (changed==0))
          {
            changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {
              if(bin!=0)
              {
                found=1;
                /// found has only to be set if the binomial has not been
                /// reduced to zero (else there are no new binomials)
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();

              size--;
            }
            second_iter.next();
          }
        }

        /// The list generators[i]
        /// =generators[S.subsets_of_support[i][S.number_of_subsets[i]-1]]
        /// has to be tested separately to avoid a reduction of the actual
        /// binomial by itself.

        if(changed==0)
          /// binomial referenced by first_iter has not yet been reduced
        {
          second_iter.set_to_list(generators[i]);

          while((second_iter.element_is_marked_head_reduced()==FALSE) &&
                (changed==0))
          {
            if(second_iter!=first_iter)
              /// the two iterators do not reference the same element
              changed=bin.reduce_head_by(second_iter.get_element(),w);

            if(changed!=0)
              /// bin has been reduced
            {

#ifdef SL_LIST

              /// bin will be deleted ar extracted - maybe dangerous for
              /// second_iter
              if(first_iter.next_is(second_iter))
                second_iter.next();

#endif  /// SL_LIST

              if(bin!=0)
              {
                found=1;
                aux_list._insert(bin);
                first_iter.extract_element();
              }
              else
                first_iter.delete_element();

              size--;
            }

            else
              /// bin has not been reduced
              second_iter.next();
          }
        }

      if(changed==0)
        first_iter.next();
      /// Else first_iter has already been set to the next element by deletion
      /// or extraction.
      }

    }


    /// Now we have found all currently possible reductions.
    /// The elements remaining in the generator lists cannot be interreduced
    /// and are marked reduced.

    for(int i=0;i<Number_of_Lists;i++)
    {
      first_iter.set_to_list(generators[i]);
      if(first_iter.is_at_end()==FALSE)
        while(first_iter.element_is_marked_head_reduced()==FALSE)
        {
          first_iter.mark_element_head_reduced();
          first_iter.next();
        }
    }


    /// Now reinsert reduced elements
    /// It seems to be quite unimportant for the performance if an element is
    /// completely reduced before reinsertion or not.

    first_iter.set_to_list(aux_list);
    while(first_iter.is_at_end()==FALSE)
    {
      binomial& bin=first_iter.get_element();
      reduce(bin,FALSE);

      if(bin==0)
        first_iter.delete_element();
      else
      {
        generators[bin.head_support%Number_of_Lists].insert(bin);
        size++;
        /// We do not call the add_generator(...)-routine because we do not
        /// want number_of_new_binomials to be incremented.
        first_iter.extract_element();
      }
    }

  }
  while(found==1);

  /// When leaving the loop, no generators have been interreduced during
  /// the last iteration; we are done.

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


  return(*this);
}





ideal& ideal::final_reduce()
{
/// During Buchberger큦 algorithm, we perform only head reductions
/// (minimalizations). This strategy showed to be a little more efficient
/// than the strategy to do reduce the ideal always completely.
/// This method leads to a minimal, but in general not to the reduced Groebner
/// basis. The actual procedure reduces such a minimal basis at the end of
/// Buchberger큦 algorithm. It will probably cause problems when called
/// in the course of the algorithm. For an explaination of this fact, see
/// the following comment.

  minimalize();

/// Now there remain only tail reductions. They are quite simple: Each
/// binomial's tail is reduced as long as possible. As no binomial can be
/// reduced to zero by that and a binomial cannot reduce its own tail,
/// we do not have to pay special attention to that (under the assumption
/// that a real term ordering (i.e. a well-ordering) is used and that
/// the head and the tail of the binomial are coorect with respect to this
/// ordering).

/// Notice that the head can change because of a tail reduction due to the
/// trivial factors elimination (the new head will always divide the
/// old one). This change is especially dangerous if
/// SUPPORT_DRIVEN_METHODS_EXTENDED are enabled: It may happen that the
/// binomial is in the wrong list after a tail reduction.
/// Furthermore, if a head change occurs, it may happen that the generating
/// set is no more minimalized after this. So the reduction has to be restarted
/// after such a head change (and the respective binomial has to be marked
/// head_unreduced before).
/// This does not seem to be very efficient.

/// For this reason, the reduction routine is only written for a final
/// reduction (having already computed a Groebner basis of the ideal).
/// This Groebner basis is first minimalized. After that, a head change during
/// tail reduction is impossible because the head is already a minimal
/// generator of the initial ideal (and the new head would divide the old).

/// However, this argumentation is only valid if the input ideal is saturated.
/// In some algorithms (Hosten_Sturmfels...) this is not the case in
/// intermediate steps. The final reduction may cause inconsistencies here.
/// But as the list structure is rebuild after each intermediate Groebner
/// basis calculation (change of the term ordering) and as the last Groebner
/// basis calculation deals with a saturated ideal, the final result will be
/// correct.
/// (For non-saturated input ideals, the computed Groebner basis is in general
/// not a Groebner basis of the input ideal, but one for an ideal "between"
/// the input ideal and its saturation.)


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list_iterator first_iter;

  first_iter.set_to_list(generators);

  while(first_iter.is_at_end()==FALSE)
  {
    binomial& bin=first_iter.get_element();
    int changed;
    /// to control if bin has been reduced

    do
    {
      changed=0;
      list_iterator second_iter;
      second_iter.set_to_list(generators);

      while(second_iter.is_at_end()==FALSE)
      {
        changed+=bin.reduce_tail_by(second_iter.get_element(),w);
        /// As soon as a reduction occurs, changed is set to a value !=0.
        second_iter.next();
      }

    }
    while(changed>0);

    first_iter.next();
  }

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

/// In this case, the reduction has to be organized in a slightly different
/// way to use the support information to its full extend.
/// As soon as an reduction has taken place, the iteration over the reducer
/// lists is restarted using the new tail support information.

  list_iterator first_iter;

  for(int i=0;i<Number_of_Lists;i++)
  {
    first_iter.set_to_list(generators[i]);

    while(first_iter.is_at_end()==FALSE)
    {
      binomial& bin=first_iter.get_element();
      int changed;
      /// to control if bin has been reduced

      do
      {
        changed=0;
        list_iterator second_iter;

        int supp=bin.tail_support%Number_of_Lists;
        /// determine the lists over which we have to iterate

        for(int j=0;(j<S.number_of_subsets[supp]) && (changed==0);j++)
        {
          second_iter.set_to_list(generators[S.subsets_of_support[supp][j]]);
          /// This is the j-th list among the generator lists with elements
          /// whose support is a subset of supp.

          while((second_iter.is_at_end()==FALSE) && (changed==0))
          {
            changed=bin.reduce_tail_by(second_iter.get_element(),w);
            /// Here we can do a simple assignment; as the iteration is stopped
            /// as soon as some reduction is done, reduced cannot be reset to
            /// zero in this assignment.
            second_iter.next();
          }
        }

      }
      while(changed>0);

      first_iter.next();
    }
  }

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED

  return(*this);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// auxiliary stuff //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





int ideal::add_new_generators()
{
/// Reduces the binomials in the "new_generators" list(s) by the generators
/// and moves them to the "generators" list(s).


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  int result=0;
  /// element inserted?

  list_iterator iter(new_generators);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    reduce(bin,FALSE);

    if(bin==0)
      iter.delete_element();
    else
    {
      add_generator(bin);
      iter.extract_element();
      result=1;
    }
  }


#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  int result=0;
  /// element inserted?

  list_iterator iter;

  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(new_generators[i]);

    while(iter.is_at_end()==FALSE)
    {
      binomial& bin=iter.get_element();
      reduce(bin,FALSE);

      if(bin==0)
        iter.delete_element();
      else
      {
        add_generator(bin);
        iter.extract_element();
        result=1;
      }
    }
  }


#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED


  return result;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// binomial reduction //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////





binomial& ideal::reduce(binomial& bin, BOOLEAN complete) const
{
/// As bin is reduced by a fixed set of binomials, it is sufficient to do
/// head reductions first, then tail reductions (cf. Pottier).

  list_iterator iter;
  Integer reduced;
  /// to control if the binomial has been reduced during the actual iteration


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  do
  {
    iter.set_to_list(generators);
    reduced=0;
    /// not yet reduced

    while(iter.is_at_end()==FALSE)
    {
      reduced+=bin.reduce_head_by(iter.get_element(),w);
      /// reduced is incremented (and so set to a value >0) as soon as bin
      /// is really reduced
      iter.next();
    }
  }
  while((reduced>0) && (bin!=0));

  if(complete==TRUE)
    do
    {
      iter.set_to_list(generators);
      reduced=0;
      /// not yet reduced

      while(iter.is_at_end()==FALSE)
      {
        reduced+=bin.reduce_tail_by(iter.get_element(),w);
        /// reduced is incremented (and so set to a value >0) as soon as bin
        /// is really reduced
        iter.next();
      }
    }
    while((reduced>0) && (bin!=0));

#endif  /// NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  do
  {
    reduced=0;
    /// not yet reduced

    int supp=bin.head_support%Number_of_Lists;
    /// determine the lists over which we have to iterate

    /// As soon as some reduction is done, the iteration is started with
    /// the new support information; so we do not finish iterations over lists
    /// that cannot contain reducers any more.

    for(int i=0;(i<S.number_of_subsets[supp]) && (reduced==0);i++)
    {
      iter.set_to_list(generators[S.subsets_of_support[supp][i]]);
      /// This is the i-th list among the generator lists with elements
      /// whose support is a subset of that of supp.

      while((iter.is_at_end()==FALSE)&&(reduced==0))
      {
        reduced=bin.reduce_head_by(iter.get_element(),w);
        /// Here we can do a simple assignment; as the iteration is stopped
        /// as soon as some reduction is done, reduced cannot be reset to zero
        /// in this assignment.
        iter.next();
      }
    }
  }
  while((reduced>0) && (bin!=0));

  if(complete==TRUE)
    do
    {
      reduced=0;
      /// not yet reduced

      int supp=bin.tail_support%Number_of_Lists;
      /// determine the lists over which we have to iterate

      /// As soon as some reduction is done, the iteration is started with
      /// the new support information; so we do not finish iterations over
      /// lists that cannot contain reducers any more.

      for(int i=0;(i<S.number_of_subsets[supp]) && (reduced==0);i++)
      {
        iter.set_to_list(generators[S.subsets_of_support[supp][i]]);
        /// This is the i-th list among the generator lists with elements
        /// whose support is a subset of that of supp.

        while((iter.is_at_end()==FALSE)&&(reduced==0))
        {
          reduced=bin.reduce_tail_by(iter.get_element(),w);
          /// Here we can do a simple assignment; as the iteration is stopped
          /// as soon as some reduction is done, reduced cannot be reset to
          /// zero in this assignment
        iter.next();
      }
    }
  }
  while(reduced>0);
  /// bin cannot be reduced to zero by a tail reduction

#endif  /// SUPPORT_DRIVEN_METHODS_EXTENDED

  return(bin);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// variants of Buchberger큦 algorithm //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





ideal& ideal::reduced_Groebner_basis_1(const int& S_pair_criteria,
                                       const float& interred_percentage)
{
  /// set flags for the use of the S-pair criteria
  /// for an explaination see in globals.h
  rel_primeness=(S_pair_criteria & 1);
  M_criterion=(S_pair_criteria & 2);
  F_criterion=(S_pair_criteria & 4);
  B_criterion=(S_pair_criteria & 8);
  second_criterion=(S_pair_criteria & 16);

  interreduction_percentage=interred_percentage;

  int done;
  /// control variable for recognizing when Buchberger's algorithm has reached
  /// his end

  /// first minimalize the initial generating system
  minimalize();

  do
  {
    done=1;
    /// no new generators found yet

    compute_actual_S_pairs_1();
    /// compute the S-pairs of the actual generators
    /// These are stored in a unreduced form in aux_list.

    list_iterator S_pair_iter(aux_list);
    /// now reduce and insert the computed S-pairs

    while(S_pair_iter.is_at_end()==FALSE)
    {
      binomial& S_bin=S_pair_iter.get_element();
      reduce(S_bin,FALSE);

      if(S_bin!=0)
        /// new generator found
      {
        add_generator(S_bin);
        S_pair_iter.extract_element();
        done=0;
        /// algorithm does not terminate when a new generator is found
      }
      else
        S_pair_iter.delete_element();
    }

    /// now all computed S-pairs are inserted as generators
    /// enough for a minimalization?

    if(interreduction_percentage>=0)
      /// intermediate interreductions allowed
    {
      if(number_of_new_binomials>=size*interreduction_percentage/100)
        /// enough new generators since last minimalization
      {
        minimalize();
        number_of_new_binomials=0;
      }
    }

    /// now we restart the algorithm with the new generating system
    /// if the generating system has changed during the last iteration

  }
  while(done==0);
  /// if done==1, all computed S-pairs reduced to zero

  /// compute reduced from minimal Groebner basis
  final_reduce();

  return(*this);
}






ideal& ideal::reduced_Groebner_basis_1a(const int& S_pair_criteria,
                                        const float& interred_percentage)
{
  /// set flags for the use of the S-pair criteria
  /// for an explaination see in globals.h
  rel_primeness=(S_pair_criteria & 1);
  M_criterion=(S_pair_criteria & 2);
  F_criterion=(S_pair_criteria & 4);
  B_criterion=(S_pair_criteria & 8);
  second_criterion=(S_pair_criteria & 16);

  interreduction_percentage=interred_percentage;

  int done;
  /// control variable for recognizing when Buchberger's algorithm has reached
  /// his end

  /// first minimalize the initial generating system
  minimalize();

  do
  {
    done=1;
    /// no new generators found yet

    compute_actual_S_pairs_1a();
    /// compute the S-pairs of the actual generators
    /// These are stored in a unreduced form in aux_list.
    /// aux_list is ordered according to the ideal큦 term ordering.

    list_iterator S_pair_iter(aux_list);
    /// now reduce and insert the computed S-pairs

    while(S_pair_iter.is_at_end()==FALSE)
    {
      binomial& S_bin=S_pair_iter.get_element();
      reduce(S_bin,FALSE);

      if(S_pair_iter.get_element()!=0)
        /// new generator found
      {
        add_generator(S_bin);
        S_pair_iter.extract_element();
        done=0;
        /// algorithm does not terminate when a new generator is found
      }
      else
        S_pair_iter.delete_element();
    }

    /// now all computed S-pairs are inserted as generators
    /// enough for a minimalization?

    if(interreduction_percentage>=0)
      /// intermediate interreductions allowed
    {
      if(number_of_new_binomials>=size*interreduction_percentage/100)
        /// enough new generators since last minimalization
      {
        minimalize();
        number_of_new_binomials=0;
      }
    }

    /// now we restart the algorithm with the new generating system
    /// if the generating system has changed during the last iteration

  }
  while(done==0);
  /// if done==1, all computed S-pairs reduced to zero

  /// compute reduced from minimal Groebner basis
  final_reduce();

  return(*this);
}





ideal& ideal::reduced_Groebner_basis_2(const int& S_pair_criteria,
                                       const float& interred_percentage)
{
  /// set flags for the use of the S-pair criteria
  /// for an explaination see in globals.h
  rel_primeness=(S_pair_criteria & 1);
  M_criterion=(S_pair_criteria & 2);
  F_criterion=(S_pair_criteria & 4);
  B_criterion=(S_pair_criteria & 8);
  second_criterion=(S_pair_criteria & 16);

  interreduction_percentage=interred_percentage;

  int done;
  /// control variable for recognizing when Buchberger's algorithm has reached
  /// his end

  /// first minimalize the initial generating system
  minimalize();

  do
  {
    done=1;
    /// no new generators found yet

    compute_actual_S_pairs_2();
    /// compute the S-pairs of the actual generators
    /// These are stored in a reduced version in aux_list.

    minimalize_S_pairs();
    /// minimalize the binomials in aux_list
    /// These are not only S-pairs, but also ideal generators that were
    /// reduced by an S-pair during the S-pair computation.

    list_iterator S_pair_iter(aux_list);

    if(S_pair_iter.is_at_end()==FALSE)
      /// Zero binomials are not inserted in aux_list during the S-pair
      /// computation; i.e. if aux_list is not empty, a further iteration step
      /// has to be done.
      done=0;

    /// now insert the computed S-pairs
    while(S_pair_iter.is_at_end()==FALSE)
      /// S_pairs remaining
    {
      add_generator(S_pair_iter.get_element());
      S_pair_iter.extract_element();
    }

    /// now all computed S-pairs are inserted as generators
    /// enough for a minimalization?

    if(interreduction_percentage>=0)
      /// intermediate interreductions allowed
    {
      if(number_of_new_binomials>=size*interreduction_percentage/100)
        /// enough new generators since last minimalization
      {
        minimalize();
        number_of_new_binomials=0;
      }
    }

    /// now we restart the algorithm with the new generating system
    /// if the generating system has changed during the last iteration

  }
  while(done==0);
  /// if done==1, all computed S-pairs reduced to zero

  /// compute reduced from minimal Groebner basis
  final_reduce();

  return(*this);
}





ideal& ideal::reduced_Groebner_basis_3(const int& S_pair_criteria,
                                       const float& interred_percentage)
{
  /// set flags for the use of the S-pair criteria
  /// for an explaination see in globals.h
  rel_primeness=(S_pair_criteria & 1);
  M_criterion=(S_pair_criteria & 2);
  F_criterion=(S_pair_criteria & 4);
  B_criterion=(S_pair_criteria & 8);
  second_criterion=(S_pair_criteria & 16);

  interreduction_percentage=interred_percentage;

  int not_done;
  /// control variable for recognizing when Buchberger's algorithm has reached
  /// his end

  /// first minimalize the initial generating system
  minimalize();

  do
  {

    compute_actual_S_pairs_3();
    /// compute the S-pairs of the actual generators
    /// These are stored in areduced version in the list(s) new_generators.

    minimalize_new_generators();
    /// minimalize the binomials in aux_list
    /// These are not only S-pairs, but also ideal generators that were
    /// reduced by an S-pair during the S-pair computation.

    not_done=add_new_generators();
    /// move binomials from new_generators to generators

    /// now all computed S-pairs are inserted as generators
    /// enough for a minimalization?

    if(interreduction_percentage>=0)
      /// intermediate interreductions allowed
    {
      if(number_of_new_binomials>=size*interreduction_percentage/100)
        /// enough new generators since last reduction
      {
        minimalize();
        number_of_new_binomials=0;
      }
    }

    /// now we restart the algorithm with the new generating system
    /// if the generating system has changed during the last iteration

  }
  while(not_done==1);
  /// if not_done==0, all computed S-pairs reduced to zero

  /// compute reduced from minimal Groebner basis
  final_reduce();

  return(*this);
}





ideal& ideal::reduced_Groebner_basis(const int& version,
                                     const int& S_pair_criteria,
                                     const float& interred_percentage)
{
  switch(version)
  {
      case 0:
        return reduced_Groebner_basis_1a(S_pair_criteria, interred_percentage);
      case 1:
        return reduced_Groebner_basis_1(S_pair_criteria, interred_percentage);
      case 2:
        return reduced_Groebner_basis_2(S_pair_criteria, interred_percentage);
      case 3:
        return reduced_Groebner_basis_3(S_pair_criteria, interred_percentage);
      default:
        cerr<<"WARNING: ideal& ideal::reduced_Groebner_basis(const int&, "
          "const int&, const float&):\n"
          "version argument out of range, nothing done"<<endl;
        return*this;
  }
}





#endif  /// BUCHBERGER_CC
