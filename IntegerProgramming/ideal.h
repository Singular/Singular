// ideal.h

// This class implements a saturated binomial ideal. Such an ideal is
// given by its generators.
// If SUPPORT_DRIVEN_METHODS_EXTENDED are enabled, the generators are not
// stored in a single list, but classified according to their support. This
// representation, together with the subset_tree data structure, allows a
// faster search for reducers.

// The entries of the two-dimensional array "subsets_of_support" are to be
// read as bit vectors with List_Support_Variables components (the constant
// List_Support_Variables is defined in globals.h). So all entries
// are in the range between 0 and 2^List_Support_Variables -1.
// subsets_of_support[i] is an array that contains all binary vectors whose
// support is a subset of that of i, i read as a binary vector.
// number_of_subsets[i] is the number of these subsets, i.e. the length of
// the array subsets_of_support[i].

// If, for example, List_Support_Variables=8, the datastructure subset_tree
// has 6561=8^3 entries.



#ifndef IDEAL_H
#define IDEAL_H



#include "list.h"
#include "matrix.h"




const int Number_of_Lists=1<<List_Support_Variables;




////////////////////// struct subset_tree ////////////////////////////////////

typedef struct
{
  int* subsets_of_support[Number_of_Lists];
  int number_of_subsets[Number_of_Lists];
} subset_tree;

//////////////////////// class ideal /////////////////////////////////////////

class ideal
{

private:

// generator lists

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  subset_tree S;

  list generators[Number_of_Lists];

  list new_generators[Number_of_Lists];
  // Only needed in some special versions of Buchberger´s algorithm to
  // store newly found generators.

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  list generators;

  list new_generators;
  // Only needed in some special versions of Buchberger´s algorithm to
  // store newly found generators.

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

// flags for the use of the S-pair criteria and the autoreduction

  int rel_primeness;
  int M_criterion;
  int F_criterion;
  int B_criterion;
  int second_criterion;
  // When Buchberger´s algorithm is called, we only use one argument which
  // describes the combination of the criteria to be used (see in globals.h).
  // But we use five flags instead of one here because this is a little more
  // efficient.
  // The standard setting enables the relative primeness criterion, the M- and
  // the B-criterion.

  float interreduction_percentage;
  // To  determine the autoreduction frequency, i.e. the percentage of new
  // generators (i.e. generators found since the last autoreduction) with
  // respect to the total size of an ideal that must be reached to cause an
  // interreduction.
  // If Interreduction_Percentage==0, interreduction is done after each
  // S-Pair computation. If Interreduction_Percentage<0, interreduction is
  // only done once at the end of the algorithm.
  // The standard setting is 12 (=12%).

// further members

  term_ordering w;
  // For technical reasons, the term ordering is taken as a member.
  // So we do not need to pass it as an argument to each ideal function,
  // and the management of the generator lists is easier and safer when
  // using SUPPORT_DRIVEN_METHODS_EXTENDED.

  list aux_list;
  // an auxiliary list for keeping (for example) S-pairs

  long size;
  // the actual number of generators of the ideal
  // Also used as an "error flag"; a negative size means that an error has
  // occurred:
  // -1 indicates a "semantic" error (which occurs e.g. if some constructor
  //    argument is out of range)
  // -2 indicates an error occurred when reading from a file
  // -3 indicates an overflow of an integer type variable


  long number_of_new_binomials;
  // the number of newly found generators

// private member functions

// subroutines for building and deleting the subset_tree data structure
// (implemented in ideal.cc)

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  void create_subset_tree();
  void destroy_subset_tree();

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

// subroutines for Buchberger´s algorithm (implemented in Buchberger.cc except
// from the first two implemeted in ideal.cc)

// Some of these procedures do not interact with all versions of Buchberger´s
// algorithm. Generally, they cannot be combined as one likes. This header
// file only gives a brief overview of their features. For more detailed
// comments see the implementation.

  ideal& add_new_generator(binomial& bin);
  ideal& add_generator(binomial& bin);
  // Inserts a (new) generator in the appropriate list.

  int add_new_generators();
  // Moves the new generators to the generator lists.

// S-pair computation

  BOOLEAN unnecessary_S_pair(list_iterator&, list_iterator&) const;
  // Checks if the S_Pair of the binomials referenced by the iterators can be
  // discarded (by the criteria chosen in globals.h).

