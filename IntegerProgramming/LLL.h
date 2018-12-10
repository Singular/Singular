/// LLL.h

/// This file contains the implementation of two special variants of the
/// LLL-algorithm.
/// For further explainations see the book of Henri Cohen, A Course in
/// Computational Algebraic Number Theory.

/// When performing the LLL-algorithm, some coefficients grow very fast.
/// Therefore one should use a  data type for arbitrary long integers
/// (called "BigInt" in the code).
/// If no such data type is specified, BigInt is defined to be a long
/// (cf. globals.h).

#ifndef LLL_H
#define LLL_H

#include "globals.h"

extern short relations(BigInt** b, const short& number_of_vectors,
                       const short& vector_dimension, BigInt**& H);
/// Computes the relations of the input vectors stored in b and returns the
/// dimension r of the lattice spanned by these relations.
/// The return value -1 indicates that an error has occurred.
/// A LLL-reduced basis of the relations is written into the two-dimensional
/// array H. Memory allocation for this array is done in the routine;
/// when leaving the routine, the dimension will be vector_columns x r.
/// This routine corresponds to algorithm 2.7.2 in Cohen's book.

extern short integral_LLL(BigInt **b, const short& number_of_vectors,
                          const short& vector_dimension);
/// Reduces the input vectors stored in b (in the sense of an LLL-reduction).
/// The input vectors have to be linearly independent.
/// ATTENTION: The input vectors are modified during this algorithm. For
/// efficiency reasons in our application, we do NOT store a transformation
/// matrix!
/// The return value is -1 if an error has occurred, 0 else.
/// This routine corresponds to algorithm 2.6.7 in Cohen's book.

#endif  /// LLL_H
