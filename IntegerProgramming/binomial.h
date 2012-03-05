// binomial.h

// The binomials considered here are differences of power-products of a
// special kind. The initial monomial or head (with respect to a given
// term ordering as described in term_ordering.h) has coefficient 1, the other
// monomial (the tail) has coefficient -1. The head and the tail of a
// binomial are always relatively prime, i.e., they do not involve common
// variables.

// Such a binomial in n variables is represented as an Integer vector of
// size n: If the i-th variable is involved in the head with exponent k,
// the i-th component of the vector is set to k. If the i-th variable is
// involved in the tail with exponent k, the i-th component of the vector
// is set to -k.

// The members head_support and tail_support of type unsigned long have to be
// understood as bit-vectors with m components, where m is the number of
// bits in a long integer. The i-th bit of head_support tells us if the i-th
// variable is involved in the head of the binomial (for i<=m): it is 1
// iff this variable appears in the head. Analogously for tail_support.
// Using these vectors, the question if a binomial can be reduced by another
// can often be answered by performing a simple bitwise operation.

// To avoid global variables, the number of variables is taken by the
// constructors. But as it is the same for all appearing binomials during
// the run of our algorithms (unless some elimination is done), the member
// functions do not perform any range checks. This makes the algorithms
// more efficient.

// Most of the constructors do not perform sign checks on their arguments.
// The reason is simple: Unlike the matrix or term ordering constructors, some
// binomial constructors are frequently called during computation. As the
// new binomials are build from the input binomials, it is sufficient to do
// these checks on the input.



#ifndef BINOMIAL_H
#define BINOMIAL_H



class term_ordering;



class binomial
{



private:

  Integer* exponent_vector;

  short _number_of_variables;


#ifdef SUPPORT_DRIVEN_METHODS

  unsigned long head_support;
  unsigned long tail_support;

#endif  // SUPPORT_DRIVEN_METHODS



public:



// constructors and destructor

  binomial(const short& number_of_variables);
  // Allocates memory for a binomial of size number_of_variables
  // without initialization.
  // No range check on the argument!

  binomial(const short& number_of_variables, const Integer* exponents);
  binomial(const short& number_of_variables, const Integer* exponents,
           const term_ordering&);
  // These constructors build a binomial from its exponent vector/array.
  // The first one takes the positive components as its head, the negative
  // ones as its tail; the second one determines its head and tail with
  // respect to the given term ordering.
  // The array "exponents" is always copied. It would be faster to set
  // only pointers on it. However, this is very dangerous, and as these
  // constructors are only used at the beginning of the algorithm, it would
  // not really improve performance.
  // The Integer pointer exponents is declared as "const" to suggest that
  // the referenced array is not changed (although the "const"-declaration
  // does not assert this).
  // These constructor check the range (sign) of their argument
  // number_of_variables because it is not frequently called, but at a
  // critical point (ideal constructors).

  binomial(const binomial&);
  // copy-constructor
  // No check if the copied binomial is corrupt!

  ~binomial();
  // destructor



// object information

  short number_of_variables() const;
  // Returns the number of variables.

  short error_status() const;
  // Returns _number_of_variables iff _number_of_variables<0
  // (this is the "errror flag").



// assignment and access operators

  binomial& operator=(const binomial&);
  // assignment operator with memory control

  Integer operator[](const short& i) const;
  // Access operator for reading exponent_vector[i]
  // (cannot be used to overwrite exponent_vector[i]);
  // no range chack on i!



// comparison operators

  BOOLEAN operator==(const binomial& v) const;
  BOOLEAN operator!=(const binomial& v) const;
  // comparison of binomials

  BOOLEAN operator==(const Integer comp_value) const;
  BOOLEAN operator!=(const Integer comp_value) const;
  BOOLEAN operator<=(const Integer comp_value) const;
  BOOLEAN operator>=(const Integer comp_value) const;
  // operators for an efficient comparison with the zero binomial
  // (comp_value=0)



// basic routines for Buchbergers's algorithm

  Integer head_reductions_by(const binomial& b) const;
  // Returns the number of possible reductions of the actual binomial's head
  // by the binomial b.
  // The return value -1 means that b==0 or head(b)==1; one should not try
  // reduce by such a binomial (reduction is undefined or does not terminate).
  // The procedure reduce_tail_by(...) returns the unchanged binomial in such
  // a case without a warning. This is done in view of the application:
  // If a generator of an ideal is reduced to zero during a Groebner basis
  // calculation and one forgets to delete it, this generator is ignored
  // (else it would probably cause a fatal error).

  Integer tail_reductions_by(const binomial& b) const;
  // Returns the number of possible reductions of the actual binomial's tail
  // by the binomial b.
  // For the return value -1 see above.