  ideal& compute_actual_S_pairs_1();
  ideal& compute_actual_S_pairs_1a();
  ideal& compute_actual_S_pairs_2();
  ideal& compute_actual_S_pairs_3();
  // different versions for computing the S-binomials of the actual generators
  // They differ for example in the following points:
  // - They insert the S-binomials in the order in which they are computed, in
  //   the order given by the term ordering w or according to their support.
  // - Some reduce an S-binomial directly after its computation by the ideal
  //   generators, others reduce it later (after having computed more
  //   S-binomials, hence more possible reducers).
  // - Some reduce the ideal generators immediately by a newly computed
  //   S-binomial, others don´t.
  // In each case, the use of the list flag "done" guarantees that we perform
  // the S-pair computation only once for each binomial pair.

// minimalization / autoreduction

  ideal& reduce_by(const binomial&, list_iterator&, list_iterator&);
  // Reduces the heads of the ideal generators by the given binomial
  // (used by some versions of the S-pair computation).

  ideal& minimalize_S_pairs();
  ideal& minimalize_new_generators();
  ideal& minimalize();
  // Performs an autoreduction of the binomials stored in aux_list
  // respectively in the list(s) new_generators respectively in the
  // generators.
  // The respective list(s) is (are) reduced to a minimal set (not
  // necessarily to a reduced set); this means that only the heads of the
  // binomials are reduced, not their tails. This strategy showed to be
  // a little more efficient than a complete autoreduction.
  // The last two procedures use the "head_reduced" flag of the list class
  // to avoid unnecessary tests for interreduction.

  ideal& final_reduce();
  // final interreduction of the ideal generators
  // It seems to be slightly more efficient to perform a complete
  // interreduction only at the end of Buchberger's algorithm and
  // to replace the intermediate interreductions by minimalizations.
  // For efficiency reasons, this routine is designed for reducing a
  // Groebner basis of an saturated ideal. Reducing another generating
  // set with it may cause inconsistencies (cf. comment in the implementation).

// constructor subroutines for the handling of the different algorithms
// (implemented in ideal.cc)
// The "Conti-Traverso constructors" create a toric ideal directly from the
// input matrix and the term ordering given by the input weight vector.
// The other constructors create a toric ideal from a lattice basis of
// the kernel of the input matrix.

  ideal& Conti_Traverso_ideal(matrix&, const term_ordering&);
  ideal& Positive_Conti_Traverso_ideal(matrix&, const term_ordering&);
  ideal& Pottier_ideal(matrix&, const term_ordering&);
  ideal& Hosten_Sturmfels_ideal(matrix&, const term_ordering&);
  ideal& DiBiase_Urbanke_ideal(matrix&, const term_ordering&);
  ideal& Bigatti_LaScala_Robbiano_ideal(matrix&, const term_ordering&);

public:

// constructors and destructor (implemented in ideal.cc)

  ideal(matrix&, const term_ordering&, const int& algorithm);
  // Creates a binomial ideal from the given matrix using the given algorithm
  // (see in globals.h).
  // The arguments are checked for consistency as far as possible.
  // The term ordering is needed to determine the leading terms of the
  // binomials in the resulting generating set. Neither is the generated ideal
  // saturated nor is it given in form of a Groebner basis.
  // Such computations must be explicitely demanded.
  // The argument matrix cannot be declared as const because the constructor
  // may call the LLL-algorithm to compute the matrix kernel (if this hasn´t
  // been done yet). This algorithm changes some matrix members. But the
  // real matrix remains, of course, unchanged.

  ideal(const ideal&);
  // copy-constructor
  // It might be useful to keep several Groebner bases of the same ideal
  // (or of an ideal and its elimination ideal).

  ideal(ifstream&, const term_ordering&, const int& number_of_generators);
  // Reads an ideal from a given ifstream in the following way:
  // A block of integers is converted into a binomial
  // that is stored in the generator list(s) with respect to the given
  // term ordering. The size of such a block is the size of the given
  // term_ordering, i.e. the number of variables for which it was constructed.
  // This is done number_of_generators times.

   ~ideal();
  // destructor

// object information (implemented in ideal.cc)

  long number_of_generators() const;
  // Returns the actual number of generators.

  int error_status() const;
  // Returns -1 if an error has occurred (i.e. size<0), else 0.

// Buchberger stuff (implemented in Buchberger.cc)

