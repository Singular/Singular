


#include "kernel/mod2.h"

#include "kernel/linear_algebra/MinorProcessor.h"

#include "polys/kbuckets.h"

#include "kernel/structs.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/kstd1.h"

#include "kernel/ideals.h"

using namespace std;

#ifdef COUNT_AND_PRINT_OPERATIONS
VAR long addsPoly        = 0;    /* for the number of additions of two polynomials */
VAR long multsPoly       = 0;    /* for the number of multiplications of two polynomials */
VAR long addsPolyForDiv  = 0;    /* for the number of additions of two polynomials for
                                polynomial division part */
VAR long multsPolyForDiv = 0;    /* for the number of multiplications of two polynomials
                                for polynomial division part */
VAR long multsMon        = 0;    /* for the number of multiplications of two monomials */
VAR long multsMonForDiv  = 0;    /* for the number of m-m-multiplications for polynomial
                                division part */
VAR long savedMultsMFD   = 0;    /* number of m-m-multiplications that could be saved
                                when polynomial division would be optimal
                                (if p / t1 = t2 + ..., then t1 * t2 = LT(p), i.e.,
                                this multiplication need not be performed which
                                would save one m-m-multiplication) */
VAR long divsMon         = 0;    /* for the number of divisions of two monomials;
                                these are all guaranteed to work, i.e., m1/m2 only
                                when exponentVector(m1) >= exponentVector(m2) */
void printCounters (char* prefix, bool resetToZero)
{
  printf("%s [p+p(div) | p*p(div) | m*m(div, -save) | m/m ]", prefix);
  printf(" = [%ld(%ld) | %ld(%ld) | %ld(%d, -%ld) | %ld]\n",
         addsPoly, addsPolyForDiv, multsPoly, multsPolyForDiv,
         multsMon, multsMonForDiv, savedMultsMFD, divsMon);
  if (resetToZero)
  {
    multsMon = 0; addsPoly = 0; multsPoly = 0; divsMon = 0;
    savedMultsMFD = 0; multsMonForDiv = 0; addsPolyForDiv = 0;
    multsPolyForDiv = 0;
  }
}
#endif
/* COUNT_AND_PRINT_OPERATIONS */

void MinorProcessor::print() const
{
  PrintS(this->toString().c_str());
}

int MinorProcessor::getBestLine (const int k, const MinorKey& mk) const
{
  /* This method identifies the row or column with the most zeros.
     The returned index (bestIndex) is absolute within the pre-
     defined matrix.
     If some row has the most zeros, then the absolute (0-based)
     row index is returned.
     If, contrariwise, some column has the most zeros, then -1 minus
     the absolute (0-based) column index is returned. */
  int numberOfZeros = 0;
  int bestIndex = 100000;    /* We start with an invalid row/column index. */
  int maxNumberOfZeros = -1; /* We update this variable whenever we find
                                a new so-far optimal row or column. */
  for (int r = 0; r < k; r++)
  {
    /* iterate through all k rows of the momentary minor */
    int absoluteR = mk.getAbsoluteRowIndex(r);
    numberOfZeros = 0;
    for (int c = 0; c < k; c++)
    {
      int absoluteC = mk.getAbsoluteColumnIndex(c);
      if (isEntryZero(absoluteR, absoluteC)) numberOfZeros++;
    }
    if (numberOfZeros > maxNumberOfZeros)
    {
      /* We found a new best line which is a row. */
      bestIndex = absoluteR;
      maxNumberOfZeros = numberOfZeros;
    }
  };
  for (int c = 0; c < k; c++)
  {
    int absoluteC = mk.getAbsoluteColumnIndex(c);
    numberOfZeros = 0;
    for (int r = 0; r < k; r++)
    {
      int absoluteR = mk.getAbsoluteRowIndex(r);
      if (isEntryZero(absoluteR, absoluteC)) numberOfZeros++;
    }
    if (numberOfZeros > maxNumberOfZeros)
    {
      /* We found a new best line which is a column. So we transform
         the return value. Note that we can easily retrieve absoluteC
         from bestLine: absoluteC = - 1 - bestLine. */
      bestIndex = - absoluteC - 1;
      maxNumberOfZeros = numberOfZeros;
    }
  };
  return bestIndex;
}

void MinorProcessor::setMinorSize(const int minorSize)
{
  _minorSize = minorSize;
  _minor.reset();
}

bool MinorProcessor::hasNextMinor()
{
  return setNextKeys(_minorSize);
}

void MinorProcessor::getCurrentRowIndices(int* const target) const
{
  return _minor.getAbsoluteRowIndices(target);
}

void MinorProcessor::getCurrentColumnIndices(int* const target) const
{
  return _minor.getAbsoluteColumnIndices(target);
}

void MinorProcessor::defineSubMatrix(const int numberOfRows,
                                     const int* rowIndices,
                                     const int numberOfColumns,
                                     const int* columnIndices)
{
  /* The method assumes ascending row and column indices in the
     two argument arrays. These indices are understood to be zero-based.
     The method will set the two arrays of ints in _container.
     Example: The indices 0, 2, 3, 7 will be converted to an array with
     one int representing the binary number 10001101
     (check bits from right to left). */

  _containerRows = numberOfRows;
  int highestRowIndex = rowIndices[numberOfRows - 1];
  int rowBlockCount = (highestRowIndex / 32) + 1;
  unsigned *rowBlocks=(unsigned*)omAlloc(rowBlockCount*sizeof(unsigned));
  for (int i = 0; i < rowBlockCount; i++) rowBlocks[i] = 0;
  for (int i = 0; i < numberOfRows; i++)
  {
    int blockIndex = rowIndices[i] / 32;
    int offset = rowIndices[i] % 32;
    rowBlocks[blockIndex] += (1 << offset);
  }

  _containerColumns = numberOfColumns;
  int highestColumnIndex = columnIndices[numberOfColumns - 1];
  int columnBlockCount = (highestColumnIndex / 32) + 1;
  unsigned *columnBlocks=(unsigned*)omAlloc0(columnBlockCount*sizeof(unsigned));
  for (int i = 0; i < numberOfColumns; i++)
  {
    int blockIndex = columnIndices[i] / 32;
    int offset = columnIndices[i] % 32;
    columnBlocks[blockIndex] += (1 << offset);
  }

  _container.set(rowBlockCount, rowBlocks, columnBlockCount, columnBlocks);
  omFree(columnBlocks);
  omFree(rowBlocks);
}

