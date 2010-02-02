#ifndef MINOR_INTERFACE_H
#define MINOR_INTERFACE_H

/* activate the next define if you want all basic operations
   counted and printed when one of the methods documented
   herein will be invoked (not fully implemented yet) */
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
   
/**
* Returns the specified set of minors (= subdeterminantes) of the
* given matrix. These minors form the set of generators of the ideal
* which is actually returned.<br>
* If k == 0, all non-zero minors will be computed. For k > 0, only
* the first k non-zero minors (to some fixed ordering among all minors)
* will be computed. Use k < 0 to compute the first |k| minors (including
* zero minors).<br>
* algorithm must be one of "Bareiss" and "Laplace".<br>
* i must be either NULL or an ideal capturing a standard basis. In the
* later case all minors will be reduced w.r.t. i.
* If allDifferent is true, each minor will be included as generator
* in the resulting ideal only once; otherwise as often as it occurs as
* minor value during the computation.
* @param m the matrix from which to compute minors
* @param minorSize the size of the minors to be computed
* @param k the number of minors to be computed
* @param algorithm the algorithm to be used for the computation
* @param i NULL or an ideal which encodes a standard basis
* @param allDifferent if true each minor is considered only once
* @return the ideal which has as generators the specified set of minors
*/
ideal getMinorIdeal (const matrix m, const int minorSize, const int k,
                     const char* algorithm, const ideal i,
                     const bool allDifferent);

/**
* Returns the specified set of minors (= subdeterminantes) of the
* given matrix. These minors form the set of generators of the ideal
* which is actually returned.<br>
* If k == 0, all non-zero minors will be computed. For k > 0, only
* the first k non-zero minors (to some fixed ordering among all minors)
* will be computed. Use k < 0 to compute the first |k| minors (including
* zero minors).<br>
* The underlying algorithm is Laplace's algorithm with caching of certain
* subdeterminantes. The caching strategy can be set; see
* int MinorValue::getUtility () const in Minor.cc. cacheN is the maximum
* number of cached polynomials (=subdeterminantes); cacheW the maximum
* weight of the cache during all computations.<br>
* i must be either NULL or an ideal capturing a standard basis. In the
* later case all minors will be reduced w.r.t. i.
* If allDifferent is true, each minor will be included as generator
* in the resulting ideal only once; otherwise as often as it occurs as
* minor value during the computation.
* @param m the matrix from which to compute minors
* @param minorSize the size of the minors to be computed
* @param k the number of minors to be computed
* @param i NULL or an ideal which encodes a standard basis
* @param cacheStrategy one of {1, .., 5}; see Minor.cc
* @param cacheN maximum number of cached polynomials (=subdeterminantes)
* @param cacheW maximum weight of the cache
* @param allDifferent if true each minor is considered only once
* @return the ideal which has as generators the specified set of minors
*/
ideal getMinorIdealCache (const matrix m, const int minorSize, const int k,
                          const ideal i, const int cacheStrategy,
                          const int cacheN, const int cacheW,
                          const bool allDifferent);

/**
* Returns the specified set of minors (= subdeterminantes) of the
* given matrix. These minors form the set of generators of the ideal
* which is actually returned.<br>
* If k == 0, all non-zero minors will be computed. For k > 0, only
* the first k non-zero minors (to some fixed ordering among all minors)
* will be computed. Use k < 0 to compute the first |k| minors (including
* zero minors).<br>
* The algorithm is heuristically chosen among "Bareiss", "Laplace",
* and Laplace with caching (of subdeterminants).<br>
* i must be either NULL or an ideal capturing a standard basis. In the
* later case all minors will be reduced w.r.t. i.
* If allDifferent is true, each minor will be included as generator
* in the resulting ideal only once; otherwise as often as it occurs as
* minor value during the computation.
* @param m the matrix from which to compute minors
* @param minorSize the size of the minors to be computed
* @param k the number of minors to be computed
* @param i NULL or an ideal which encodes a standard basis
* @param allDifferent if true each minor is considered only once
* @return the ideal which has as generators the specified set of minors
*/
ideal getMinorIdealHeuristic (const matrix m, const int minorSize,
                              const int k, const ideal i,
                              const bool allDifferent);

#endif
/* MINOR_INTERFACE_H */
