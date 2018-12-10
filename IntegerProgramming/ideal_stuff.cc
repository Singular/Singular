// ideal_stuff.cc

// implementation of the special ideal features needed by the IP-algorithms

#ifndef IDEAL_STUFF_CC
#define IDEAL_STUFF_CC

#include "ideal.h"

////////////////////// elimination stuff ///////////////////////////////////

ideal& ideal::eliminate()
{
// eliminates the generators of the ideal involving elimination variables
// with respect to w

  if(w.number_of_elimination_variables()<=0)
    // elimination unnecessary
    return *this;

  list_iterator iter;

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

// Simply iterate over the generator list and delete the elements involving
// elimination variables.
// There is no need to change the done/undone or the reduced/unreduced mark of
// an element.

  iter.set_to_list(generators);

  while((iter.is_at_end())==FALSE)
  {
    binomial& bin=iter.get_element();

    if(bin.involves_elimination_variables(w)==TRUE)
    {
      iter.delete_element();
      size--;
    }
    else
    {
      bin.drop_elimination_variables(w);
      iter.next();
    }
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

// Iterate over the generator lists and check whether the elements involve
// elimination variables.
// As the set of support variables can be changed by the elimination, the
// elements that are not deleted are first moved to the aux_list and then
// reinserted according to their new support.
// The elimination variables are droppd while reinserting.
// In general, elimination is done only once. The time needed for this is
// linear in the number of generators (in the Groebner basis). The elimination
// itself is therefore very fast in comparison to the Groebner basis
// computation needed for it... So we renounce to a complicated optimization
// of this procedure (the support information is not used). In fact, tests
// show that elimination time is really negligible.

  // elimination
  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(generators[i]);

    while((iter.is_at_end())==FALSE)
    {
      binomial& bin=iter.get_element();

      if(bin.involves_elimination_variables(w)==TRUE)
      {
        iter.delete_element();
        size--;
      }
      else
      {
        aux_list._insert(bin);
        iter.extract_element();
        // As the generators are reinserted later, we do not decrement the
        // size (and so do not need to increment it during reinsertion).
      }
    }
  }

  // reinsertion
  iter.set_to_list(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    bin.drop_elimination_variables(w);
    generators[bin.head_support%Number_of_Lists].insert(bin);
    // size is not incremented, see above...
    iter.extract_element();
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED


  // finally adapt term ordering
  w.convert_to_weighted_ordering();

  return *this;
}




ideal& ideal::pseudo_eliminate()
{

  if(w.number_of_weighted_variables()<=0)
  {
    cerr<<"WARNING: ideal& ideal::pseudo_eliminate():\n"
      "cannot be performed without weighted variables"<<endl;
    return *this;
  }


  list_iterator iter;

  int last_weighted_variable=w.number_of_weighted_variables()-1;


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

// Simply iterate over the generator list and delete the elements involving
// the last weighted variable.
// There is no need to change the done/undone or the reduced/unreduced mark of
// an element.

  iter.set_to_list(generators);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();

    if(bin[last_weighted_variable]!=0)
      // weighted variable to drop is involved in bin
    {
      iter.delete_element();
      size--;
    }
    else
    {
      bin.drop_last_weighted_variable(w);
      iter.next();
    }
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

// Iterate over the generator lists and check whether the elements involve
// the last weighted variable.
// As the set of support variables can be changed by the pseudo-elimination,
// the elements that are not deleted are first moved to the aux_list and then
// reinserted according to their new support.
// The last weight variable is dropped while reinserting.
// For the time needed by this function see the remarks for ideal::eliminate().

  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(generators[i]);

    while((iter.is_at_end())==FALSE)
    {
      binomial& bin=iter.get_element();

      if(bin[last_weighted_variable]!=0)
      {
        iter.delete_element();
        size--;
      }
      else
      {
        aux_list._insert(bin);
        iter.extract_element();
        // As the generators are reinserted later, we do not decrement the
        // size (and so do not need to increment it during reinsertion).
      }
    }
  }


  iter.set_to_list(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    bin.drop_last_weighted_variable(w);
    generators[bin.head_support%Number_of_Lists].insert(bin);
    // size is not incremented, see above...
    iter.extract_element();
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  // finally adapt term ordering
  w.delete_last_weighted_variable();

  return *this;
}




/////////////////// management of the term ordering /////////////////////////




ideal& ideal::change_term_ordering_to(const term_ordering& _w)
{

  // first check compatibility
  if((w.number_of_weighted_variables()+w.number_of_elimination_variables())!=
     (_w.number_of_weighted_variables()+_w.number_of_elimination_variables()))
  {
    cerr<<"WARNING: ideal& ideal::change_term_ordering_to"
      "(const term_ordering&):\nincompatibility detected, term ordering not"
      "changed."<<endl;
    return *this;
  }

  // change term ordering
  w=_w;

  list_iterator iter;


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

// Simply iterate over the generator list. Because the change of the term
// ordering, the "done" and "reduced" marks of the elements have to be deleted.

  iter.set_to_list(generators);

  while((iter.is_at_end())==FALSE)
  {
    (iter.get_element()).adapt_to_term_ordering(w);
    iter.mark_element_undone();
    iter.mark_element_head_unreduced();
    iter.next();
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

// As head and tail might have to be exchanged, the elements are first moved to
// the aux_list and then reinserted according to their new head.

  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(generators[i]);

    while((iter.is_at_end())==FALSE)
    {
      binomial& bin=iter.get_element();

      if(bin.adapt_to_term_ordering(w)==-1)
        // head and tail exchanged
      {
        aux_list._insert(bin);
        iter.extract_element();
        // As the generators are reinserted later, we do not decrement the
        // size (and so do not need to increment it during reinsertion).
      }
      else
      {
        // Although the S-pairs of the remaining elements have already been
        // computed once, the "done" marks have to be deleted: With a new
        // term ordering, the results of the S-pair reduction can change -
        // as well as the interreduction results.
        iter.mark_element_undone();
        iter.mark_element_head_unreduced();
        iter.next();
      }
    }
  }

  // reinsertion
  iter.set_to_list(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    generators[bin.head_support%Number_of_Lists].insert(bin);
    // size is not incremented, see above...
    iter.extract_element();
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED


  return *this;
}




/////////// manipulation of the variables ///////////////////////////////////




ideal& ideal::swap_variables_unsafe(const int& i, const int& j)
{
  // first check arguments
  if((i<0) || (i>=w.number_of_weighted_variables())
     || (j<0) || (j>=w.number_of_weighted_variables()))
  {
    cout<<"WARNING: ideal::swap_variables(const int&, const int&)\n "
      "or ideal::swap_variables_unsafe(const int&, const int&):\n"
      "index out of range"<<endl;
    return *this;
  }

  if(i==j)
    return(*this);
  // special case to avoid unnecessary overhead


  list_iterator iter;


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  iter.set_to_list(generators);

  while((iter.is_at_end())==FALSE)
  {
    (iter.get_element()).swap_variables(i,j);
    iter.next();
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

// As head_support and tail_support are manipulated, the elements are first
// moved to the aux_list and then reinserted according to their new head.
// But head and tail are not adapted to the new term ordering induced by
// the change of the variable order - this is only done in the "safe"
// routine swap_variables(const int&, const int&).

  for(int l=0;l<Number_of_Lists;l++)
  {
    iter.set_to_list(generators[l]);

    while((iter.is_at_end())==FALSE)
    {
      binomial& bin=iter.get_element();
      bin.swap_variables(i,j);
      aux_list._insert(bin);
      iter.extract_element();
      // As the generators are reinserted later, we do not decrement the
      // size (and so do not need to increment it during reinsertion).
    }
  }

  // reinsertion
  iter.set_to_list(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    generators[bin.head_support%Number_of_Lists].insert(bin);
    // size is not incremented, see above...
    iter.extract_element();
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED


  // finally adapt term ordering
  w.swap_weights(i,j);

  return *this;
}




ideal& ideal::swap_variables(const int& i, const int& j)
{

  swap_variables_unsafe(i,j);

  change_term_ordering_to(w);
  // This rebuilds the list structure...

  return *this;
}




ideal& ideal::flip_variable_unsafe(const int& i)
{
  // first check argument
  if((i<0) || (i>=w.number_of_weighted_variables()))
  {
    cout<<"WARNING: ideal::flip_variables(const int&):\n"
      "argument out of range, nothing done"<<endl;
    return *this;
  }


  list_iterator iter;


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  iter.set_to_list(generators);

  while((iter.is_at_end())==FALSE)
  {
    (iter.get_element()).flip_variable(i);
    iter.next();
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

// As head_support and tail_support can change, the elements are first moved
// to the aux_list and then reinserted according to their new head.

  for(int l=0;l<Number_of_Lists;l++)
  {
    iter.set_to_list(generators[l]);

    while((iter.is_at_end())==FALSE)
    {
      binomial& bin=iter.get_element();
      bin.flip_variable(i);
      aux_list._insert(bin);
      iter.extract_element();
      // As the generators are reinserted later, we do not decrement the
      // size (and so do not need to increment it during reinsertion).
    }
  }

  // reinsertion
  iter.set_to_list(aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial& bin=iter.get_element();
    generators[bin.head_support%Number_of_Lists].insert(bin);
    iter.extract_element();
    // size is not incremented, see above...
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  return *this;
}
#endif  // IDEAL_STUFF_CC