bool MinorProcessor::setNextKeys(const int k)
{
  /* This method moves _minor to the next valid (k x k)-minor within
     _container. It returns true iff this is successful, i.e. iff
     _minor did not already encode the terminal (k x k)-minor. */
  if (_minor.compare(MinorKey(0, 0, 0, 0)) == 0)
  {
    /* This means that we haven't started yet. Thus, we are about
       to compute the first (k x k)-minor. */
    _minor.selectFirstRows(k, _container);
    _minor.selectFirstColumns(k, _container);
    return true;
  }
  else if (_minor.selectNextColumns(k, _container))
  {
    /* Here we were able to pick a next subset of columns
       within the same subset of rows. */
    return true;
  }
  else if (_minor.selectNextRows(k, _container))
  {
    /* Here we were not able to pick a next subset of columns
       within the same subset of rows. But we could pick a next
       subset of rows. We must hence reset the subset of columns: */
    _minor.selectFirstColumns(k, _container);
    return true;
  }
  else
  {
    /* We were neither able to pick a next subset
       of columns nor of rows. I.e., we have iterated through
       all sensible choices of subsets of rows and columns. */
    return false;
  }
}

bool MinorProcessor::isEntryZero (const int /*absoluteRowIndex*/,
                                  const int /*absoluteColumnIndex*/) const
{
  assume(false);
  return false;
}

string MinorProcessor::toString () const
{
  assume(false);
  return "";
}

int MinorProcessor::IOverJ(const int i, const int j)
{
  /* This is a non-recursive implementation. */
  assume( (i >= 0) && (j >= 0) && (i >= j));
  if (j == 0 || i == j) return 1;
  int result = 1;
  for (int k = i - j + 1; k <= i; k++) result *= k;
  /* Now, result = (i - j + 1) * ... * i. */
  for (int k = 2; k <= j; k++) result /= k;
  /* Now, result = (i - j + 1) * ... * i / 1 / 2 ...
     ... / j = i! / j! / (i - j)!. */
  return result;
}

int MinorProcessor::Faculty(const int i)
{
  /* This is a non-recursive implementation. */
  assume(i >= 0);
  int result = 1;
  for (int j = 1; j <= i; j++) result *= j;
  // Now, result = 1 * 2 * ... * i = i!
  return result;
}

int MinorProcessor::NumberOfRetrievals (const int rows, const int columns,
                                        const int containerMinorSize,
                                        const int minorSize,
                                        const bool multipleMinors)
{
  /* This method computes the number of potential retrievals
     of a single minor when computing all minors of a given size
     within a matrix of given size. */
  int result = 0;
  if (multipleMinors)
  {
    /* Here, we would like to compute all minors of size
       containerMinorSize x containerMinorSize in a matrix
       of size rows x columns.
       Then, we need to retrieve any minor of size
       minorSize x minorSize exactly n times, where n is as
       follows: */
    result = IOverJ(rows - minorSize, containerMinorSize - minorSize)
           * IOverJ(columns - minorSize, containerMinorSize - minorSize)
           * Faculty(containerMinorSize - minorSize);
  }
  else
  {
    /* Here, we would like to compute just one minor of size
       containerMinorSize x containerMinorSize. Then, we need
       to retrieve any minor of size minorSize x minorSize exactly
       (containerMinorSize - minorSize)! times: */
    result = Faculty(containerMinorSize - minorSize);
  }
  return result;
}

MinorProcessor::MinorProcessor () :
  _container(0, NULL, 0, NULL),
  _minor(0, NULL, 0, NULL),
  _containerRows(0),
  _containerColumns(0),
  _minorSize(0),
  _rows(0),
  _columns(0)
{
}

MinorProcessor::~MinorProcessor () { }

IntMinorProcessor::IntMinorProcessor ()
{
  _intMatrix = 0;
}

string IntMinorProcessor::toString () const
{
  char h[32];
  string t = "";
  string s = "IntMinorProcessor:";
  s += "\n   matrix: ";
  sprintf(h, "%d", _rows); s += h;
  s += " x ";
  sprintf(h, "%d", _columns); s += h;
  for (int r = 0; r < _rows; r++)
  {
    s += "\n      ";
    for (int c = 0; c < _columns; c++)
    {
      sprintf(h, "%d", getEntry(r, c)); t = h;
      for (int k = 0; k < int(4 - strlen(h)); k++) s += " ";
      s += t;
    }
  }
  int myIndexArray[500];
  s += "\n   considered submatrix has row indices: ";
  _container.getAbsoluteRowIndices(myIndexArray);
  for (int k = 0; k < _containerRows; k++)
  {
    if (k != 0) s += ", ";
    sprintf(h, "%d", myIndexArray[k]); s += h;
  }
  s += " (first row of matrix has index 0)";
  s += "\n   considered submatrix has column indices: ";
  _container.getAbsoluteColumnIndices(myIndexArray);
  for (int k = 0; k < _containerColumns; k++)
  {
    if (k != 0) s += ", ";
    sprintf(h, "%d", myIndexArray[k]); s += h;
  }
  s += " (first column of matrix has index 0)";
  s += "\n   size of considered minor(s): ";
  sprintf(h, "%d", _minorSize); s += h;
  s += "x";
  s += h;
  return s;
}

IntMinorProcessor::~IntMinorProcessor()
{
  /* free memory of _intMatrix */
  delete [] _intMatrix; _intMatrix = 0;
}

bool IntMinorProcessor::isEntryZero (const int absoluteRowIndex,
                                     const int absoluteColumnIndex) const
{
  return getEntry(absoluteRowIndex, absoluteColumnIndex) == 0;
}

void IntMinorProcessor::defineMatrix (const int numberOfRows,
                                      const int numberOfColumns,
                                      const int* matrix)
{
  /* free memory of _intMatrix */
  omFree(_intMatrix); _intMatrix = NULL;

  _rows = numberOfRows;
  _columns = numberOfColumns;

  /* allocate memory for new entries in _intMatrix */
  int n = _rows * _columns;
  _intMatrix = (int*)omAlloc(n*sizeof(int));

  /* copying values from one-dimensional method
     parameter "matrix" */
  for (int i = 0; i < n; i++)
    _intMatrix[i] = matrix[i];
}

IntMinorValue IntMinorProcessor::getMinor(const int dimension,
                                          const int* rowIndices,
                                          const int* columnIndices,
                                          Cache<MinorKey, IntMinorValue>& c,
                                          const int characteristic,
                                          const ideal& iSB)
{
  defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
  _minorSize = dimension;
  /* call a helper method which recursively computes the minor using the
     cache c: */
  return getMinorPrivateLaplace(dimension, _container, false, c,
                                characteristic, iSB);
}

IntMinorValue IntMinorProcessor::getMinor(const int dimension,
                                          const int* rowIndices,
                                          const int* columnIndices,
                                          const int characteristic,
                                          const ideal& iSB,
                                          const char* algorithm)
{
  defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
  _minorSize = dimension;

  /* call a helper method which computes the minor (without a cache): */
  if (strcmp(algorithm, "Laplace") == 0)
    return getMinorPrivateLaplace(_minorSize, _container, characteristic,
                                  iSB);
  else if (strcmp(algorithm, "Bareiss") == 0)
    return getMinorPrivateBareiss(_minorSize, _container, characteristic,
                                  iSB);
  else assume(false);

  /* The following code is never reached and just there to make the
     compiler happy: */
  return IntMinorValue();
}

