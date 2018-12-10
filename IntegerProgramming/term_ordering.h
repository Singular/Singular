/// term_ordering.h


/// Our IP-algorithms need two kinds of term orderings: a weighted term ordering
/// and a block ordering that eliminates some auxiliary variables. Both
/// are constructed from the input or for the specific requirements of the
/// algorithms. To allow an easy and flexible handling, the term ordering is
/// realized as a class.

/// Weight vectors for term orderings have to be nonnegative in most contexts.
/// But input vectors with negative components are not refused by the
/// constructors. This has the following two reasons:
/// - There is a more general notion of term ordering who does not claim
///   the monomial 1 to be a (unique) minimal monomial. This allows negative
///   weights.
/// - This class is written for the final purpose of integer programming.
///   The linear objective function induces the term ordering and might also
///   have negative components. Although the program actually cannot treat
///   this case, there are ways to tackle it. In view of a possible later
///   expansion of the program, the nonnegativity check is delegated to a
///   separate function instead of doing it in the constructors.

/// For the same reasons as mentionned in the binomial class, frequently
/// used operations on Integer vectors do not perform range checks. Nor do
/// they check if an error has occurred (i.e. control the "error flag", see
/// below).
/// These are concretely the functions weight(...), compare_to_zero(...) and
/// compare(...).
/// All other routines perform argument checks (range, file format...).
/// This is only really important when reading from a file (as most other range
/// checks are already made by the routines calling these functions). But as
/// all these operations are executed only a few times in the course of our
/// algorithms, the checks do not slow down computation.

#ifndef TERM_ORDERING_H
#define TERM_ORDERING_H

#include "globals.h"

class binomial;

class term_ordering
{
protected:

  float *weight_vector;

  short weighted_block_size;
  /// the number of weighted variables
  /// Also the "error flag", i.e. values <0 indicate an error:
  /// -1 indicates a "semantic" error (arguments out of range, for example a
  ///    negative number of variables);
  /// -2 indicates an error occurred when reading from a file.

  short weighted_ordering;
  /// the term ordering refining the weight on the weighted variables
  /// possibilities: see in globals.h

  short elimination_block_size;
  /// the number of elimination variables

  short elimination_ordering;
  /// the term ordering on the elimination variables

  BOOLEAN homogeneous;
  /// This flag is set to TRUE if the term ordering is thought of as a
  /// homogeneous one (i.e. the weighted part), else to FALSE. The standard
  /// setting is FALSE.
  /// The routine compare_to_zero is faster if the term ordering is homogenous.
  /// It would be safer and more natural to take this flag as a member of
  /// the ideal class; this would, however, require many supplementary
  /// functions (homogeneous versions of existing functions) in the ideal and
  /// even in the binomial class.

public:
/// constructors and destructor

  term_ordering(const BOOLEAN& homogeneous=FALSE);
  /// Sets weighted_block_size and elimination_block_size to zero.
  /// With this default contructor, the term ordering can be taken as a member
  /// of another class (the ideal class in our case).

  term_ordering(const short& number_of_weighted_variables,
                const float* weights,
                const short& _weighted_ordering,
                const BOOLEAN& _homogeneous=FALSE);
  /// Initializes weight vector with weights,
  /// weighted_block_size with number_of_weighted_variables,
  /// the refining ordering as given in the argument
  /// and elimination_block_size with 0.

  term_ordering(const short& number_of_weighted_variables,
                const float* weights,
                const short& _weighted_ordering,
                const short& number_of_elimination_variables,
                const short& _elimination_ordering,
                const BOOLEAN& _homogeneous=FALSE);
  /// Initializes weight vector with weights,
  /// weighted_block_size with number_of_weighted_variables,
  /// the orderings as given in the arguments
  /// and elimination_block_size with _number_of_elimination_variables.

  term_ordering(ifstream& input, const short& _weighted_ordering,
                const BOOLEAN& _homogeneous=FALSE);
  /// Reads a weight vector from the input stream and converts it into a
  /// weighted term ordering (with the refining ordering given by the second
  /// argument).
  /// The input stream must have the following format:
  ///
  ///    size n of the weight vector (=weighted_block_size)
  ///    components 0..n-1 of the weight vector
  ///

