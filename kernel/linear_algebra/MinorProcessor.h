#ifndef MINOR_PROCESSOR_H
#define MINOR_PROCESSOR_H

#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "kernel/linear_algebra/Cache.h"
#include "kernel/linear_algebra/Minor.h"

// #include <assert.h>
#include <string>

/* write "##define COUNT_AND_PRINT_OPERATIONS x" if you want
   to count all basic operations and have them printed when
   one of the methods documented herein will be invoked;
   otherwise, comment this line;
   x = 1: only final counters (after computing ALL
          specified minors) will be printed, i.e., no
          intermediate results;
   x = 2: print counters after the computation of each
          minor; this will be much more information
   x = 3: print also all intermediate matrices with the
          numbers of monomials in each entry;
          this will be much much more information */
//#define COUNT_AND_PRINT_OPERATIONS 2

void printCounters (char* prefix, bool resetToZero);

/*! \class MinorProcessor
    \brief Class MinorProcessor implements the key methods for computing one
    or all sub-determinantes of a given size in a pre-defined matrix; either
    without caching or by using a cache.

    After defining the entire matrix (e.g. 10 x 14) using<br>
    MinorProcessor::defineMatrix (const int, const int, const int*),<br>
    the user may do two different things:<br>
    1. He/she can simply compute a minor in this matrix using<br>
    MinorProcessor::getMinor (const int, const int*, const int*,
                              Cache<MinorKey, MinorValue>&), or<br>
    MinorProcessor::getMinor (const int, const int*, const int*);<br>
    depending on whether a cache shall or shall not be used, respectively.<br>
    In the first case, the user simply provides all row and column indices of
    the desired minor.
    2. He/she may define a smaller sub-matrix (e.g. 8 x 7) using
    MinorValue::defineSubMatrix (const int, const int*, const int, const int*).
    Afterwards, he/she may compute all minors of an even smaller size (e.g.
    5 x 5) that consist exclusively of rows and columns of this (8 x 7)
    sub-matrix (inside the entire 10 x 14 matrix).<br>
    The implementation at hand eases the iteration over all such minors. Also
    in the second case there are both implementations, i.e., with and without
    using a cache.<br><br>
    MinorProcessor makes use of MinorKey, MinorValue, and Cache. The
    implementation of all mentioned classes (MinorKey, MinorValue, and
    MinorProcessor) is generic to allow for the use of different types of
    keys and values.
    \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
*/
class MinorProcessor
{
  protected:
    /**
    * A static method for computing the maximum number of retrievals of a
    * minor.<br>
    * More concretely, we are given a matrix of size \c rows x \c columns. We
    * furthermore assume that we have - as part of this matrix - a minor of
    * size \c containerMinorSize x \c containerMinorSize. Now we are
    * interested in the number of times a minor of yet smaller size
    * \c minorSize x \c minorSize will be needed when we compute the
    * containerMinor by Laplace's Theorem.<br>
    * The method returns the combinatorial results for both cases:
    * containerMinor is fixed within the matrix
    * (<c>multipleMinors == false</c>), or it can vary inside the matrix
    * (<c>multipleMinors == true</c>).<br>
    * The notion is here that we want to cache the small minor of size
    * \c minorSize x \c minorSize, i.e. compute it just once.
    * @param rows the number of rows of the underlying matrix
    * @param columns the number of columns of the underlying matrix
    * @param containerMinorSize the size of the container minor
    * @param minorSize the size of the small minor (which may be retrieved
    *        multiple times)
    * @param multipleMinors decides whether containerMinor is fixed within
    *        the underlying matrix or not
    * @return the number of times, the small minor will be needed when
    *         computing one or all containerMinors
    */
    static int NumberOfRetrievals (const int rows, const int columns,
                                   const int containerMinorSize,
                                   const int minorSize,
                                   const bool multipleMinors);
    /**
    * A static method for computing the binomial coefficient i over j.
    * \par Assert
    * The method checks whether <em>i >= j >= 0</em>.
    * @param i a positive integer greater than or equal to \a j
    * @param j a positive integer less than or equal to \a i, and greater
    *        than or equal to \e 0.
    * @return the binomial coefficient i over j
    */
    static int IOverJ (const int i, const int j);