IntMinorValue IntMinorProcessor::getNextMinor(const int characteristic,
                                              const ideal& iSB,
                                              const char* algorithm)
{
  /* call a helper method which computes the minor (without a cache): */
  if (strcmp(algorithm, "Laplace") == 0)
    return getMinorPrivateLaplace(_minorSize, _minor, characteristic, iSB);
  else if (strcmp(algorithm, "Bareiss") == 0)
    return getMinorPrivateBareiss(_minorSize, _minor, characteristic, iSB);
  else assume(false);

  /* The following code is never reached and just there to make the
     compiler happy: */
  return IntMinorValue();
}

IntMinorValue IntMinorProcessor::getNextMinor(Cache<MinorKey,
                                              IntMinorValue>& c,
                                              const int characteristic,
                                              const ideal& iSB)
{
    /* computation with cache */
    return getMinorPrivateLaplace(_minorSize, _minor, true, c, characteristic,
                                  iSB);
}

/* computes the reduction of an integer i modulo an ideal
   which captures a std basis */
int getReduction (const int i, const ideal& iSB)
{
  if (i == 0) return 0;
  poly f = pISet(i);
  poly g = kNF(iSB, currRing->qideal, f);
  int result = 0;
  if (g != NULL) result = n_Int(pGetCoeff(g), currRing->cf);
  pDelete(&f);
  pDelete(&g);
  return result;
}

