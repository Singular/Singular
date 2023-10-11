// IP_algorithms.h

#ifndef IP_ALGORITHMS_H
#define IP_ALGORITHMS_H

#include "ideal.h"

typedef char* INPUT_FILE;
typedef char* OUTPUT_FILE;

// This file contains the declaration of all implemented IP-algorithms.
// They only solve problems for nonnegative weight vectors.

// Let A an integral (mxn)-matrix, b a vector with m integral
// coefficients and c a vector with n nonnegative real coefficients.
// The solution of the IP-problem
//
//    minimize cx
//    under the conditions
//             Ax=b and all components of x are nonnegative integers
//
// proceeds in two steps:

// First, we have to compute the toric ideal of A and its Groebner basis
// with respect to a term ordering refining the cost function c. This can
// be done by the following procedures. They check the format of their input
// file (which should be a MATRIX file as described below) and return 1 if
// they were successful, 0 else.
//  They take as arguments:
// - their input file
// - the arguments for Buchberger´s algorithm (see the comments in ideal.h)
// - the output mode (verbose or not, default: not verbose; verbose mode
//   prints compiler settings and options for Buchberger´s algorithm)
// The last four arguments have default values and do not have to be
// specified.

// In the case of a successful computation, they write the computed Groebner
// basis into a GROEBNER file which is named as the input file with extension
// replaced by
//      .GB.<alg>
// where <alg> is an abbreviation for the used algorithm:
//         ct     for Conti_Traverso(...)
//        pct     for Positive_Conti_Traverso(...)
//        ect     for Elim_Conti_Traverso(...)
//         pt     for Pottier(...)
//         hs     for Hosten_Sturmfels(...)
//         du     for DiBiase_Urbanke(...)
//        blr     for Bigatti_LaScala_Robbiano(...)

extern int Conti_Traverso(INPUT_FILE MATRIX,
                          const int& version=1,
                          const int& S_pair_criteria=11,
                          const float& interred_percentage=12.0,
                          const BOOLEAN& verbose=FALSE);
// The complete algorithm of Conti and Traverso which computes the toric
// ideal of an extended matrix and which can be used for solving
// integer programs without initial solution.

extern int Positive_Conti_Traverso(INPUT_FILE MATRIX,
                                   const int& version=1,
                                   const int& S_pair_criteria=11,
                                   const float& interred_percentage=12.0,
                                   const BOOLEAN& verbose=FALSE);
// The variant of the above algorithm for matrices and right hand vector
// with only nonnegative entries using one elimination variable less.

extern int Elim_Conti_Traverso(INPUT_FILE MATRIX,
                               const int& version=1,
                               const int& S_pair_criteria=11,
                               const float& interred_percentage=12.0,
                               const BOOLEAN& verbose=FALSE);
// A version of the Conti-Traverso-algorithm which really computes the toric
// ideal of A. Used for test purposes (correctness and performance of the
// other algorithms). Nonnegativity is ignored.

extern int Pottier(INPUT_FILE MATRIX,
                   const int& version=1,
                   const int& S_pair_criteria=11,
                   const float& interred_percentage=12.0,
                   const BOOLEAN& verbose=FALSE);
// The algorithm proposed by Pottier using one elimination variable and
// a lattice basis of the matrix kernel to compute the toric ideal A.

extern int Hosten_Sturmfels(INPUT_FILE MATRIX,
                            const int& version=1,
                            const int& S_pair_criteria=11,
                            const float& interred_percentage=12.0,
                            const BOOLEAN& verbose=FALSE);
// The algorithm proposed by Hosten and Sturmfels which computes the toric
// ideal of A from its kernel lattice basis without supplementary
// variables, but with various Groebner basis calculations.

extern int DiBiase_Urbanke(INPUT_FILE MATRIX,
                           const int& version=1,
                           const int& S_pair_criteria=11,
                           const float& interred_percentage=12.0,
                           const BOOLEAN& verbose=FALSE);
// The algorithm proposed by DiBiase and Urbanke which computes the toric
// ideal of A from its kernel lattice basis using "variable flips".

extern int Bigatti_LaScala_Robbiano(INPUT_FILE MATRIX,
                                    const int& version=1,
                                    const int& S_pair_criteria=11,
                                    const float& interred_percentage=12.0,
                                    const BOOLEAN& verbose=FALSE);
// A modified version of the algorithm called EATI using "pseudo-elimination".
// This algorithm is quite similar to Pottier's algorithm, but deals with
// homogeneous binomials.

// The second step of the IP-solution is to reduce one or more given
// initial solutions (which also define right-hand vectors b) with respect
// to the computed Groebner basis. In the case that the Groebner basis
// was computed via the algorithm of Conti and Traverso, it is sufficient
// to give the right-hand vectors. In all other cases we need explicit
// initial solutions. The routine

extern int solve(INPUT_FILE PROBLEM, INPUT_FILE GROEBNER);

// solves the IP-problems given by the vectors in PROBLEM with respect
// to GROEBNER which should contain a Groebner basis as computed above. The
// output is appended to a (eventually newly created) file named as GROEBNER
// with extensions replaced by:
//      .sol.<alg>
// where <alg> is an abbreviation of the algorithm used to compute GROEBNER.