    /**
    * A static method for computing the factorial of i.
    * \par Assert
    * The method checks whether <em>i >= 0</em>.
    * @param i an integer greater than or equal to \a 0
    * @return the factorial of i
    */
    static int Faculty (const int i);

    /**
    * A method for iterating through all possible subsets of \c k rows and
    * \c k columns inside a pre-defined submatrix of a pre-defined matrix.<br>
    * The method will set \c _rowKey and \c columnKey to represent the
    * next possible subsets of \c k rows and columns inside the submatrix
    * determined by \c _globalRowKey and \c _globalColumnKey.<br>
    * When first called, this method will just shift \c _rowKey and
    * \c _columnKey to point to the first sensible choices. Every subsequent
    * call will move to the next \c _columnKey until there is no next.
    * In this situation, a next \c _rowKey will be set, and \c _columnKey
    * again to the first possible choice.<br>
    * Finally, in case there is also no next \c _rowkey, the method returns
    * \c false. (Otherwise \c true is returned.)
    * @param k the size of the minor / all minors of interest
    * @return true iff there is a next possible choice of rows and columns
    */
    bool setNextKeys (const int k);

    /**
    * private store for the rows and columns of the container minor within
    * the underlying matrix;
    * \c _container will be used to fix a submatrix (e.g. 40 x 50) of a
    * larger matrix (e.g. 70 x 100). This is useful when we would like to
    * compute all minors of a given size (e.g. 4 x 4) inside such a
    * pre-defined submatrix.
    */
    MinorKey _container;

    /**
    * private store for the number of rows in the container minor;
    * This is set by MinorProcessor::defineSubMatrix (const int, const int*,
    *                                                 const int, const int*).
    */
    int _containerRows;

    /**
    * private store for the number of columns in the container minor;
    * This is set by MinorProcessor::defineSubMatrix (const int, const int*,
    *                                                 const int, const int*).
    */
    int _containerColumns;

    /**
    * private store for the rows and columns of the minor of interest;
    * Usually, this minor will encode subsets of the rows and columns in
    * _container.
    */
    MinorKey _minor;

    /**
    * private store for the dimension of the minor(s) of interest
    */
    int _minorSize;

    /**
    * private store for the number of rows in the underlying matrix
    */
    int _rows;

    /**
    * private store for the number of columns in the underlying matrix
    */
    int _columns;

    /**
    * A method for identifying the row or column with the most zeros.<br>
    * Using Laplace's Theorem, a minor can more efficiently be computed when
    * developing along this best line.
    * The returned index \c bestIndex is 0-based within the pre-defined
    * matrix. If some row has the most zeros, then the (0-based) row index is
    * returned. If, contrarywise, some column has the most zeros, then
    * <c>x = - 1 - c</c> where \c c is the column index, is returned.
    * (Note that in this case \c c can be reconstructed by computing
    * <c>c = - 1 - x</c>.)
    * @param k the size of the minor / all minors of interest
    * @param mk the representation of rows and columns of the minor of
    *        interest
    * @return an int encoding which row or column has the most zeros
    */
    int getBestLine (const int k, const MinorKey& mk) const;

    /**
    * A method for testing whether a matrix entry is zero.
    * @param absoluteRowIndex the absolute (zero-based) row index
    * @param absoluteColumnIndex the absolute (zero-based) column index
    * @return true iff the specified matrix entry is zero
    */
    virtual bool isEntryZero (const int absoluteRowIndex,
                              const int absoluteColumnIndex) const;
  public:
    /**
    * The default constructor
    */
    MinorProcessor ();

    /**
     * A destructor for deleting an instance. We must make this destructor
     * virtual so that destructors of all derived classes will automatically
     * also call the destructor of the base class MinorProcessor.
     */
     virtual ~MinorProcessor ();

    /**
    * A method for defining a sub-matrix within a pre-defined matrix.
    * @param numberOfRows the number of rows in the sub-matrix
    * @param rowIndices an array with the (0-based) indices of rows inside
    *        the pre-defined matrix
    * @param numberOfColumns the number of columns in the sub-matrix
    * @param columnIndices an array with the (0-based) indices of columns
    *        inside the pre-defined matrix
    * @see MinorValue::defineMatrix (const int, const int, const int*)
    */
    void defineSubMatrix (const int numberOfRows, const int* rowIndices,
                          const int numberOfColumns, const int* columnIndices);