IntMinorValue IntMinorProcessor::getMinorPrivateLaplace(
     const int k,
     const MinorKey& mk,
     const int characteristic,
     const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  /* The method works by recursion, and using Lapace's Theorem along the
     row/column with the most zeros. */
  if (k == 1)
  {
    int e = getEntry(mk.getAbsoluteRowIndex(0), mk.getAbsoluteColumnIndex(0));
    if (characteristic != 0) e = e % characteristic;
    if (iSB != 0) e = getReduction(e, iSB);
    return IntMinorValue(e, 0, 0, 0, 0, -1, -1); /* "-1" is to signal that any
                                                    statistics about the number
                                                    of retrievals does not make
                                                    sense, as we do not use a
                                                    cache. */
  }
  else
  {
    /* Here, the minor must be 2x2 or larger. */
    int b = getBestLine(k, mk);                   /* row or column with most
                                                     zeros */
    int result = 0;                               /* This will contain the
                                                     value of the minor. */
    int s = 0; int m = 0; int as = 0; int am = 0; /* counters for additions and
                                                     multiplications, ..."a*"
                                                     for accumulated operation
                                                     counters */
    bool hadNonZeroEntry = false;
    if (b >= 0)
    {
      /* This means that the best line is the row with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in minorRowKey: */
      int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
      for (int c = 0; c < k; c++) /* This iterates over all involved columns. */
      {
        int absoluteC = mk.getAbsoluteColumnIndex(c);
        if (getEntry(b, absoluteC) != 0) /* Only then do we have to consider
                                            this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* Next MinorKey is mk with row b and column absoluteC omitted: */
          MinorKey subMk = mk.getSubMinorKey(b, absoluteC);
          IntMinorValue mv = getMinorPrivateLaplace(k - 1, subMk,
                             characteristic, iSB);  /* recursive call */
          m += mv.getMultiplications();
          s += mv.getAdditions();
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          /* adding sub-determinante times matrix entry
             times appropriate sign: */
          result += sign * mv.getResult() * getEntry(b, absoluteC);

          if (characteristic != 0) result = result % characteristic;
          s++; m++; as++, am++; /* This is for the last addition and
                                   multiplication. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    else
    {
      b = - b - 1;
      /* This means that the best line is the column with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in
         minorColumnKey: */
      int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
      for (int r = 0; r < k; r++) /* This iterates over all involved rows. */
      {
        int absoluteR = mk.getAbsoluteRowIndex(r);
        if (getEntry(absoluteR, b) != 0) /* Only then do we have to consider
                                            this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* Next MinorKey is mk with row absoluteR and column b omitted. */
          MinorKey subMk = mk.getSubMinorKey(absoluteR, b);
          IntMinorValue mv = getMinorPrivateLaplace(k - 1, subMk, characteristic, iSB); /* recursive call */
          m += mv.getMultiplications();
          s += mv.getAdditions();
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          /* adding sub-determinante times matrix entry
             times appropriate sign: */
          result += sign * mv.getResult() * getEntry(absoluteR, b);
          if (characteristic != 0) result = result % characteristic;
          s++; m++; as++, am++; /* This is for the last addition and
                                   multiplication. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    if (hadNonZeroEntry)
    {
      s--; as--; /* first addition was 0 + ..., so we do not count it */
    }
    if (s < 0) s = 0; /* may happen when all subminors are zero and no
                         addition needs to be performed */
    if (as < 0) as = 0; /* may happen when all subminors are zero and no
                           addition needs to be performed */
    if (iSB != 0) result = getReduction(result, iSB);
    IntMinorValue newMV(result, m, s, am, as, -1, -1);
    /* "-1" is to signal that any statistics about the number of retrievals
       does not make sense, as we do not use a cache. */
    return newMV;
  }
}

/* This method can only be used in the case of coefficients
   coming from a field or at least from an integral domain. */
IntMinorValue IntMinorProcessor::getMinorPrivateBareiss(
     const int k,
     const MinorKey& mk,
     const int characteristic,
     const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  int *theRows=(int*)omAlloc(k*sizeof(int));
  mk.getAbsoluteRowIndices(theRows);
  int *theColumns=(int*)omAlloc(k*sizeof(int));
  mk.getAbsoluteColumnIndices(theColumns);
  /* the next line provides the return value for the case k = 1 */
  int e = getEntry(theRows[0], theColumns[0]);
  if (characteristic != 0) e = e % characteristic;
  if (iSB != 0) e = getReduction(e, iSB);
  IntMinorValue mv(e, 0, 0, 0, 0, -1, -1);
  if (k > 1)
  {
    /* the matrix to perform Bareiss with */
    long *tempMatrix=(long*)omAlloc(k * k *sizeof(long));
    /* copy correct set of entries from _intMatrix to tempMatrix */
    int i = 0;
    for (int r = 0; r < k; r++)
      for (int c = 0; c < k; c++)
      {
        e = getEntry(theRows[r], theColumns[c]);
        if (characteristic != 0) e = e % characteristic;
        tempMatrix[i++] = e;
      }
    /* Bareiss algorithm operating on tempMatrix which is at least 2x2 */
    int sign = 1;   /* This will store the correct sign resulting
                       from permuting the rows of tempMatrix. */
    int *rowPermutation=(int*)omAlloc(k*sizeof(int));
                    /* This is for storing the permutation of rows
                       resulting from searching for a non-zero
                       pivot element. */
    for (int i = 0; i < k; i++) rowPermutation[i] = i;
    int divisor = 1;   /* the Bareiss divisor */
    for (int r = 0; r <= k - 2; r++)
    {
      /* look for a non-zero entry in column r: */
      int i = r;
      while ((i < k) && (tempMatrix[rowPermutation[i] * k + r] == 0))
        i++;
      if (i == k)
        /* There is no non-zero entry; hence the minor is zero. */
        return IntMinorValue(0, 0, 0, 0, 0, -1, -1);
      if (i != r)
      {
        /* We swap the rows with indices r and i: */
        int j = rowPermutation[i];
        rowPermutation[i] = rowPermutation[r];
        rowPermutation[r] = j;
        /* Now we know that tempMatrix[rowPermutation[r] * k + r] is not zero.
           But careful; we have to negate the sign, as there is always an odd
           number of row transpositions to swap two given rows of a matrix. */
        sign = -sign;
      }
      if (r >= 1) divisor = tempMatrix[rowPermutation[r - 1] * k + r - 1];
      for (int rr = r + 1; rr < k; rr++)
        for (int cc = r + 1; cc < k; cc++)
        {
          e = rowPermutation[rr] * k + cc;
          /* Attention: The following may cause an overflow and
             thus a wrong result: */
          tempMatrix[e] = tempMatrix[e] * tempMatrix[rowPermutation[r] * k + r]
                         - tempMatrix[rowPermutation[r] * k + cc]
                         * tempMatrix[rowPermutation[rr] * k + r];
          /* The following is, by theory, always a division without
             remainder: */
          tempMatrix[e] = tempMatrix[e] / divisor;
          if (characteristic != 0)
            tempMatrix[e] = tempMatrix[e] % characteristic;
        }
      omFree(rowPermutation);
      omFree(tempMatrix);
    }
    int theValue = sign * tempMatrix[rowPermutation[k - 1] * k + k - 1];
    if (iSB != 0) theValue = getReduction(theValue, iSB);
    mv = IntMinorValue(theValue, 0, 0, 0, 0, -1, -1);
  }
  omFree(theRows);
  omFree(theColumns);
  return mv;
}

int IntMinorProcessor::getEntry (const int rowIndex,
                                 const int columnIndex) const
{
  return _intMatrix[rowIndex * _columns + columnIndex];
}

IntMinorValue IntMinorProcessor::getMinorPrivateLaplace(
     const int k, const MinorKey& mk,
     const bool multipleMinors,
     Cache<MinorKey, IntMinorValue>& cch,
     const int characteristic, const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  /* The method works by recursion, and using Lapace's Theorem along
     the row/column with the most zeros. */
  if (k == 1)
  {
    int e = getEntry(mk.getAbsoluteRowIndex(0), mk.getAbsoluteColumnIndex(0));
    if (characteristic != 0) e = e % characteristic;
    if (iSB != 0) e = getReduction(e, iSB);
    return IntMinorValue(e, 0, 0, 0, 0, -1, -1);
    /* we set "-1" as, for k == 1, we do not have any cache retrievals */
  }
  else
  {
    int b = getBestLine(k, mk);                   /* row or column with
                                                     most zeros */
    int result = 0;                               /* This will contain the
                                                     value of the minor. */
    int s = 0; int m = 0; int as = 0; int am = 0; /* counters for additions
                                                     and multiplications,
                                                     ..."a*" for
                                                     accumulated operation
                                                     counters */
    IntMinorValue mv(0, 0, 0, 0, 0, 0, 0);        /* for storing all
                                                     intermediate minors */
    bool hadNonZeroEntry = false;
    if (b >= 0)
    {
      /* This means that the best line is the row with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be
         iterating; the initial sign depends on the relative index of b
         in minorRowKey: */
      int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
      for (int c = 0; c < k; c++) /* This iterates over all involved
                                     columns. */
      {
        int absoluteC = mk.getAbsoluteColumnIndex(c);
        if (getEntry(b, absoluteC) != 0) /* Only then do we have to consider
                                            this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* Next MinorKey is mk with row b and column absoluteC omitted. */
          MinorKey subMk = mk.getSubMinorKey(b, absoluteC);
          if (cch.hasKey(subMk))
          { /* trying to find the result in the cache */
              mv = cch.getValue(subMk);
              mv.incrementRetrievals(); /* once more, we made use of the cached
                                           value for key mk */
              cch.put(subMk, mv); /* We need to do this with "put", as the
                                     (altered) number of retrievals may have
                                     an impact on the internal ordering among
                                     the cached entries. */
          }
          else
          {
              mv = getMinorPrivateLaplace(k - 1, subMk, multipleMinors, cch,
                   characteristic, iSB); /* recursive call */
              /* As this minor was not in the cache, we count the additions
                 and multiplications that we needed to perform in the
                 recursive call: */
              m += mv.getMultiplications();
              s += mv.getAdditions();
          }
          /* In any case, we count all nested operations in the accumulative
             counters: */
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          /* adding sub-determinante times matrix entry times appropriate
             sign */
          result += sign * mv.getResult() * getEntry(b, absoluteC);
          if (characteristic != 0) result = result % characteristic;
          s++; m++; as++; am++; /* This is for the last addition and
                                   multiplication. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    else
    {
      b = - b - 1;
      /* This means that the best line is the column with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in
         minorColumnKey: */
      int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
      for (int r = 0; r < k; r++) /* This iterates over all involved rows. */
      {
        int absoluteR = mk.getAbsoluteRowIndex(r);
        if (getEntry(absoluteR, b) != 0) /* Only then do we have to consider
                                            this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* Next MinorKey is mk with row absoluteR and column b omitted. */
          MinorKey subMk = mk.getSubMinorKey(absoluteR, b);
          if (cch.hasKey(subMk))
          { /* trying to find the result in the cache */
            mv = cch.getValue(subMk);
            mv.incrementRetrievals(); /* once more, we made use of the cached
                                         value for key mk */
            cch.put(subMk, mv); /* We need to do this with "put", as the
                                   (altered) number of retrievals may have an
                                   impact on the internal ordering among the
                                   cached entries. */
          }
          else
          {
            mv = getMinorPrivateLaplace(k - 1, subMk, multipleMinors, cch, characteristic, iSB); /* recursive call */
            /* As this minor was not in the cache, we count the additions and
               multiplications that we needed to do in the recursive call: */
            m += mv.getMultiplications();
            s += mv.getAdditions();
          }
          /* In any case, we count all nested operations in the accumulative
             counters: */
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          /* adding sub-determinante times matrix entry times appropriate
             sign: */
          result += sign * mv.getResult() * getEntry(absoluteR, b);
          if (characteristic != 0) result = result % characteristic;
          s++; m++; as++; am++; /* This is for the last addition and
                                   multiplication. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    /* Let's cache the newly computed minor: */
    int potentialRetrievals = NumberOfRetrievals(_containerRows,
                                                 _containerColumns,
                                                 _minorSize, k,
                                                 multipleMinors);
    if (hadNonZeroEntry)
    {
      s--; as--; /* first addition was 0 + ..., so we do not count it */
    }
    if (s < 0) s = 0; /* may happen when all subminors are zero and no
                         addition needs to be performed */
    if (as < 0) as = 0; /* may happen when all subminors are zero and no
                           addition needs to be performed */
    if (iSB != 0) result = getReduction(result, iSB);
    IntMinorValue newMV(result, m, s, am, as, 1, potentialRetrievals);
    cch.put(mk, newMV); /* Here's the actual put inside the cache. */
    return newMV;
  }
}

PolyMinorProcessor::PolyMinorProcessor ()
{
  _polyMatrix = NULL;
}

poly PolyMinorProcessor::getEntry (const int rowIndex,
                                   const int columnIndex) const
{
  return _polyMatrix[rowIndex * _columns + columnIndex];
}

bool PolyMinorProcessor::isEntryZero (const int absoluteRowIndex,
                                      const int absoluteColumnIndex) const
{
  return getEntry(absoluteRowIndex, absoluteColumnIndex) == NULL;
}

string PolyMinorProcessor::toString () const
{
  char h[32];
  string t = "";
  string s = "PolyMinorProcessor:";
  s += "\n   matrix: ";
  sprintf(h, "%d", _rows); s += h;
  s += " x ";
  sprintf(h, "%d", _columns); s += h;
  int myIndexArray[500];
  s += "\n   considered submatrix has row indices: ";
  _container.getAbsoluteRowIndices(myIndexArray);
  for (int k = 0; k < _containerRows; k++)
  {
    if (k != 0) s += ", ";
    sprintf(h, "%d", myIndexArray[k]); s += h;
  }
  s += " (first row of matrix has index 0)";
  s += "\n   considered submatrix has column indices: ";
  _container.getAbsoluteColumnIndices(myIndexArray);
  for (int k = 0; k < _containerColumns; k++)
  {
    if (k != 0) s += ", ";
    sprintf(h, "%d", myIndexArray[k]); s += h;
  }
  s += " (first column of matrix has index 0)";
  s += "\n   size of considered minor(s): ";
  sprintf(h, "%d", _minorSize); s += h;
  s += "x";
  s += h;
  return s;
}

PolyMinorProcessor::~PolyMinorProcessor()
{
  /* free memory of _polyMatrix */
  int n = _rows * _columns;
  for (int i = 0; i < n; i++)
    p_Delete(&_polyMatrix[i], currRing);
  omfree(_polyMatrix); _polyMatrix = NULL;
}

void PolyMinorProcessor::defineMatrix (const int numberOfRows,
                                       const int numberOfColumns,
                                       const poly* polyMatrix)
{
  /* free memory of _polyMatrix */
  int n = _rows * _columns;
  for (int i = 0; i < n; i++)
    p_Delete(&_polyMatrix[i], currRing);
  omfree(_polyMatrix); _polyMatrix = NULL;

  _rows = numberOfRows;
  _columns = numberOfColumns;
  n = _rows * _columns;

  /* allocate memory for new entries in _polyMatrix */
  _polyMatrix = (poly*)omAlloc(n*sizeof(poly));

  /* copying values from one-dimensional method
     parameter "polyMatrix" */
  for (int i = 0; i < n; i++)
    _polyMatrix[i] = pCopy(polyMatrix[i]);
}

PolyMinorValue PolyMinorProcessor::getMinor(const int dimension,
                                            const int* rowIndices,
                                            const int* columnIndices,
                                            Cache<MinorKey, PolyMinorValue>& c,
                                            const ideal& iSB)
{
  defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
  _minorSize = dimension;
  /* call a helper method which recursively computes the minor using the cache
     c: */
  return getMinorPrivateLaplace(dimension, _container, false, c, iSB);
}

PolyMinorValue PolyMinorProcessor::getMinor(const int dimension,
                                            const int* rowIndices,
                                            const int* columnIndices,
                                            const char* algorithm,
                                            const ideal& iSB)
{
  defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
  _minorSize = dimension;
  /* call a helper method which computes the minor (without using a cache): */
  if (strcmp(algorithm, "Laplace") == 0)
    return getMinorPrivateLaplace(_minorSize, _container, iSB);
  else if (strcmp(algorithm, "Bareiss") == 0)
    return getMinorPrivateBareiss(_minorSize, _container, iSB);
  else assume(false);

  /* The following code is never reached and just there to make the
     compiler happy: */
  return PolyMinorValue();
}

PolyMinorValue PolyMinorProcessor::getNextMinor(const char* algorithm,
                                                const ideal& iSB)
{
  /* call a helper method which computes the minor (without using a
     cache): */
  if (strcmp(algorithm, "Laplace") == 0)
    return getMinorPrivateLaplace(_minorSize, _minor, iSB);
  else if (strcmp(algorithm, "Bareiss") == 0)
    return getMinorPrivateBareiss(_minorSize, _minor, iSB);
  else assume(false);

  /* The following code is never reached and just there to make the
     compiler happy: */
  return PolyMinorValue();
}

PolyMinorValue PolyMinorProcessor::getNextMinor(Cache<MinorKey,
                                                PolyMinorValue>& c,
                                                const ideal& iSB)
{
    /* computation with cache */
    return getMinorPrivateLaplace(_minorSize, _minor, true, c, iSB);
}

/* assumes that all entries in polyMatrix are already reduced w.r.t. iSB */
PolyMinorValue PolyMinorProcessor::getMinorPrivateLaplace(const int k,
                                                          const MinorKey& mk,
                                                          const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  /* The method works by recursion, and using Lapace's Theorem along the
     row/column with the most zeros. */
  if (k == 1)
  {
    PolyMinorValue pmv(getEntry(mk.getAbsoluteRowIndex(0),
                                mk.getAbsoluteColumnIndex(0)),
                       0, 0, 0, 0, -1, -1);
    /* "-1" is to signal that any statistics about the number of retrievals
       does not make sense, as we do not use a cache. */
    return pmv;
  }
  else
  {
    /* Here, the minor must be 2x2 or larger. */
    int b = getBestLine(k, mk);                   /* row or column with most
                                                     zeros */
    poly result = NULL;                           /* This will contain the
                                                     value of the minor. */
    int s = 0; int m = 0; int as = 0; int am = 0; /* counters for additions
                                                     and multiplications,
                                                     ..."a*" for accumulated
                                                     operation counters */
    bool hadNonZeroEntry = false;
    if (b >= 0)
    {
      /* This means that the best line is the row with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in minorRowKey: */
      int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
      for (int c = 0; c < k; c++) /* This iterates over all involved columns. */
      {
        int absoluteC = mk.getAbsoluteColumnIndex(c);
        if (!isEntryZero(b, absoluteC)) /* Only then do we have to consider
                                           this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* Next MinorKey is mk with row b and column absoluteC omitted. */
          MinorKey subMk = mk.getSubMinorKey(b, absoluteC);
          /* recursive call: */
          PolyMinorValue mv = getMinorPrivateLaplace(k - 1, subMk, iSB);
          m += mv.getMultiplications();
          s += mv.getAdditions();
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          poly signPoly = pISet(sign);
          poly temp = pp_Mult_qq(mv.getResult(), getEntry(b, absoluteC),
                                 currRing);
          temp = p_Mult_q(signPoly, temp, currRing);
          result = p_Add_q(result, temp, currRing);
#ifdef COUNT_AND_PRINT_OPERATIONS
          multsPoly++;
          addsPoly++;
          multsMon += pLength(mv.getResult()) * pLength(getEntry(b, absoluteC));
#endif
          s++; m++; as++, am++; /* This is for the addition and multiplication
                                   in the previous lines of code. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    else
    {
      b = - b - 1;
      /* This means that the best line is the column with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in
         minorColumnKey: */
      int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
      for (int r = 0; r < k; r++) /* This iterates over all involved rows. */
      {
        int absoluteR = mk.getAbsoluteRowIndex(r);
        if (!isEntryZero(absoluteR, b)) /* Only then do we have to consider
                                           this sub-determinante. */
        {
          hadNonZeroEntry = true;
          /* This is mk with row absoluteR and column b omitted. */
          MinorKey subMk = mk.getSubMinorKey(absoluteR, b);
          /* recursive call: */
          PolyMinorValue mv = getMinorPrivateLaplace(k - 1, subMk, iSB);
          m += mv.getMultiplications();
          s += mv.getAdditions();
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          poly signPoly = pISet(sign);
          poly temp = pp_Mult_qq(mv.getResult(), getEntry(absoluteR, b),
                                 currRing);
          temp = p_Mult_q(signPoly, temp, currRing);
          result = p_Add_q(result, temp, currRing);
#ifdef COUNT_AND_PRINT_OPERATIONS
          multsPoly++;
          addsPoly++;
          multsMon += pLength(mv.getResult()) * pLength(getEntry(absoluteR, b));
#endif
          s++; m++; as++, am++; /* This is for the addition and multiplication
                                   in the previous lines of code. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    if (hadNonZeroEntry)
    {
      s--; as--; /* first addition was 0 + ..., so we do not count it */
#ifdef COUNT_AND_PRINT_OPERATIONS
      addsPoly--;
#endif
    }
    if (s < 0) s = 0; /* may happen when all subminors are zero and no
                         addition needs to be performed */
    if (as < 0) as = 0; /* may happen when all subminors are zero and no
                           addition needs to be performed */
    if (iSB != NULL)
    {
      poly tmpresult = kNF(iSB, currRing->qideal, result);
      pDelete(&result);
      result=tmpresult;
    }
    PolyMinorValue newMV(result, m, s, am, as, -1, -1);
    /* "-1" is to signal that any statistics about the number of retrievals
       does not make sense, as we do not use a cache. */
    pDelete(&result);
    return newMV;
  }
}

/* assumes that all entries in polyMatrix are already reduced w.r.t. iSB */
PolyMinorValue PolyMinorProcessor::getMinorPrivateLaplace(
     const int k,
     const MinorKey& mk,
     const bool multipleMinors,
     Cache<MinorKey, PolyMinorValue>& cch,
     const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  /* The method works by recursion, and using Lapace's Theorem along
     the row/column with the most zeros. */
  if (k == 1)
  {
    PolyMinorValue pmv(getEntry(mk.getAbsoluteRowIndex(0),
                                mk.getAbsoluteColumnIndex(0)),
                       0, 0, 0, 0, -1, -1);
    /* we set "-1" as, for k == 1, we do not have any cache retrievals */
    return pmv;
  }
  else
  {
    int b = getBestLine(k, mk);                   /* row or column with most
                                                     zeros */
    poly result = NULL;                           /* This will contain the
                                                     value of the minor. */
    int s = 0; int m = 0; int as = 0; int am = 0; /* counters for additions
                                                     and multiplications,
                                                     ..."a*" for accumulated
                                                     operation counters */
    bool hadNonZeroEntry = false;
    if (b >= 0)
    {
      /* This means that the best line is the row with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in
         minorRowKey: */
      int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
      for (int c = 0; c < k; c++) /* This iterates over all involved columns. */
      {
        int absoluteC = mk.getAbsoluteColumnIndex(c);
        if (!isEntryZero(b, absoluteC)) /* Only then do we have to consider
                                           this sub-determinante. */
        {
          hadNonZeroEntry = true;
          PolyMinorValue mv; /* for storing all intermediate minors */
          /* Next MinorKey is mk with row b and column absoluteC omitted. */
          MinorKey subMk = mk.getSubMinorKey(b, absoluteC);
          if (cch.hasKey(subMk))
          { /* trying to find the result in the cache */
            mv = cch.getValue(subMk);
            mv.incrementRetrievals(); /* once more, we made use of the cached
                                         value for key mk */
            cch.put(subMk, mv); /* We need to do this with "put", as the
                                   (altered) number of retrievals may have an
                                   impact on the internal ordering among cache
                                   entries. */
          }
          else
          {
            /* recursive call: */
            mv = getMinorPrivateLaplace(k - 1, subMk, multipleMinors, cch,
                                        iSB);
            /* As this minor was not in the cache, we count the additions and
               multiplications that we needed to do in the recursive call: */
            m += mv.getMultiplications();
            s += mv.getAdditions();
          }
          /* In any case, we count all nested operations in the accumulative
             counters: */
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          poly signPoly = pISet(sign);
          poly temp = pp_Mult_qq(mv.getResult(), getEntry(b, absoluteC),
                                 currRing);
          temp = p_Mult_q(signPoly, temp, currRing);
          result = p_Add_q(result, temp, currRing);
#ifdef COUNT_AND_PRINT_OPERATIONS
          multsPoly++;
          addsPoly++;
          multsMon += pLength(mv.getResult()) * pLength(getEntry(b, absoluteC));
#endif
          s++; m++; as++; am++; /* This is for the addition and multiplication
                                   in the previous lines of code. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    else
    {
      b = - b - 1;
      /* This means that the best line is the column with absolute (0-based)
         index b.
         Using Laplace, the sign of the contributing minors must be iterating;
         the initial sign depends on the relative index of b in
         minorColumnKey: */
      int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
      for (int r = 0; r < k; r++) /* This iterates over all involved rows. */
      {
        int absoluteR = mk.getAbsoluteRowIndex(r);
        if (!isEntryZero(absoluteR, b)) /* Only then do we have to consider
                                           this sub-determinante. */
        {
          hadNonZeroEntry = true;
          PolyMinorValue mv; /* for storing all intermediate minors */
          /* Next MinorKey is mk with row absoluteR and column b omitted. */
          MinorKey subMk = mk.getSubMinorKey(absoluteR, b);
          if (cch.hasKey(subMk))
          { /* trying to find the result in the cache */
            mv = cch.getValue(subMk);
            mv.incrementRetrievals(); /* once more, we made use of the cached
                                         value for key mk */
            cch.put(subMk, mv); /* We need to do this with "put", as the
                                   (altered) number of retrievals may have an
                                   impact on the internal ordering among the
                                   cached entries. */
          }
          else
          {
            mv = getMinorPrivateLaplace(k - 1, subMk, multipleMinors, cch,
                                        iSB); /* recursive call */
            /* As this minor was not in the cache, we count the additions and
               multiplications that we needed to do in the recursive call: */
            m += mv.getMultiplications();
            s += mv.getAdditions();
          }
          /* In any case, we count all nested operations in the accumulative
             counters: */
          am += mv.getAccumulatedMultiplications();
          as += mv.getAccumulatedAdditions();
          poly signPoly = pISet(sign);
          poly temp = pp_Mult_qq(mv.getResult(), getEntry(absoluteR, b),
                                 currRing);
          temp = p_Mult_q(signPoly, temp, currRing);
          result = p_Add_q(result, temp, currRing);
#ifdef COUNT_AND_PRINT_OPERATIONS
          multsPoly++;
          addsPoly++;
          multsMon += pLength(mv.getResult()) * pLength(getEntry(absoluteR, b));
#endif
          s++; m++; as++; am++; /* This is for the addition and multiplication
                                   in the previous lines of code. */
        }
        sign = - sign; /* alternating the sign */
      }
    }
    /* Let's cache the newly computed minor: */
    int potentialRetrievals = NumberOfRetrievals(_containerRows,
                                                 _containerColumns,
                                                 _minorSize,
                                                 k,
                                                 multipleMinors);
    if (hadNonZeroEntry)
    {
      s--; as--; /* first addition was 0 + ..., so we do not count it */
#ifdef COUNT_AND_PRINT_OPERATIONS
      addsPoly--;
#endif
    }
    if (s < 0) s = 0; /* may happen when all subminors are zero and no
                         addition needs to be performed */
    if (as < 0) as = 0; /* may happen when all subminors are zero and no
                           addition needs to be performed */
    if (iSB != NULL)
    {
      poly tmpresult = kNF(iSB, currRing->qideal, result);
      pDelete(&tmpresult);
      result=tmpresult;
    }
    PolyMinorValue newMV(result, m, s, am, as, 1, potentialRetrievals);
    pDelete(&result); result = NULL;
    cch.put(mk, newMV); /* Here's the actual put inside the cache. */
    return newMV;
  }
}

/* This can only be used in the case of coefficients coming from a field
   or at least an integral domain. */
static void addOperationBucket(poly f1, poly f2, kBucket_pt bucket)
{
  /* fills all terms of f1 * f2 into the bucket */
  poly a = f1; poly b = f2;
  int aLen = pLength(a); int bLen = pLength(b);
  if (aLen > bLen)
  {
    b = f1; a = f2; bLen = aLen;
  }
  pNormalize(b);

  while (a != NULL)
  {
    /* The next line actually uses only LT(a): */
    kBucket_Plus_mm_Mult_pp(bucket, a, b, bLen);
    a = pNext(a);
  }
}

/* computes the polynomial (p1 * p2 - p3 * p4) and puts result into p1;
   the method destroys the old value of p1;
   p2, p3, and p4 may be pNormalize-d but must, apart from that,
   not be changed;
   This can only be used in the case of coefficients coming from a field
   or at least an integral domain. */
static void elimOperationBucketNoDiv(poly &p1, poly p2, poly p3, poly p4)
{
#ifdef COUNT_AND_PRINT_OPERATIONS
  if ((pLength(p1) != 0) && (pLength(p2) != 0))
  {
    multsPoly++;
    multsMon += pLength(p1) * pLength(p2);
  }
  if ((pLength(p3) != 0) && (pLength(p4) != 0))
  {
    multsPoly++;
    multsMon += pLength(p3) * pLength(p4);
  }
  if ((pLength(p1) != 0) && (pLength(p2) != 0) &&
      (pLength(p3) != 0) && (pLength(p4) != 0))
    addsPoly++;
#endif
  kBucket_pt myBucket = kBucketCreate(currRing);
  addOperationBucket(p1, p2, myBucket);
  poly p3Neg = pNeg(pCopy(p3));
  addOperationBucket(p3Neg, p4, myBucket);
  pDelete(&p3Neg);
  pDelete(&p1);
  p1 = kBucketClear(myBucket);
  kBucketDestroy(&myBucket);
}

/* computes the polynomial (p1 * p2 - p3 * p4) / p5 and puts result into p1;
   the method destroys the old value of p1;
   p2, p3, p4, and p5 may be pNormalize-d but must, apart from that,
   not be changed;
   c5 is assumed to be the leading coefficient of p5;
   p5Len is assumed to be the length of p5;
   This can only be used in the case of coefficients coming from a field
   or at least an integral domain. */
void elimOperationBucket(poly &p1, poly &p2, poly &p3, poly &p4, poly &p5,
                         number &c5, int p5Len)
{
#ifdef COUNT_AND_PRINT_OPERATIONS
  if ((pLength(p1) != 0) && (pLength(p2) != 0))
  {
    multsPoly++;
    multsMon += pLength(p1) * pLength(p2);
  }
  if ((pLength(p3) != 0) && (pLength(p4) != 0))
  {
    multsPoly++;
    multsMon += pLength(p3) * pLength(p4);
  }
  if ((pLength(p1) != 0) && (pLength(p2) != 0) &&
      (pLength(p3) != 0) && (pLength(p4) != 0))
    addsPoly++;
#endif
  kBucket_pt myBucket = kBucketCreate(currRing);
  addOperationBucket(p1, p2, myBucket);
  poly p3Neg = pNeg(pCopy(p3));
  addOperationBucket(p3Neg, p4, myBucket);
  pDelete(&p3Neg);

  /* Now, myBucket contains all terms of p1 * p2 - p3 * p4.
     Now we need to perform the polynomial division myBucket / p5
     which is known to work without remainder: */
  pDelete(&p1); poly helperPoly = NULL;

  poly bucketLm = pCopy(kBucketGetLm(myBucket));
  while (bucketLm != NULL)
  {
    /* divide bucketLm by the leading term of p5 and put result into bucketLm;
       we start with the coefficients;
       note that bucketLm will always represent a term */
    number coeff = nDiv(pGetCoeff(bucketLm), c5);
    nNormalize(coeff);
    pSetCoeff(bucketLm, coeff);
    /* subtract exponent vector of p5 from that of quotient; modifies
       quotient */
    p_ExpVectorSub(bucketLm, p5, currRing);
#ifdef COUNT_AND_PRINT_OPERATIONS
    divsMon++;
    multsMonForDiv += p5Len;
    multsMon += p5Len;
    savedMultsMFD++;
    multsPoly++;
    multsPolyForDiv++;
    addsPoly++;
    addsPolyForDiv++;
#endif
    kBucket_Minus_m_Mult_p(myBucket, bucketLm, p5, &p5Len);
    /* The following lines make bucketLm the new leading term of p1,
       i.e., put bucketLm in front of everything which is already in p1.
       Thus, after the while loop, we need to revert p1. */
    helperPoly = bucketLm;
    helperPoly->next = p1;
    p1 = helperPoly;

    bucketLm = pCopy(kBucketGetLm(myBucket));
  }
  p1 = pReverse(p1);
  kBucketDestroy(&myBucket);
}

/* assumes that all entries in polyMatrix are already reduced w.r.t. iSB
   This can only be used in the case of coefficients coming from a field!!! */
PolyMinorValue PolyMinorProcessor::getMinorPrivateBareiss(const int k,
                                                          const MinorKey& mk,
                                                          const ideal& iSB)
{
  assume(k > 0); /* k is the minor's dimension; the minor must be at least
                    1x1 */
  int *theRows=(int*)omAlloc(k*sizeof(int));
  mk.getAbsoluteRowIndices(theRows);
  int *theColumns=(int*)omAlloc(k*sizeof(int));
  mk.getAbsoluteColumnIndices(theColumns);
  if (k == 1)
  {
    PolyMinorValue tmp=PolyMinorValue(getEntry(theRows[0], theColumns[0]),
                          0, 0, 0, 0, -1, -1);
    omFree(theColumns);
    omFree(theRows);
    return tmp;
  }
  else /* k > 0 */
  {
    /* the matrix to perform Bareiss with */
    poly* tempMatrix = (poly*)omAlloc(k * k * sizeof(poly));
    /* copy correct set of entries from _polyMatrix to tempMatrix */
    int i = 0;
    for (int r = 0; r < k; r++)
      for (int c = 0; c < k; c++)
        tempMatrix[i++] = pCopy(getEntry(theRows[r], theColumns[c]));

    /* Bareiss algorithm operating on tempMatrix which is at least 2x2 */
    int sign = 1; /* This will store the correct sign resulting from
                     permuting the rows of tempMatrix. */
    int *rowPermutation=(int*)omAlloc(k*sizeof(int));
                 /* This is for storing the permutation of rows
                                resulting from searching for a non-zero pivot
                                element. */
    for (int i = 0; i < k; i++) rowPermutation[i] = i;
    poly divisor = NULL;
    int divisorLength = 0;
    number divisorLC;
    for (int r = 0; r <= k - 2; r++)
    {
      /* look for a non-zero entry in column r, rows = r .. (k - 1)
         s.t. the polynomial has least complexity: */
      int minComplexity = -1; int complexity = 0; int bestRow = -1;
      poly pp = NULL;
      for (int i = r; i < k; i++)
      {
        pp = tempMatrix[rowPermutation[i] * k + r];
        if (pp != NULL)
        {
          if (minComplexity == -1)
          {
            minComplexity = pSize(pp);
            bestRow = i;
          }
          else
          {
            complexity = 0;
            while ((pp != NULL) && (complexity < minComplexity))
            {
              complexity += nSize(pGetCoeff(pp)); pp = pNext(pp);
            }
            if (complexity < minComplexity)
            {
              minComplexity = complexity;
              bestRow = i;
            }
          }
          if (minComplexity <= 1) break; /* terminate the search */
        }
      }
      if (bestRow == -1)
      {
        /* There is no non-zero entry; hence the minor is zero. */
        for (int i = 0; i < k * k; i++) pDelete(&tempMatrix[i]);
        return PolyMinorValue(NULL, 0, 0, 0, 0, -1, -1);
      }
      pNormalize(tempMatrix[rowPermutation[bestRow] * k + r]);
      if (bestRow != r)
      {
        /* We swap the rows with indices r and i: */
        int j = rowPermutation[bestRow];
        rowPermutation[bestRow] = rowPermutation[r];
        rowPermutation[r] = j;
        /* Now we know that tempMatrix[rowPermutation[r] * k + r] is not zero.
           But careful; we have to negate the sign, as there is always an odd
           number of row transpositions to swap two given rows of a matrix. */
        sign = -sign;
      }
#if (defined COUNT_AND_PRINT_OPERATIONS) && (COUNT_AND_PRINT_OPERATIONS > 2)
      poly w = NULL; int wl = 0;
      printf("matrix after %d steps:\n", r);
      for (int u = 0; u < k; u++)
      {
        for (int v = 0; v < k; v++)
        {
          if ((v < r) && (u > v))
            wl = 0;
          else
          {
            w = tempMatrix[rowPermutation[u] * k + v];
            wl = pLength(w);
          }
          printf("%5d  ", wl);
        }
        printf("\n");
      }
      printCounters ("", false);
#endif
      if (r != 0)
      {
        divisor = tempMatrix[rowPermutation[r - 1] * k + r - 1];
        pNormalize(divisor);
        divisorLength = pLength(divisor);
        divisorLC = pGetCoeff(divisor);
      }
      for (int rr = r + 1; rr < k; rr++)
        for (int cc = r + 1; cc < k; cc++)
        {
          if (r == 0)
            elimOperationBucketNoDiv(tempMatrix[rowPermutation[rr] * k + cc],
                                     tempMatrix[rowPermutation[r]  * k + r],
                                     tempMatrix[rowPermutation[r]  * k + cc],
                                     tempMatrix[rowPermutation[rr] * k + r]);
          else
            elimOperationBucket(tempMatrix[rowPermutation[rr] * k + cc],
                                tempMatrix[rowPermutation[r]  * k + r],
                                tempMatrix[rowPermutation[r]  * k + cc],
                                tempMatrix[rowPermutation[rr] * k + r],
                                divisor, divisorLC, divisorLength);
        }
    }
#if (defined COUNT_AND_PRINT_OPERATIONS) && (COUNT_AND_PRINT_OPERATIONS > 2)
    poly w = NULL; int wl = 0;
    printf("matrix after %d steps:\n", k - 1);
    for (int u = 0; u < k; u++)
    {
      for (int v = 0; v < k; v++)
      {
        if ((v < k - 1) && (u > v))
          wl = 0;
        else
        {
          w = tempMatrix[rowPermutation[u] * k + v];
          wl = pLength(w);
        }
        printf("%5d  ", wl);
      }
      printf("\n");
    }
#endif
    poly result = tempMatrix[rowPermutation[k - 1] * k + k - 1];
    tempMatrix[rowPermutation[k - 1] * k + k - 1]=NULL; /*value now in result*/
    if (sign == -1) result = pNeg(result);
    if (iSB != NULL)
    {
      poly tmpresult = kNF(iSB, currRing->qideal, result);
      pDelete(&result);
      result=tmpresult;
    }
    PolyMinorValue mv(result, 0, 0, 0, 0, -1, -1);
    for (int i = 0; i < k * k; i++) pDelete(&tempMatrix[i]);
    omFreeSize(tempMatrix, k * k * sizeof(poly));
    omFree(rowPermutation);
    omFree(theColumns);
    omFree(theRows);
    return mv;
  }
}