  term_ordering(const short& n, ifstream& input,
                const short& _weighted_ordering,
                const BOOLEAN& _homogeneous=FALSE);
  /// Reads a weight vector of size n from the input stream and converts it
  /// into a weighted term ordering (with the refining ordering given by the
  /// second argument).
  /// The input stream has the following format:
  ///
  ///    components 0..n-1 of the weight vector
  ///

  term_ordering(const term_ordering&);
  /// copy-constructor


  ~term_ordering();
  /// destructor

/// object properties

  short number_of_weighted_variables() const;
  /// Returns weighted_block_size.

  short weight_refinement() const;
  /// Returns weighted_ordering.

  short number_of_elimination_variables() const;
  /// Returns elimination_block_size.

  short elimination_refinement() const;
  /// Returns elimination_ordering.

  BOOLEAN is_homogeneous() const;
  /// Returns homogeneous.

  short error_status() const;
  /// Returns weighted_block_size if weighted_block_size<0, else 0
  /// (this is the "error flag").

  BOOLEAN is_nonnegative() const;
  /// Returns TRUE iff all components of the weight vector are >=0.
  /// If this is not the case, Buchberger's algorithm will perhaps not
  /// terminate using the actual ordering, i.e. the ordering is no
  /// well ordering.

  BOOLEAN is_positive() const;
  /// Returns TRUE ifs all components of the weight vector are >0.
  /// If this is not the case, this may lead to problems even if all weights
  /// are nonnegative: Zero weights do not give a well ordering in combination
  /// with a reverse lexicographical refinement.

/// frequently used comparison functions

  double weight(const Integer* v) const;
  /// Returns the weight of an Integer vector v with respect to weight_vector,
  /// i.e. the scalar product of the two vectors.
  /// UNCHECKED PRECONDITION: v has weighted_block_size components.
  /// The return type is double to avoid overflows.

  short compare_to_zero(const Integer* v) const;
  /// This function is used to determine a binomials's head with respect to the
  /// actual term ordering. The comparison of its two monomials is done
  /// according to the binomial data structure:
  /// Instead of comparing explicitely two monomials, the Integer vector
  /// corresponding to their difference is compared to the zero vector.
  /// The function returns
  ///          1, if v>0,
  ///             -1, if v<0,
  ///              0, if v=0
  /// UNCHECKED PRECONDITION:
  /// v has weighted_block_size + elimination_block_size components

  short compare(const binomial& bin1, const binomial& bin2) const;
  /// Compares the given binomials by first comparing the heads, then in the
  /// case of equality the tails.
  /// UNCHECKED PRECONDITION: The exponent vectors of the binomials have
  /// already the correct sign, i.e. the positive part of a vector is really
  /// its head and not its tail.
  /// The function returns
  ///        -1 if bin1<bin2
  ///         0 if bin1=bin2
  ///         1 if bin1>bin2

/// operators and routines needed by the algorithms to manipulate the term
/// ordering

  term_ordering& operator=(const term_ordering&);
  /// assignment operator with memory control

  float operator[](const short& i) const;
  /// Returns weight_vector[i] (FLT_MAX if i is out of range).

  term_ordering& convert_to_weighted_ordering();
  /// Sets elimination_block_size to 0.

  term_ordering& convert_to_elimination_ordering
  (const short& number_of_elimination_variables,
   const short& _elimination_ordering);
  /// Sets elimination_block_size to number_of_elimination_variables
  /// and the ordering on the elimination variables as given in the argument.

  term_ordering& append_weighted_variable(const float& weight);
  /// Appends the given weight to the weight vector (with memory control).

  term_ordering& delete_last_weighted_variable();
  /// Deletes the last weighted variable (with memory control).

  term_ordering& swap_weights(const short& i, const short& j);
  /// This function is used if two weighted variables must change their
  /// position (order) with respect to the refining ordering.

/// output

  void print_weight_vector() const;
  void print() const;
  /// Writes the term ordering to the standard output medium
  /// (only the weight vector or with supplementary information).

  void print_weight_vector(FILE*) const;
  void print(FILE*) const;
  /// Writes the term ordering to the referenced file
  /// which has to be opened for writing before.

  void print_weight_vector(ofstream&) const;
  void print(ofstream&) const;
  /// Writes the term ordering to the given ofstream.

  void format_print_weight_vector(ofstream&) const;
  /// Writes the term ordering to the given ofstream in the format required
  /// by the IP-algorithms.

};
#endif  /// TERM_ORDERING_H
