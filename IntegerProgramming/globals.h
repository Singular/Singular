// globals.h

// In this file
// - some facilities as the g++ input/output routines are included,
// - the general data type can be chosen (the people of Singular do not like
//   templates),
// - some makros are defined for an easier handling of the program,
// - some flags and constants can be set to observe the behaviour of the
//   algorithms when using different strategies.

#ifndef GLOBALS_H
#define GLOBALS_H
// Include facilities needed by several files:

// the following is not good! TODO: move to all including sources...

#include "singularconfig.h"


#include <stdio.h>
#ifndef HAVE_IOSTREAM_H
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#else
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <limits.h>
#endif
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

// Include a BigInt class from a foreign library:
// This is needed by the LLL algorithm. If no such class is included,
// it computes with long ints; this results in overflows and unexpected
// behaviour even when computing with relatively small instances.

#define GMP

#ifdef GMP

#include "BigInt.h"

#else   // GMP

typedef long BigInt;

#endif // GMP

// Define the general data type:
// short seems to be sufficient for most practicable instances.

typedef short Integer;
// The general data type to deal with can be short, long or int...

//#define _SHORT_
// For an overflow control for thE result of the LLL algorithm, we have to
// know the data type used.



// Define a BOOLEAN data type etc. for an easy-to-read code:

typedef char BOOLEAN;
#define TRUE  1
#define FALSE 0

#define MAXIMUM(a,b) (a>b? a:b)

#define INHOMOGENEOUS 0
#define HOMOGENEOUS   1
// For a more comfortable call of the term_ordering constructors.




// Enumerate the algorithms:
// The ideal constructor (builds an ideal from the input matrix) depends on
// the algorithm. The constants are defined to make constructor calls easier
// (you do not need to remember a number).

#define CONTI_TRAVERSO           1
#define POSITIVE_CONTI_TRAVERSO  2
#define POTTIER                  3
#define HOSTEN_STURMFELS         4
#define DIBIASE_URBANKE          5
#define BIGATTI_LASCALA_ROBBIANO 6




// Enumerate the criteria to discard unnecessary S-pairs:
// The constants are defined to allow an easier call of Buchberger´s algorithm
// with different criteria. This algorithm takes as an argument a short int
// which represents the combination of criteria to be used (with a default
// combination if no argument is given).
// The method to compute the argument for a certain combination is simple:
// Simply add the constants that belong to criteria you want to use. As the
// constants are chosen to be different powers of 2, each short int (in the
// range of 0 to 31) gives a unique combination (cf. the read-write-execute
// rights for files in UNIX systems).
// EXAMPLE:
// If you want to call Buchberger´s algorithm with the criterion "relatively
// prime leading terms" and the second criterion for the ideal I, write:
//
//      I.reduced_Groebner_basis_1(REL_PRIMENESS + SECOND_CRITERION);
// or
//      I.reduced_Groebner_basis_1(17);
//
// The argument 0 means that no criteria are used, the argument 31 leads to
// the use of all criteria.

#define REL_PRIMENESS      1
#define M_CRITERION        2
#define F_CRITERION        4
#define B_CRITERION        8
#define SECOND_CRITERION  16

// The names of tehse criteria are chosen according to the criteria
// described in Gebauer/Moeller (except from the last).
// The first one (relatively prime leading terms) is a local criterion
// and the most effective.
// The last four criteria are global ones involving searches over the
// generator lists. But although the lcm-computations performed by these
// checks are not much cheaper than a reduction, most of the criteria seem to
// accelerate the computations.
// REL_PRIMENESS should always be used.
// The Gebauer/Moeller-criteria (M,F,B) seem to improve computation time in
// the general case; Buchberger's second criterion seems to improve it in
// more special cases (for example the transportation problem). They should
// be switched on and off according to these results, but never be enabled
// together; the overhead of too many criteria leads to a rather bad
// performance in all cases.
// The relatively primeness criterion is tested first because it
// is very easy to check. The order of the Gebauer/Moeller-criteria does not
// really affect computation. The relative order of the Gebauer/Moeller
// criteria and the second criterion (if they are enabled together) has almost
// the same effect as switching off the last criterion in this order.
// There is no possibility to change the order in which the criteria are
// tested without changing the program!




// Enumerate the term orderings that can be used to refine the weight:
// W_REV_LEX will give a term ordering in the classical sense if and only if
// all weights are positive. It has been implemented because several
// algorithms need a reverse lexicographical refinement. (Of course, these
// algorithms control if they have a positive grading.)
// A warning will be output when using W_REV_LEX in an insecure case.

#define W_LEX          4
#define W_REV_LEX      5
#define W_DEG_LEX      6
#define W_DEG_REV_LEX  7



// Enumerate the term ordering used for the block of elimination variables:

#define LEX          1
#define DEG_LEX      2
#define DEG_REV_LEX  3

//////////////////////////////////////////////////////////////////////////////
///////////////     TEST PARAMETER SECTION    ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Set flags concerning the basic operations and the course of Buchberger's
// algorithm:

#define SUPPORT_DRIVEN_METHODS
// possibilities:
// SUPPORT_DRIVEN_METHODS
// NO_SUPPORT_DRIVEN_METHODS

// This flag allows to switch of and on the use of the support vectors in
// the binomial class; these are used to speed up tests for reducibility of
// binomials, relative primeness etc.

#define SUPPORT_DRIVEN_METHODS_EXTENDED
// possibilities:
// SUPPORT_DRIVEN_METHODS_EXTENDED
// NO_SUPPORT_DRIVEN_METHODS_EXTENDED

// This flag allows to switch of and on the extended use of the support
// information. This includes the splitting of the ideal generators into
// several lists according to their head support. This discards many
// unnecessary tests for divisibility etc.
// SUPPORT_DRIVEN_METHODS_EXTENDED should only be enabled when
// SUPPORT_DRIVEN_METHODS is set, too! The combination
// NO_SUPPORT_DRIVEN_METHODS and  SUPPORT_DRIVEN_METHODS_EXTENDED is quite
// senseless and will only work if your compiler automatically initializes
// integers to zero.

const unsigned short List_Support_Variables=8;

// This is the number of variables considered to create and maintain the
// generator lists of an ideal if SUPPORT_DRIVEN_METHODS_EXTENDED is
// enabled. As there are 2^List_Support_Variables lists, this number
// should not be too big (to avoid the overhead of many empty lists).
// The best number may depend on the problem size and structure...

#define SUPPORT_VARIABLES_LAST
// possibilities:
// SUPPORT_VARIABLES_LAST
// SUPPORT_VARIABLES_FIRST

// This flag determines whether the first or the last variables are considered
// by the support methods. So this setting will only affect the computation if
// such methods are enabled.
// The reason for the introduction of this flag is the following:
// The Conti-Traverso algorithm for solving an integer program works in two
// phases: Given an artificial solution in auxiliary variables, these
// variables are first eliminated to get a feasible solution. The elimination
// variables are for technical reasons always the last. In a second phase,
// this solution is reduced to a minimal solution with respect to a given
// term ordering.
// If SUPPORT_VARIABLES_LAST is set, the first phase will probably take
// less time as if SUPPORT_VARIABLES_FIRST is set. If one is only interested
// in finding a feasible solution (not necessary  minimal),
// SUPPORT_VARIABLES_LAST might be the better choice.

#endif  // GLOBALS_H