  ideal& reduced_Groebner_basis_1(const int& S_pair_criteria=11,
                                  const float& interred_percentage=12.0);
  ideal& reduced_Groebner_basis_1a(const int& S_pair_criteria=11,
                                   const float& interred_percentage=12.0);
  ideal& reduced_Groebner_basis_2(const int& S_pair_criteria=11,
                                  const float& interred_percentage=12.0);
  ideal& reduced_Groebner_basis_3(const int& S_pair_criteria=11,
                                  const float& interred_percentage=12.0);
  // Several different versions of Buchberger´s algorithm for computing
  // the reduced Groebner basis of the actual ideal. They differ in the
  // steering of the algorithm (i.e. in the way in which the S-pair
  // computation and reduction is organized). Further variants can be
  // obtained by setting the flags in globals.h (autoreduction frequency,
  // use of the S-pair criteria and the support information).
  // Except from the variant 1a (which orders the S-pairs with respect to w
  // and which showed to be quite slow), the efficiency of these algorithms
  // does not differ too much.
  // The first argument indicates which criteria will be used to discard
  // unnecessary S-pairs. For an explaination of how this works, see in
  // globals.h. The default value 11 means that we use the criterion of
  // relatively prime leading terms as well as the M- and the B-criterion.
  // The second argument specifies the interreduction frequency; see the
  // comment for the member interreduction_percentage above. The standard
  // value is 12%.
  // ATTENTION: In spite of the different argument type you should never
  // try to use these functions with one default argument (either two or
  // none). When writing
  //    reduced_Groebner_basis_1(10.5),
  // the GNU C++ compiler casts 10.5 into an integer and takes it as the
  // argument S_pair_criteria!
  // ATTENTION: If the input ideal is not saturated, the computed reduced
  // Groebner basis is not that of the input ideal, but that of an ideal
  // "between" the input ideal and its saturation.

  ideal& reduced_Groebner_basis(const int& version=1,
                                const int& S_pair_criteria=11,
                                const float& interred_percentage=12.0);
  // Takes the version of Buchberger´s algorithm as above as an argument
  // (to allow a call of different versions of our IP-algorithms from the
  // command line). To call version 1a, the first argument has to be zero.

  binomial& reduce(binomial& bin, BOOLEAN complete=TRUE) const;
  // Reduces a binomial by the actual generators.
  // To reduce a binomial by the ideal (outside of Buchberger´s algorithm),
  // be sure to have computed the reduced Groebner basis before.
  // The flag "complete" indicates if the binomial is reduced completely
  // (head and tail); if it is set to FALSE, only the tail will be reduced.
  // Using this flag in a clever manner allows to improve the performance
  // of Buchberger's algorithm by up to three percent.

// special features needed by our IP-algorithms
// (implemented in ideal_stuff.cc)

  ideal& eliminate();
  // Eliminates the generators involving elimination variables
  // (with respect to the term ordering w).
  // The reduced Groebner basis with respect to w should have been computed
  // before calling this routine.

  ideal& pseudo_eliminate();
  // Eliminates the generators involving the last weighted variable -
  // a special routine needed by the algorithm of Bigatti, La Scala and
  // Robbiano.

  ideal& change_term_ordering_to(const term_ordering& _w);
  // Replaces the actual term ordering by the argument ordering.
  // Their "compatibility" (number of variables) is checked, head and tail
  // of the generators are adapted to the new ordering.
  // This may especially involve a rebuilding of the list structure if
  // SUPPORT_DRIVEN_METHODS_EXTENDED are enabled.

  ideal& swap_variables(const int& i, const int& j);
  // Swaps the i-th and the j-th variable in all generators as well as the
  // corresponding components of the term ordering's weight vector.
  // If SUPPORT_DRIVEN_METHODS are enabled, the list structure is rebuilt
  // according to the new order on the variables.

  ideal& swap_variables_unsafe(const int& i, const int& j);
  // Swaps the i-th and the j-th variable in all generators as well as the
  // corresponding components of the term ordering's weight vector.
  // DANGER: The head/tail structure is not rebuilt!

  ideal& flip_variable_unsafe(const int& i);
  // Inverts the sign of the i-th variable in all generators.
  // DANGER: The list structure is not rebuilt!

// output (implemented in ideal.cc)

  void print() const;
  void print_all() const;
  // Writes the ideal to the standard output medium.
  // The second routine also prints the S-pair flags.

  void print(FILE*) const;
  void print_all(FILE*) const;
  // Writes the ideal to the referenced file which has to be opened
  // for writing before.

  void print(ofstream&) const;
  void print_all(ofstream &) const;
  // Writes the ideal to the given ofstream.

  void format_print(ofstream&) const;
  // Writes the ideal generators to the given ofstream in a format
  // that allows them to be reread by the ideal constructor from an ifstream.

};

#endif  // IDEAL_H
