#ifndef MINOR_INTERFACE_H
#define MINOR_INTERFACE_H

/* activate the next define if you want all basic operations
   counted and printed when one of the following methods
   is invoked
   (not fully implemented yet) */
/* #define COUNT_AND_PRINT_OPERATIONS 1 */

/* all computations are module char, if char <> 0;
   if additionally, an ideal i != NULL is given, then computations are
   modulo i (in this case, i is assumed to be a standard basis);
   if k = 0, then all non-zero minors are requested, otherwise
   if k > 0, then the first k non-zero minors are requested,
   if k < 0, then the first |k| minors (zero is allowed) are requested,
   (note that k <> 0 may result in a smaller number k' of
   minors if there are only k' < |k| minors;
   if an algorithm is provided, it must be one of "Bareiss" or
   "Laplace"; when a cache is used, the underlying algorithm
   is automatically Laplace */
ideal getMinorIdeal          (const matrix& m, const int minorSize, const int k,
                              const char* algorithm, const ideal& i,
                              const bool allDifferent);
ideal getMinorIdealCache     (const matrix& m, const int minorSize, const int k,
                              const ideal& i, const int cacheStrategy,
                              const int cacheN, const int cacheW,
                              const bool allDifferent);
ideal getMinorIdealHeuristic (const matrix& m, const int minorSize, const int k,
                              const ideal& i, const bool allDifferent);

#endif
/* MINOR_INTERFACE_H */