// We also provide functionality to recompute toric ideals with respect to
// another cost function:

extern int change_cost(INPUT_FILE GROEBNER, INPUT_FILE NEW_COST,
                       const int& version=1,
                       const int& S_pair_criteria=11,
                       const float& interred_percentage=12.0,
                       const BOOLEAN& verbose=FALSE);

// recomputes the Groebner basis in GROEBNER with respect to the cost given
// in NEW_COST and writes the result into the file named as NEW_COST with
// extension replaced by
//      .GB.<alg>
// where <alg> is an abbreviation of the algorithm used to compute GROEBNER.

//////////////////////////////////////////////////////////////////////////////
////////////      FORMAT OF INPUT AND OUTPUT FILES     ///////////////////////
//////////////////////////////////////////////////////////////////////////////

// The files appearing under the name MATRIX in the above declarations should
// look as follows to be accepted by the algorithms:

//      MATRIX                                  (* specifie format *)
//
//      columns:
//      <number of columns>
//
//      cost vector:
//      <coefficients of the cost vector>
//
//      rows:
//      <number of rows>
//
//      matrix:
//      <matrix coefficients>
//
//      positive row space vector:              (* optional, see below *)
//      <coefficients of row space vector>

// For example:

//      MATRIX
//
//      columns:
//      3
//
//      cost vector:
//      1 1 1
//
//      rows:
//      2
//
//      matrix:
//      1 2 3
//      4 5 6
//
//      positive row space vector:
//      1 2 3

// The positive row space vector is only needed by the algorithms of
// Hosten/Sturmfels and Bigatti/LaScala/Robbiano. It is ignored by the other
// algorithms, as well as further lines in the input file. This allows us to
// use the same input format for all algorithms. The comments (as the word
// "rows:") are only written into the file to make it easy to read.


// The file named NEW_COST in the change_cost procedure should have the
// following format:

//      NEW_COST
//
//      variables:                          (* only weighted variables *)
//      <number of weighted variables>
//
//      cost vector:
//      <coefficients o the weight vector>

// For convenience, the MATRIX format is also accepted by the change_cost
// procedure. The lines following the cost vector are then ignored.

// The files appearing under the name GROEBNER in the above declarations
// are created in the following form by the algorithms for computing toric
// ideals. This form is accepted by the solve(...) and the
// change_cost(...) procedures:

//      GROEBNER                            (* specifie format *)
//
//      computed with algorithm:
//      <algorithm name abbreviation>       (* abbreviations as above *)
//      from file(s):                       (* the following four lines are
//      <name of respective MATRIX file>       optional *)
//      computation time:
//      <computation time in seconds>
//
//      term ordering:
//      elimination block
//      <number of elimination variables>
//      <LEX / DEG_LEX                      (* only if number of elimination
//      / DEG_REV_LEX>                         variables >0 *)
//      weighted block
//      <number of weighted variables>
//      <W_LEX / W_REV_LEX                  (* number of weighted variables
//      / W_DEG_LEX / W_DEG_REV_LEX>           should always >0 *)
//      <weight_vector>
//
//      size:
//      <number of elements>
//
//      Groebner basis:
//      <basis elements>                    (* one basis element in each row,
//                                             given by the coefficients of
//                                             its vector representation *)
//      settings for the                    (* all following lines are
//      Buchberger algorithm:                  optional, verbose mode *)
//      version:                            (* as explained in ideal.h,
//      <version number>                       between 0 and 3 *)
//      S-pair criteria:
//      <{relatively prime leading terms,   (* a subset of these criteria *)
//        criterion M, criterion F,
//        criterion B, second criterion}>
//      interreduction percentage:
//      <interreduction percentage>
//
//      compiler settings:                  (* see the procedure
//      ...                                    print_flags(...) in
//      ...                                    IP_algorithms.cc *)

// If the GROEBNER file is created by change_cost(...), the algorithm name
// and MATRIX file are chosen as those of the original GROEBNER file; the
// new_cost file is specified as a second MATRIX file.


// The files appearing under the name PROBLEM in the above declarations should
// look as follows to be accepted by the procedure solve(...):

//      PROBLEM                                     (* specifie format *)
//
//      vector size:
//      <vector dimension>
//
//      number of instances:
//      <number of vectors>
//
//      right hand or initial solution vectors:
//      <vector coefficients>                       (* one vector per row *)


// solve(...) verifies if the vector size given in the PROBLEM file and
// the number of variables given in the GROEBNER file match the respective
// algorithm. In the matching case, it creates a SOLUTION file of the
// following format (not used as an input file for any procedure):

//      SOLUTION                                    (* specifie format *)
//
//      computed with algorithm:                    (* from the respective
//      <algorithm name abbreviation>                  GROEBNER file *)
//      from file:                                  (* the GROEBNER file *)
//      <file name>
//
//      <right hand/initial solution> vector:
//      <vector as in the problem file>
//      solvable:                                   (* only if right-hand
//      <YES/NO>                                       vector is given *)
//      optimal solution:                           (* only in the case of
//      <optimal solution vector>                      existence *)
//      computation time:                           (* only reduction time *)
//      <reduction time in seconds>
//
//      ...                                         (* further vectors *)

#endif