    /**
    * Sets the size of the minor(s) of interest.<br>
    * This method needs to be performed before beginning to compute all minors
    * of size \a minorSize inside a pre-defined submatrix of an underlying
    * (also pre-defined) matrix.
    * @param minorSize the size of the minor(s) of interest
    * @see MinorValue::defineSubMatrix (const int, const int*, const int,
    *                                   const int*)
    */
    void setMinorSize (const int minorSize);

    /**
    * A method for checking whether there is a next choice of rows and columns
    * when iterating through all minors of a given size within a pre-defined
    * sub-matrix of an underlying matrix.<br>
    * The number of rows and columns has to be set before using
    * MinorValue::setMinorSize(const int).<br>
    * After calling MinorValue::hasNextMinor (), the current sets of rows and
    * columns may be inspected using
    * MinorValue::getCurrentRowIndices(int* const) const and
    * MinorValue::getCurrentColumnIndices(int* const) const.
    * @return true iff there is a next choice of rows and columns
    * @see MinorProcessor::getMinor (const int, const int*, const int*)
    * @see MinorValue::getCurrentRowIndices(int* const) const
    * @see MinorValue::getCurrentColumnIndices(int* const) const
    */
    bool hasNextMinor ();

    /**
    * A method for obtaining the current set of rows corresponding to the
    * current minor when iterating through all minors of a given size within
    * a pre-defined sub-matrix of an underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * The user of this method needs to know the number of rows in order to
    * know which entries of the newly filled \c target will be valid.
    * @param target an int array to be filled with the row indices
    * @see MinorProcessor::hasNextMinor ()
    */
    void getCurrentRowIndices (int* const target) const;

    /**
    * A method for obtaining the current set of columns corresponding to the
    * current minor when iterating through all minors of a given size within
    * a pre-defined sub-matrix of an underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * The user of this method needs to know the number of columns in order to
    * know which entries of the newly filled \c target will be valid.
    * @param target an int array to be filled with the column indices
    * @see MinorProcessor::hasNextMinor ()
    */
    void getCurrentColumnIndices (int* const target) const;

    /**
    * A method for providing a printable version of the represented
    * MinorProcessor.
    * @return a printable version of the given instance as instance of class
    * string
    */
   virtual std::string toString () const;

    /**
    * A method for printing a string representation of the given
    * MinorProcessor to std::cout.
    */
    void print () const;
};

/*! \class IntMinorProcessor
    \brief Class IntMinorProcessor is derived from class MinorProcessor.

    This class implements the special case of integer matrices.
    \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
*/
class IntMinorProcessor : public MinorProcessor
{
  private:
    /**
    * private store for integer matrix entries
    */
    int* _intMatrix;

    /**
    * A method for retrieving the matrix entry.
    * @param rowIndex the absolute (zero-based) row index
    * @param columnIndex the absolute (zero-based) column index
    * @return the specified matrix entry
    */
    int getEntry (const int rowIndex, const int columnIndex) const;

    /**
    * A method for computing the value of a minor, using a cache.<br>
    * The sub-matrix is specified by \c mk. Computation works recursively
    * using Laplace's Theorem. We always develop along the row or column with
    * the most zeros; see
    * MinorProcessor::getBestLine (const int k, const MinorKey& mk).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis. Moreover, if the given
    * characteristic is non-zero, all results will be computed modulo this
    * characteristic.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param multipleMinors decides whether we compute just one or all minors
    *        of a specified size
    * @param c a cache to be used for caching reusable sub-minors
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivateLaplace (const int k,
                                                   const MinorKey& mk,
                                                   const int characteristic,
                                                   const ideal& iSB)
    */
    IntMinorValue getMinorPrivateLaplace (const int k, const MinorKey& mk,
                                          const bool multipleMinors,
                                          Cache<MinorKey, IntMinorValue>& c,
                                          int characteristic,
                                          const ideal& iSB);

    /**
    * A method for computing the value of a minor, without using a cache.<br>
    * The sub-matrix is specified by \c mk. Computation works recursively
    * using Laplace's Theorem. We always develop along the row or column with
    * the most zeros; see
    * MinorProcessor::getBestLine (const int k, const MinorKey& mk).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis. Moreover, if the given
    * characteristic is non-zero, all results will be computed modulo this
    * characteristic.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivateLaplace (const int k,
                                                   const MinorKey& mk,
                                                   const bool multipleMinors,
                                                   Cache<MinorKey,
                                                         IntMinorValue>& c,
                                                   int characteristic,
                                                   const ideal& iSB)
    */
    IntMinorValue getMinorPrivateLaplace (const int k, const MinorKey& mk,
                                          const int characteristic,
                                          const ideal& iSB);