  int reduce_head_by(const binomial& b, const term_ordering& w);
  // Performs a multiple reduction of the actual binomial's head by the
  // binomial b and computes the new head and tail with respect to the term
  // ordering w.
  // The return value is 1 if the binomial has really been reduced, 2 if
  // the binomial has been reduced to zero, 0 if the binomial has not been
  // changed.

  int reduce_tail_by(const binomial& b, const term_ordering& w);
  // Performs a multiple reduction of the actual binomial's tail by the
  // binomial b and computes the new head and tail with respect to the term
  // ordering w.
  // The return value is 1 if the binomial has really been reduced, else 0.
  // (By a tail reduction, the binomial cannot be reduced to zero if the
  // binomial itself and b are consistent with w, i.e. if head and tail are
  // correct.)

  friend binomial& S_binomial(const binomial& a, const binomial& b,
                              const term_ordering& w);
  // Computes the S-binomial of a and b with respect to the term ordering w
  // and returns a reference on it (the necessary memory is allocated during
  // the computation).



// criteria for unnecessary S-Pairs

  friend BOOLEAN relatively_prime(const binomial& a, const binomial& b);
  // Returns TRUE iff the leading terms of a and b are relatively prime.

  friend BOOLEAN M(const binomial& a,const binomial& b,const binomial& c);
  // Verifies if lcm(head(a),head(c)) divides properly lcm(head(b),head(c)).

  friend BOOLEAN F(const binomial& a, const binomial& b, const binomial& c);
  // Verifies if lcm(head(a),head(c))=lcm(head(b),head(c)).

  friend BOOLEAN B(const binomial& a, const binomial& b, const binomial& c);
  // Verifies if head(a) divides lcm(head(b),head(c)) and
  // lcm(head(a),head(b))!=lcm(head(b),head(c))!=lcm(head(a),head(c)).

  friend BOOLEAN second_crit(const binomial& a, const binomial& b,
                             const binomial& c);
  // Verifies if head(a) divides lcm(head(b),head(c)).



// special routines needed by the IP-algorithms

// All this routines are not very frequently used (especially not in
// Buchberger's algorithm), so I haven't spent much time in optimization.
// None of them performs range checks on their arguments.

  BOOLEAN involves_elimination_variables(const term_ordering& w);
  // Verifies if the binomial involves elimination variables with respect
  // to w.
  // UNCHECKED PRECONDITION: w and the binomial are compatible, i.e.
  // involve the same number of variables.

  BOOLEAN drop_elimination_variables(const term_ordering& w);
  // Cuts the elimination variables (with respect to w) from the binomial
  // and adapts the member _number_of_variables as well as the head and the
  // tail.
  // UNCHECKED PRECONDITION: w and the binomial are compatible.
  // The interesting part of the binomial (ot its additive inverse) is copied
  // hereby to be able to free the memory needed by the dropped components.

  BOOLEAN drop_last_weighted_variable(const term_ordering& w);
  // Cuts the last variable from the binomial and adapts the member
  // _number_of_variables as well as head and tail (the last two to the given
  // term_ordering)
  // UNCHECKED PRECONDITION: w is a weighted termordering (without elimination
  // variables) that is compatible to the actual binomial.
  // The interesting part of the binomial (ot its additive inverse) is copied
  // hereby to be able to free the memory needed by the dropped components.

  int adapt_to_term_ordering(const term_ordering&);
  // Determines head and tail of the binomial with respect to the given term
  // ordering; i.e. multiplies the exponent vector with -1 and exchanges
  // head_support and tail_support, if necessary.
  // Returns -1 if head and tail were exchanged, 1 else.
  // UNCHECKED PRECONDITION: w and the binomial are compatible.

  binomial& swap_variables(const short& i,const short& j);
  // Swaps the components i and j of the exponent vector and actualizes
  // head_support and tail_support.
  // No range check on the arguments!

  binomial& flip_variable(const short& i);
  // Inverts component i of the exponent vector and actualizes head_support
  // and tail_support.
  // No range check on i!



// output

  void print() const;
  void print_all() const;
  // Writes the actual binomial to the standard output medium.
  // The first routine only prints the exponent vector, the second prints
  // the head and tail support, too (if SUPPORT_DRIVEN_METHODS are used).

  void print(FILE* output) const;
  void print_all(FILE* output) const;
  // Writes the actual binomial to the referenced file which must have
  // been opened for writing before.

  void print(ofstream&) const;
  void print_all(ofstream&) const;
  // Writes the actual binomial to the given ofstream.

  void format_print(ofstream&) const;
  // Writes the given binomial to the actual ofstream in the format needed
  // by the IP-algorithms.

  friend class ideal;
  // The class ideal is declared as a friend for efficiency reasons:
  // This avoids many unnecessary function calls for inspecting members
  // like head_support and tail_support.
  // The class ideal does not change any members of the binomial class.

  friend short term_ordering::compare(const binomial&, const binomial&) const;


};


#endif  // BINOMIAL_H













