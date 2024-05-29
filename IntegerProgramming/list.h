// list.h


// The classes "list" and "list_iterator" are designed for the special needs
// of Buchberger's algorithm; it is not recommendable to use them for
// more general purposes.
// "list" implements a simply linked list of binomials (or, more exactly,
// pointers on binomials).
// A list_iterator is not much more than a pointer to a list element, but using
// iterators instead of pointers makes the code of Buchberger's algorithm much
// easier to read.

// The delegation of tasks to this classes and the realization of some
// functions are quite special.
// So is the insertion of list elements the task of the class list, while the
// deletion is delegated to the iterator class. The reason is the simple:
// Elements are almost always inserted at the beginning of a list (except
// from the case of an ordered list), but can be deleted from an arbitrary
// place (this is necessary e.g. if a binomial is reduced to zero during
// Buchberger's algorithm).

// For efficiency reasons, the iterator operations are not secure. They do
// not check if the iterator really references a list element or if it has
// reached the end of the list. To assert this, use the is_at_end()-test.

// I have designed an own iterator class instead of taking pointers for
// iteration as lists members for flexibility reasons:
// Buchberger's algorithm requires to form pairs of binomials (or, for
// testing criteria to discard S-Pairs, even triples). So we need up to
// three pointers to iterate over the generator lists. The advantage of the
// iterator class is simply that the programmer does not have to keep
// in mind the lists over which he is actually iterating.
// This was very helpful for me during the implementation of Buchberger's
// algorithm (especially for the implementation of
// SUPPORT_DRIVEN_METHODS_EXTENDED where many lists have to be considered).


// There are two different implementations of the class list:
// - If SL_LIST is set, we use a simply linked list.
//   The insert operations are very efficient because we have to set very
//   few pointers. But the deletion of an element is a dangerous operation
//   because it moves the following element (not the binomial, but the struct)
//   to another storage place.
// - If DL_LIST is set, we use a doubly linked list.
//   This shows to be considerably slower, but it is easier to use



#ifndef LIST_H
#define LIST_H



#define DL_LIST
// possibilities:
// SL_LIST
// DL_LIST



#include "binomial__term_ordering.h"




////////////////////////////// struct element ///////////////////////////////




typedef struct Element
{
  binomial *entry;
  struct Element *next;


#ifdef DL_LIST

  struct Element *previous;

#endif  // DL_LIST


  // flags needed by the ideal implementation to speed up Buchberger's
  // algorithm
  BOOLEAN done;
  BOOLEAN head_reduced;

} element;




/////////////////////////// class list ////////////////////////////////////




class list
{



private:


  element *start;
  // pointer to the beginning



public:


// constructors and destructor

  list();
  // Creates an "empty" list (with a dummy element for a user friendly
  // iterator class, see the comments in list.cc).

  list(const list&);
  // copy-constructor

  ~list();
  // destructor



// inserting

  list& insert(binomial&);
  list& copy_insert(const binomial&);
  // These operations insert a binomial at the beginning of the list:
  // insert does this by placing pointers on it, copy_insert by copying the
  // binomial.
  // The first operation is dangerous; but the consequent use of the
  // combination insert - extract_element (cf. class list_iterator) instead of
  // copy_insert - delete_element is very important for the performance of
  // Buchberger's algorithm.

  list& _insert(binomial&);
  list& _copy_insert(const binomial&);
  // A little more efficient insert function for list that do not use
  // the 3 flags - these are not set.
  // It is not more efficient to implement these simpler lists as an own class.

  list& ordered_insert(binomial&, const term_ordering&);
  list& ordered_copy_insert(const binomial&, const term_ordering&);
  // These operations insert a binomial according to the given term ordering
  // into a list. (The list should be ordered by the same term ordering.)
  // To insert elements, we use a simple linear search.

  list& _ordered_insert(binomial&, const term_ordering&);
  list& _ordered_copy_insert(const binomial&, const term_ordering&);
  // A little more efficient ordered_insert function for list that do not use
  // the 3 flags - these are not set.



// output

  void print() const;
  void ordered_print(const term_ordering&) const;
  // Writes the list to the standard output medium.
  // The first routine writes the list elements as they are ordered in
  // the list.
  // The second one writes them in increasing order with respect to the
  // argument term ordering; the list has not to be ordered before and
  // will not be ordered after. This routine is essentially written to
  // compare Gröbner bases.

  void print(FILE* output) const;
  void ordered_print(FILE* output, const term_ordering&) const;
  // Writes the list to the referenced file which has to be opened for
  // writing before.

  void print(ofstream&) const;
  void ordered_print(ofstream&, const term_ordering&) const;
  // Writes the list to the given ofstream.

  void format_print(ofstream&) const;
  void ordered_format_print(ofstream&, const term_ordering&) const;
  // Writes the list to the given ofstream in the format needed by the
  // IP-algorithms.

  friend class list_iterator;

};




///////////////////// class list_iterator ////////////////////////////////




class list_iterator
{



private:

  element* actual;



public:



// constructors and destructor

  list_iterator();
  // Sets actual to NULL.

  list_iterator(list& l);
  // Sets a list_iterator to the beginning of l.

  list_iterator(list_iterator& iter);
  // Sets a list iterator to the same position as iter.

  ~list_iterator();
  // destructor



// object information

  BOOLEAN element_is_marked_done() const;
  // Returns the "done"-component of the referenced element.

  BOOLEAN element_is_marked_head_reduced() const;
  // Returns the "head_reduced"-component of the referenced element.

  BOOLEAN is_at_end() const;
  // Returns TRUE iff the iterator is at the end of "its" list
  // (especially if the iterator references no list).



// assignment

  list_iterator& set_to_list(const list& l);
  // Sets the iterator to the beginning of l.

  list_iterator& operator=(const list_iterator& iter);
  // Sets the iterator to the same element as iter.

  list_iterator& next();
  // Sets the iterator to the next entry.



// comparison

  int operator==(const list_iterator& iter) const;
  int operator!=(const list_iterator& iter) const;
  // These operators verify if actual references the same element
  // as iter.actual.

  int next_is(const list_iterator& iter) const;
  // Checks if actual->next references the same element as iter.actual.
  // This check is needed in very special situations (two iterators reference
  // subsequent elements of a list, and the first is deleted; then the
  // iterator referencing the second before deletion references freed memory
  // after deletion, cf. the implementation of delete and extract).



// manipulation of list elements

  // All this operations could be declared as const because they do not
  // change the "actual" pointer. For suggestive reasons, this is not done
  // because they change the referenced list element.

  binomial& get_element();
  // Returns the referenced binomial.

  list_iterator& delete_element();
  // Deletes the referenced binomial.

  list_iterator& extract_element();
  // Only resets pointers so that the referenced binomial
  // is no longer found when iterating over the list.

  list_iterator& mark_element_done();
  // Sets the "done"-component of the referenced element to TRUE.

  list_iterator& mark_element_undone();
  // Sets the "done"-component of the referenced element to FALSE.

  list_iterator& mark_element_head_reduced();
  // Sets the "head_reduced"-component of the referenced element to TRUE.

  list_iterator& mark_element_head_unreduced();
  // Sets the "head_reduced"-component of the referenced element to FALSE.

};


#endif  // list.h