    /**
    * A method for computing the value of a minor using Bareiss's
    * algorithm.<br>
    * The sub-matrix is specified by \c mk.
    * If an ideal is given, it is assumed to be a standard basis. In this
    * case, all results will be reduced w.r.t. to this basis. Moreover, if the
    * given characteristic is non-zero, all results will be computed modulo
    * this characteristic.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivateLaplace (const int k,
                                                   const MinorKey& mk,
                                                   const int characteristic,
                                                   const ideal& iSB)
    */
    IntMinorValue getMinorPrivateBareiss (const int k, const MinorKey& mk,
                                          const int characteristic,
                                          const ideal& iSB);
  protected:
    /**
    * A method for testing whether a matrix entry is zero.
    * @param absoluteRowIndex the absolute (zero-based) row index
    * @param absoluteColumnIndex the absolute (zero-based) column index
    * @return true iff the specified matrix entry is zero
    */
    bool isEntryZero (const int absoluteRowIndex,
                      const int absoluteColumnIndex) const;
  public:
    /**
    * A constructor for creating an instance.
    */
    IntMinorProcessor ();

    /**
    * A destructor for deleting an instance.
    */
    ~IntMinorProcessor ();

    /**
    * A method for defining a matrix with integer entries.
    * @param numberOfRows the number of rows
    * @param numberOfColumns the number of columns
    * @param matrix the matrix entries in a linear array, i.e., from left to
    *        right and top to bottom
    * @see MinorValue::defineSubMatrix (const int, const int*, const int,
    *                                   const int*)
    */
    void defineMatrix (const int numberOfRows, const int numberOfColumns,
                       const int* matrix);

    /**
    * A method for computing the value of a minor without using a cache.<br>
    * The sub-matrix is determined by \c rowIndices and \c columnIndices.
    * Computation works either by Laplace's algorithm or by Bareiss's
    * algorithm.<br>
    * If an ideal is given, it is assumed to be a standard basis. In this
    * case, all results will be reduced w.r.t. to this basis. Moreover, if the
    * given characteristic is non-zero, all results will be computed modulo
    * this characteristic.
    * @param dimension the size of the minor to be computed
    * @param rowIndices 0-based indices of the rows of the minor
    * @param columnIndices 0-based indices of the column of the minor
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @param algorithm either "Bareiss" or "Laplace"
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinor (const int dimension,
                                     const int* rowIndices,
                                     const int* columnIndices,
                                     Cache<MinorKey, IntMinorValue>& c,
                                     const int characteristic,
                                     const ideal& iSB)
    */
    IntMinorValue getMinor (const int dimension, const int* rowIndices,
                            const int* columnIndices,
                            const int characteristic, const ideal& iSB,
                            const char* algorithm);

    /**
    * A method for computing the value of a minor using a cache.<br>
    * The sub-matrix is determined by \c rowIndices and \c columnIndices.
    * Computation works by Laplace's algorithm together with caching of
    * subdeterminants.<br>
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis. Moreover, if the given
    * characteristic is non-zero, all results will be computed modulo this
    * characteristic.
    * @param dimension the size of the minor to be computed
    * @param rowIndices 0-based indices of the rows of the minor
    * @param columnIndices 0-based indices of the column of the minor
    * @param c the cache for storing subdeterminants
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinor (const int dimension,
                                     const int* rowIndices,
                                     const int* columnIndices,
                                     const int characteristic,
                                     const ideal& iSB,
                                     const char* algorithm)
    */
    IntMinorValue getMinor (const int dimension, const int* rowIndices,
                            const int* columnIndices,
                            Cache<MinorKey, IntMinorValue>& c,
                            const int characteristic,  const ideal& iSB);

    /**
    * A method for obtaining the next minor when iterating
    * through all minors of a given size within a pre-defined sub-matrix of an
    * underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * Computation works by Laplace's algorithm (without using a cache) or by
    * Bareiss's algorithm.<br>
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis. Moreover, if the given
    * characteristic is non-zero, all results will be computed modulo this
    * characteristic.
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @param algorithm either "Bareiss" or "Laplace"
    * @return the next minor
    * @see IntMinorValue::getNextMinor (Cache<MinorKey, IntMinorValue>& c,
    *                                   const int characteristic,
    *                                   const ideal& iSB)
    */
    IntMinorValue getNextMinor (const int characteristic, const ideal& iSB,
                                const char* algorithm);

    /**
    * A method for obtaining the next minor when iterating
    * through all minors of a given size within a pre-defined sub-matrix of an
    * underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * Computation works using the cache \a c which may already contain useful
    * results from previous calls of
    * IntMinorValue::getNextMinor (Cache<MinorKey, IntMinorValue>& c,
                                   const int characteristic,
                                   const ideal& iSB).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis. Moreover, if the given
    * characteristic is non-zero, all results will be computed modulo this
    * characteristic.
    * @param c the cache
    * @param characteristic 0 or the characteristic of the underlying
    *        coefficient ring/field
    * @param iSB NULL or a standard basis
    * @return the next minor
    * @see IntMinorValue::getNextMinor (const int characteristic,
    *                                   const ideal& iSB,
    *                                   const char* algorithm)
    */
    IntMinorValue getNextMinor (Cache<MinorKey, IntMinorValue>& c,
                                const int characteristic,
                                const ideal& iSB);

    /**
    * A method for providing a printable version of the represented
    * MinorProcessor.
    * @return a printable version of the given instance as instance of class
    *         string
    */
   std::string toString () const;
};

/*! \class PolyMinorProcessor
    \brief Class PolyMinorProcessor is derived from class MinorProcessor.

    This class implements the special case of polynomial matrices.
    \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
*/
class PolyMinorProcessor : public MinorProcessor
{
  private:
    /**
    * private store for polynomial matrix entries
    */
    poly* _polyMatrix;

    /**
    * A method for retrieving the matrix entry.
    * @param rowIndex the absolute (zero-based) row index
    * @param columnIndex the absolute (zero-based) column index
    * @return the specified matrix entry
    */
    poly getEntry (const int rowIndex, const int columnIndex) const;

    /**
    * A method for computing the value of a minor, using a cache.<br>
    * The sub-matrix is specified by \c mk. Computation works recursively
    * using Laplace's Theorem. We always develop along the row or column with
    * the most zeros; see
    * MinorProcessor::getBestLine (const int k, const MinorKey& mk).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param multipleMinors decides whether we compute just one or all minors
    *        of a specified size
    * @param c a cache to be used for caching reusable sub-minors
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivateLaplace (const int k,
    *                                              const MinorKey& mk,
    *                                              const ideal& iSB)
    */
    PolyMinorValue getMinorPrivateLaplace (const int k, const MinorKey& mk,
                                           const bool multipleMinors,
                                           Cache<MinorKey, PolyMinorValue>& c,
                                           const ideal& iSB);

    /**
    * A method for computing the value of a minor, without using a cache.<br>
    * The sub-matrix is specified by \c mk. Computation works recursively
    * using Laplace's Theorem. We always develop along the row or column with
    * the most zeros; see
    * MinorProcessor::getBestLine (const int k, const MinorKey& mk).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivate (const int, const MinorKey&,
    *                                       const bool,
    *                                       Cache<MinorKey, MinorValue>&)
    */
    PolyMinorValue getMinorPrivateLaplace (const int k, const MinorKey& mk,
                                           const ideal& iSB);

    /**
    * A method for computing the value of a minor, without using a cache.<br>
    * The sub-matrix is specified by \c mk. Computation works
    * using Bareiss's algorithm.
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param k the number of rows and columns in the minor to be computed
    * @param mk the representation of rows and columns of the minor to be
    *        computed
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinorPrivateLaplace (const int k,
    *                                              const MinorKey& mk,
    *                                              const ideal& iSB)
    */
    PolyMinorValue getMinorPrivateBareiss (const int k, const MinorKey& mk,
                                           const ideal& iSB);
  protected:
    /**
    * A method for testing whether a matrix entry is zero.
    * @param absoluteRowIndex the absolute (zero-based) row index
    * @param absoluteColumnIndex the absolute (zero-based) column index
    * @return true iff the specified matrix entry is zero
    */
    bool isEntryZero (const int absoluteRowIndex,
                      const int absoluteColumnIndex) const;
  public:
    /**
    * A constructor for creating an instance.
    */
    PolyMinorProcessor ();

    /**
    * A destructor for deleting an instance.
    */
    ~PolyMinorProcessor ();

    /**
    * A method for defining a matrix with polynomial entries.
    * @param numberOfRows the number of rows
    * @param numberOfColumns the number of columns
    * @param polyMatrix the matrix entries in a linear array, i.e., from left
    *        to right and top to bottom
    * @see MinorValue::defineSubMatrix (const int, const int*, const int,
    *                                   const int*)
    */
    void defineMatrix (const int numberOfRows, const int numberOfColumns,
                       const poly* polyMatrix);

    /**
    * A method for computing the value of a minor, without using a cache.<br>
    * The sub-matrix is determined by \c rowIndices and \c columnIndices.
    * Computation works either by Laplace's algorithm or by Bareiss's
    * algorithm.<br>
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param dimension the size of the minor to be computed
    * @param rowIndices 0-based indices of the rows of the minor
    * @param columnIndices 0-based indices of the column of the minor
    * @param algorithm either "Laplace" or "Bareiss"
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::getMinor (const int dimension,
    *                                const int* rowIndices,
    *                                const int* columnIndices,
    *                                Cache<MinorKey, PolyMinorValue>& c,
    *                                const ideal& iSB)
    */
    PolyMinorValue getMinor (const int dimension, const int* rowIndices,
                             const int* columnIndices, const char* algorithm,
                             const ideal& iSB);

    /**
    * A method for computing the value of a minor, using a cache.<br>
    * The sub-matrix is determined by \c rowIndices and \c columnIndices.
    * Computation works recursively using Laplace's Theorem. We always develop
    * along the row or column with most zeros; see
    * MinorProcessor::getBestLine (const int, const int, const int).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param dimension the size of the minor to be computed
    * @param rowIndices 0-based indices of the rows of the minor
    * @param columnIndices 0-based indices of the column of the minor
    * @param c a cache to be used for caching reusable sub-minors
    * @param iSB NULL or a standard basis
    * @return an instance of MinorValue representing the value of the
    *         corresponding minor
    * @see MinorProcessor::(const int dimension, const int* rowIndices,
    *                       const int* columnIndices, const char* algorithm,
    *                       const ideal& iSB)
    */
    PolyMinorValue getMinor (const int dimension, const int* rowIndices,
                             const int* columnIndices,
                             Cache<MinorKey, PolyMinorValue>& c,
                             const ideal& iSB);

    /**
    * A method for obtaining the next minor when iterating
    * through all minors of a given size within a pre-defined sub-matrix of an
    * underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * Computation works either by Laplace's algorithm (without using a cache)
    * or by Bareiss's algorithm.<br>
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param algorithm either "Laplace" or "Bareiss"
    * @param iSB NULL or a standard basis
    * @return true iff there is a next choice of rows and columns
    * @see PolyMinorValue::getNextMinor (Cache<MinorKey, PolyMinorValue>& c,
    *                                    const ideal& iSB)
    */
    PolyMinorValue getNextMinor (const char* algorithm, const ideal& iSB);

    /**
    * A method for obtaining the next minor when iterating
    * through all minors of a given size within a pre-defined sub-matrix of an
    * underlying matrix.<br>
    * This method should only be called after MinorProcessor::hasNextMinor ()
    * had been called and yielded \c true.<br>
    * Computation works using Laplace's algorithm and a cache \a c which may
    * already contain useful results from previous calls of
    * PolyMinorValue::getNextMinor (Cache<MinorKey, PolyMinorValue>& c,
    *                               const ideal& iSB).
    * If an ideal is given, it is assumed to be a standard basis. In this case,
    * all results will be reduced w.r.t. to this basis.
    * @param iSB NULL or a standard basis
    * @return the next minor
    * @see PolyMinorValue::getNextMinor (const char* algorithm,
    *                                    const ideal& iSB)
    */
    PolyMinorValue getNextMinor (Cache<MinorKey, PolyMinorValue>& c,
                                 const ideal& iSB);

    /**
    * A method for providing a printable version of the represented
    * MinorProcessor.
    * @return a printable version of the given instance as instance of class
    *         string
    */
   std::string toString () const;
};

#endif
/* MINOR_PROCESSOR_H */
