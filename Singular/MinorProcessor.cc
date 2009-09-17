#include "mod2.h"
#include "structs.h"
#include "polys.h"
#include <MinorProcessor.h>

void MinorProcessor::print() const {
    cout << this->toString();
}

int MinorProcessor::getBestLine (const int k, const MinorKey& mk) const {
    // This method identifies the row or column with the most zeros.
    // The returned index (bestIndex) is absolute within the pre-defined matrix.
    // If some row has the most zeros, then the absolute (0-based) row index is returned.
    // If, contrariwise, some column has the most zeros, then -1 minus the absolute (0-based) column index is returned.
    int numberOfZeros = 0;
    int bestIndex = 100000;    // We start with an invalid row/column index.
    int maxNumberOfZeros = -1; // We update this variable whenever we find a new so-far optimal row or column.
    for (int r = 0; r < k; r++) {
        // iterate through all k rows of the momentary minor
        int absoluteR = mk.getAbsoluteRowIndex(r);
        numberOfZeros = 0;
        for (int c = 0; c < k; c++) {
            int absoluteC = mk.getAbsoluteColumnIndex(c);
            if (isEntryZero(absoluteR, absoluteC)) numberOfZeros++;
        }
        if (numberOfZeros > maxNumberOfZeros) {
            // We found a new best line which is a row.
            bestIndex = absoluteR;
            maxNumberOfZeros = numberOfZeros;
        }
    };
    for (int c = 0; c < k; c++) {
        int absoluteC = mk.getAbsoluteColumnIndex(c);
        numberOfZeros = 0;
        for (int r = 0; r < k; r++) {
            int absoluteR = mk.getAbsoluteRowIndex(r);
            if (isEntryZero(absoluteR, absoluteC)) numberOfZeros++;
        }
        if (numberOfZeros > maxNumberOfZeros) {
            // We found a new best line which is a column. So we transform the return value.
            // Note that we can easily get back absoluteC from bestLine: absoluteC = - 1 - bestLine.
            bestIndex = - absoluteC - 1;
            maxNumberOfZeros = numberOfZeros;
        }
    };
    return bestIndex;
}

void MinorProcessor::setMinorSize(const int minorSize) {
    _minorSize = minorSize;
    _minor.reset();
}

bool MinorProcessor::hasNextMinor() {
    return setNextKeys(_minorSize);
}

void MinorProcessor::getCurrentRowIndices(int* const target) const {
    return _minor.getAbsoluteRowIndices(target);
}

void MinorProcessor::getCurrentColumnIndices(int* const target) const {
    return _minor.getAbsoluteColumnIndices(target);
}

void MinorProcessor::defineSubMatrix(const int numberOfRows, const int* rowIndices,
                                     const int numberOfColumns, const int* columnIndices) {
    // The method assumes ascending row and column indices in the two argument arrays.
    // These indices are understood to be zero-based.
    // The method will set the two arrays of unsigned longs in _container.
    // Example: The indices 0, 2, 3, 7 will be converted to an array with one unsigned long
    // representing the binary number 10001101 (check bits from right to left).

    _containerRows = numberOfRows;
    int highestRowIndex = rowIndices[numberOfRows - 1];
    int rowBlockCount = (highestRowIndex / 32) + 1;
    unsigned long rowBlocks[rowBlockCount];
    for (int i = 0; i < rowBlockCount; i++) rowBlocks[i] = 0;
    for (int i = 0; i < numberOfRows; i++) {
        int blockIndex = rowIndices[i] / 32;
        int offset = rowIndices[i] % 32;
        rowBlocks[blockIndex] += (1 << offset);
    }

    _containerColumns = numberOfColumns;
    int highestColumnIndex = columnIndices[numberOfColumns - 1];
    int columnBlockCount = (highestColumnIndex / 32) + 1;
    unsigned long columnBlocks[columnBlockCount];
    for (int i = 0; i < columnBlockCount; i++) columnBlocks[i] = 0;
    for (int i = 0; i < numberOfColumns; i++) {
        int blockIndex = columnIndices[i] / 32;
        int offset = columnIndices[i] % 32;
        columnBlocks[blockIndex] += (1 << offset);
    }

    _container.set(rowBlockCount, rowBlocks, columnBlockCount, columnBlocks);
}

bool MinorProcessor::setNextKeys(const int k) {
    // This method moves _minor to the next valid kxk-minor within _container.
    // It returns true iff this is successful, i.e. iff _minor did not already encode the final kxk-minor.
    if (_minor.compare(MinorKey(0, 0, 0, 0)) == 0) {
        // This means that we haven't started yet. Thus, we are about to compute the first kxk-minor.
        _minor.selectFirstRows(k, _container);
        _minor.selectFirstColumns(k, _container);
        return true;
    }
    else if (_minor.selectNextColumns(k, _container)) {
        // Here we were able to pick a next subset of columns (within the same subset of rows).
        return true;
    }
    else if (_minor.selectNextRows(k, _container)) {
        // Here we were not able to pick a next subset of columns (within the same subset of rows).
        // But we could pick a next subset of rows. We must hence reset the subset of columns:
        _minor.selectFirstColumns(k, _container);
        return true;
    }
    else {
        // We were neither able to pick a next subset of columns nor of rows.
        // I.e., we have iterated through all sensible choices of subsets of rows and columns.
        return false;
    }
}

bool MinorProcessor::isEntryZero (const int absoluteRowIndex, const int absoluteColumnIndex) const
{
  assume(false);
}

string MinorProcessor::toString () const
{
  assume(false);
}

int MinorProcessor::IOverJ(const int i, const int j) {
    // This is a non-recursive implementation.
    assert(i >= 0 && j >= 0 && i >= j);
    if (j == 0 || i == j) return 1;
    int result = 1;
    for (int k = i - j + 1; k <= i; k++) result *= k;
    // Here, we have result = (i - j + 1) * ... * i.
    for (int k = 2; k <= j; k++) result /= k;
    // Here, we have result = (i - j + 1) * ... * i / 1 / 2 ... / j = i! / j! / (i - j)!.
    return result;
}

int MinorProcessor::Faculty(const int i) {
    // This is a non-recursive implementation.
    assert(i >= 0);
    int result = 1;
    for (int j = 1; j <= i; j++) result *= j;
    // Here, we have result = 1 * 2 * ... * i = i!
    return result;
}

int MinorProcessor::NumberOfRetrievals (const int rows, const int columns, const int containerMinorSize,
                        const int minorSize, const bool multipleMinors) {
    // This method computes the number of potential retrievals of a single minor when computing
    // all minors of a given size within a matrix of given size.
    int result = 0;
    if (multipleMinors) {
        // Here, we would like to compute all minors of size
        // containerMinorSize x containerMinorSize in a matrix of size rows x columns.
        // Then, we need to retrieve any minor of size minorSize x minorSize exactly
        // n times, where n is as follows:
        result = IOverJ(rows - minorSize, containerMinorSize - minorSize)
               * IOverJ(columns - minorSize, containerMinorSize - minorSize)
               * Faculty(containerMinorSize - minorSize);
    }
    else {
        // Here, we would like to compute just one minor of size
        // containerMinorSize x containerMinorSize. Then, we need to retrieve
        // any minor of size minorSize x minorSize exactly
        // (containerMinorSize - minorSize)! times:
        result = Faculty(containerMinorSize - minorSize);
    }
    return result;
}

LongMinorProcessor::LongMinorProcessor() {
    _container = MinorKey(0, 0, 0, 0);
    _minor = MinorKey(0, 0, 0, 0);
    _containerRows = 0;
    _containerColumns = 0;
    _minorSize = 0;
    _matrix = 0;
    _rows = 0;
    _columns = 0;
}

string LongMinorProcessor::toString () const {
    char h[32];
    string t = "";
    string s = "LongMinorProcessor:";
    s += "\n   matrix: ";
    sprintf(h, "%d", _rows); s += h;
    s += " x ";
    sprintf(h, "%d", _columns); s += h;
    for (int r = 0; r < _rows; r++) {
        s += "\n      ";
        for (int c = 0; c < _columns; c++) {
            sprintf(h, "%d", _matrix[r][c]); t = h;
            for (int k = 0; k < int(4 - strlen(h)); k++) s += " ";
            s += t;
        }
    }
    int myIndexArray[500];
    s += "\n   considered submatrix has row indices: ";
    _container.getAbsoluteRowIndices(myIndexArray);
    for (int k = 0; k < _containerRows; k++) {
        if (k != 0) s += ", ";
        sprintf(h, "%d", myIndexArray[k]); s += h;
    }
    s += " (first row of matrix has index 0)";
    s += "\n   considered submatrix has column indices: ";
    _container.getAbsoluteColumnIndices(myIndexArray);
    for (int k = 0; k < _containerColumns; k++) {
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

bool LongMinorProcessor::isEntryZero (const int absoluteRowIndex, const int absoluteColumnIndex) const
{
  return _matrix[absoluteRowIndex][absoluteColumnIndex] == 0;
}

LongMinorProcessor::~LongMinorProcessor() {
    // free memory of _matrix
    for (int i = 0; i < _rows; i++) {
        delete [] _matrix[i]; _matrix[i] = 0;
    }
    delete [] _matrix; _matrix = 0;
}

void LongMinorProcessor::defineMatrix (const int numberOfRows, const int numberOfColumns, const int* matrix) {
    // free memory of _matrix
    for (int i = 0; i < _rows; i++) {
        delete [] _matrix[i]; _matrix[i] = 0;
    }
    delete [] _matrix; _matrix = 0;

    _rows = numberOfRows;
    _columns = numberOfColumns;

    // allocate memory for new entries in _matrix
    _matrix = new int*[_rows];
    for (int i = 0; i < _rows; i++) _matrix[i] = new int[_columns];

    // copying values from one-dimensional method parameter "matrix"
    for (int r = 0; r < _rows; r++)
        for (int c = 0; c < _columns; c++)
            _matrix[r][c] = matrix[r * _columns + c];
}

LongMinorValue LongMinorProcessor::getMinor(const int dimension, const int* rowIndices, const int* columnIndices,
                                            Cache<MinorKey, LongMinorValue>& c) {
    defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
    _minorSize = dimension;
    // call a helper method which recursively computes the minor using the cache c:
    return getMinorPrivate(dimension, _container, false, c);
}

LongMinorValue LongMinorProcessor::getMinor(const int dimension, const int* rowIndices, const int* columnIndices) {
    defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
    _minorSize = dimension;
    // call a helper method which recursively computes the minor (without using a cache):
    return getMinorPrivate(_minorSize, _container);
}

LongMinorValue LongMinorProcessor::getNextMinor() {
    // computation without cache
    return getMinorPrivate(_minorSize, _minor);
}

LongMinorValue LongMinorProcessor::getNextMinor(Cache<MinorKey, LongMinorValue>& c) {
    // computation with cache
    return getMinorPrivate(_minorSize, _minor, true, c);
}

LongMinorValue LongMinorProcessor::getMinorPrivate(const int k, const MinorKey& mk) {
    assert(k > 0); // k is the minor's dimension; the minor must be at least 1x1
    // The method works by recursion, and using Lapace's Theorem along the row/column with the most zeros.
    if (k == 1) {
        return LongMinorValue(_matrix[mk.getAbsoluteRowIndex(0)][mk.getAbsoluteColumnIndex(0)],
                              0, 0, 0, 0, -1, -1); // "-1" is to signal that any statistics about the
                                                   // number of retrievals does not make sense, as we do not use a cache.
    }
    else {
        // Here, the minor must be 2x2 or larger.
        int b = getBestLine(k, mk);                          // row or column with most zeros
        int result = 0;                                      // This will contain the value of the minor.
        int s = 0; int m = 0; int as = 0; int am = 0;        // counters for summations and multiplications,
                                                             // ..."a*" for accumulated operation counters
        if (b >= 0) {
            // This means that the best line is the row with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorRowKey:
            int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
            for (int c = 0; c < k; c++) {
                int absoluteC = mk.getAbsoluteColumnIndex(c);      // This iterates over all involved columns.
                MinorKey subMk = mk.getSubMinorKey(b, absoluteC);  // This is MinorKey when we omit row b and column absoluteC.
                if (_matrix[b][absoluteC] != 0) { // Only then do we have to consider this sub-determinante.
                    LongMinorValue mv = getMinorPrivate(k - 1, subMk);  // recursive call
                    m += mv.getMultiplications();
                    s += mv.getSummations();
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result += sign * mv.getResult() * _matrix[b][absoluteC]; // adding sub-determinante times matrix entry
                                                                             // times appropriate sign
                    s++; m++; as++, am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
            }
        }
        else {
            b = - b - 1;
            // This means that the best line is the column with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorColumnKey:
            int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
            for (int r = 0; r < k; r++) {
                int absoluteR = mk.getAbsoluteRowIndex(r);        // This iterates over all involved rows.
                MinorKey subMk = mk.getSubMinorKey(absoluteR, b); // This is MinorKey when we omit row absoluteR and column b.
                if (_matrix[absoluteR][b] != 0) { // Only then do we have to consider this sub-determinante.
                    LongMinorValue mv = getMinorPrivate(k - 1, subMk);  // recursive call
                    m += mv.getMultiplications();
                    s += mv.getSummations();
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result += sign * mv.getResult() * _matrix[absoluteR][b]; // adding sub-determinante times matrix entry
                                                                             // times appropriate sign
                    s++; m++; as++, am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
            }
        }
        s--; as--; // first summation was 0 + ..., so we do not count it
        if (s < 0) s = 0; // may happen when all subminors are zero and no summation needs to be performed
        if (as < 0) as = 0; // may happen when all subminors are zero and no summation needs to be performed
        LongMinorValue newMV = LongMinorValue(result, m, s, am, as, -1, -1); // "-1" is to signal that any statistics about the
                                                                             // number of retrievals does not make sense, as we
                                                                             // do not use a cache.
        return newMV;
    }
}

LongMinorValue LongMinorProcessor::getMinorPrivate(const int k, const MinorKey& mk,
                                                   const bool multipleMinors,
                                                   Cache<MinorKey, LongMinorValue>& cch) {
    assert(k > 0); // k is the minor's dimension; the minor must be at least 1x1
    // The method works by recursion, and using Lapace's Theorem along the row/column with the most zeros.
    if (k == 1) {
        return LongMinorValue(_matrix[mk.getAbsoluteRowIndex(0)][mk.getAbsoluteColumnIndex(0)],
                              0, 0, 0, 0, -1, -1); // we set "-1" as, for k == 1, we do not have any cache retrievals
    }
    else {
        int b = getBestLine(k, mk);                          // row or column with most zeros
        int result = 0;                                      // This will contain the value of the minor.
        int s = 0; int m = 0; int as = 0; int am = 0;        // counters for summations and multiplications,
                                                             // ..."a*" for accumulated operation counters
        LongMinorValue mv = LongMinorValue(0, 0, 0, 0, 0, 0, 0);     // for storing all intermediate minors
        if (b >= 0) {
            // This means that the best line is the row with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorRowKey:
            int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
            for (int c = 0; c < k; c++) {
                int absoluteC = mk.getAbsoluteColumnIndex(c);      // This iterates over all involved columns.
                MinorKey subMk = mk.getSubMinorKey(b, absoluteC);  // This is MinorKey when we omit row b and column absoluteC.
                if (_matrix[b][absoluteC] != 0) { // Only then do we have to consider this sub-determinante.
                    if (cch.hasKey(subMk)) { // trying to find the result in the cache
                        mv = cch.getValue(subMk);
                        mv.incrementRetrievals(); // once more, we made use of the cached value for key mk
                        cch.put(subMk, mv); // We need to do this with "put", as the (altered) number of retrievals may have
                                            // an impact on the internal ordering among cache entries.
                    }
                    else {
                        mv = getMinorPrivate(k - 1, subMk, multipleMinors, cch); // recursive call
                        // As this minor was not in the cache, we count the summations and
                        // multiplications that we needed to do in the recursive call:
                        m += mv.getMultiplications();
                        s += mv.getSummations();
                    }
                    // In any case, we count all nested operations in the accumulative counters:
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result += sign * mv.getResult() * _matrix[b][absoluteC]; // adding sub-determinante times matrix entry
                                                                             // times appropriate sign
                    s++; m++; as++; am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
            }
        }
        else {
            b = - b - 1;
            // This means that the best line is the column with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorColumnKey:
            int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
            for (int r = 0; r < k; r++) {
                int absoluteR = mk.getAbsoluteRowIndex(r);        // This iterates over all involved rows.
                MinorKey subMk = mk.getSubMinorKey(absoluteR, b); // This is MinorKey when we omit row absoluteR and column b.
                if (_matrix[absoluteR][b] != 0) { // Only then do we have to consider this sub-determinante.
                    if (cch.hasKey(subMk)) { // trying to find the result in the cache
                        mv = cch.getValue(subMk);
                        mv.incrementRetrievals(); // once more, we made use of the cached value for key mk
                        cch.put(subMk, mv); // We need to do this with "put", as the (altered) number of retrievals may have
                                            // an impact on the internal ordering among cache entries.
                    }
                    else {
                        mv = getMinorPrivate(k - 1, subMk, multipleMinors, cch); // recursive call
                        // As this minor was not in the cache, we count the summations and
                        // multiplications that we needed to do in the recursive call:
                        m += mv.getMultiplications();
                        s += mv.getSummations();
                    }
                    // In any case, we count all nested operations in the accumulative counters:
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result += sign * mv.getResult() * _matrix[absoluteR][b]; // adding sub-determinante times matrix entry
                                                                             // times appropriate sign
                    s++; m++; as++; am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
            }
        }
        // Let's cache the newly computed minor:
        int potentialRetrievals = NumberOfRetrievals(_containerRows, _containerColumns, _minorSize, k, multipleMinors);
        s--; as--; // first summation was 0 + ..., so we do not count it
        if (s < 0) s = 0; // may happen when all subminors are zero and no summation needs to be performed
        if (as < 0) as = 0; // may happen when all subminors are zero and no summation needs to be performed
        LongMinorValue newMV = LongMinorValue(result, m, s, am, as, 1, potentialRetrievals);
        cch.put(mk, newMV); // Here's the actual put inside the cache.
        return newMV;
    }
}

PolyMinorProcessor::PolyMinorProcessor() {
    _container = MinorKey(0, 0, 0, 0);
    _minor = MinorKey(0, 0, 0, 0);
    _containerRows = 0;
    _containerColumns = 0;
    _minorSize = 0;
    _polyMatrix = 0;
    _rows = 0;
    _columns = 0;
}

string PolyMinorProcessor::toString () const {
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
    for (int k = 0; k < _containerRows; k++) {
        if (k != 0) s += ", ";
        sprintf(h, "%d", myIndexArray[k]); s += h;
    }
    s += " (first row of matrix has index 0)";
    s += "\n   considered submatrix has column indices: ";
    _container.getAbsoluteColumnIndices(myIndexArray);
    for (int k = 0; k < _containerColumns; k++) {
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

bool PolyMinorProcessor::isEntryZero (const int absoluteRowIndex, const int absoluteColumnIndex) const
{
  poly zeroPoly = pISet(0);
  return pEqualPolys(_polyMatrix[absoluteRowIndex][absoluteColumnIndex], zeroPoly);
}

PolyMinorProcessor::~PolyMinorProcessor() {
    // free memory of _polyMatrix
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            p_Delete(&_polyMatrix[i][j], currRing);
        }
        delete [] _polyMatrix[i]; _polyMatrix[i] = 0;
    }
    delete [] _polyMatrix; _polyMatrix = 0;
}

void PolyMinorProcessor::defineMatrix (const int numberOfRows, const int numberOfColumns, const poly* polyMatrix) {
    // free memory of _polyMatrix
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++)
            p_Delete(&_polyMatrix[i][j], currRing);
        delete [] _polyMatrix[i]; _polyMatrix[i] = 0;
    }
    delete [] _polyMatrix; _polyMatrix = 0;

    _rows = numberOfRows;
    _columns = numberOfColumns;

    // allocate memory for new entries in _matrix
    _polyMatrix = new poly*[_rows];
    for (int i = 0; i < _rows; i++) _polyMatrix[i] = new poly[_columns];

    // copying values from one-dimensional method parameter "matrix"
    for (int r = 0; r < _rows; r++)
        for (int c = 0; c < _columns; c++)
            _polyMatrix[r][c] = pCopy(polyMatrix[r * _columns + c]);
}

PolyMinorValue PolyMinorProcessor::getMinor(const int dimension, const int* rowIndices, const int* columnIndices,
                                            Cache<MinorKey, PolyMinorValue>& c) {
    defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
    _minorSize = dimension;
    // call a helper method which recursively computes the minor using the cache c:
    return getMinorPrivate(dimension, _container, false, c);
}

PolyMinorValue PolyMinorProcessor::getMinor(const int dimension, const int* rowIndices, const int* columnIndices) {
    defineSubMatrix(dimension, rowIndices, dimension, columnIndices);
    _minorSize = dimension;
    // call a helper method which recursively computes the minor (without using a cache):
    return getMinorPrivate(_minorSize, _container);
}

PolyMinorValue PolyMinorProcessor::getNextMinor() {
    // computation without cache
    return getMinorPrivate(_minorSize, _minor);
}

PolyMinorValue PolyMinorProcessor::getNextMinor(Cache<MinorKey, PolyMinorValue>& c) {
    // computation with cache
    return getMinorPrivate(_minorSize, _minor, true, c);
}

// performs the assignment a = a + (b * c * d)
// c and d must neither be destroyed nor modified
// b may be destroyed
// a must finally contain the new value
poly ops(poly a, poly b, poly c, poly d)
{
  return p_Add_q(a, p_Mult_q(b, pp_Mult_qq(c, d, currRing), currRing), currRing);
}

PolyMinorValue PolyMinorProcessor::getMinorPrivate(const int k, const MinorKey& mk) {
    assert(k > 0); // k is the minor's dimension; the minor must be at least 1x1
    // The method works by recursion, and using Lapace's Theorem along the row/column with the most zeros.
    if (k == 1) {
        return PolyMinorValue(_polyMatrix[mk.getAbsoluteRowIndex(0)][mk.getAbsoluteColumnIndex(0)],
                              0, 0, 0, 0, -1, -1); // "-1" is to signal that any statistics about the
                                                   // number of retrievals does not make sense, as we do not use a cache.
    }
    else {
        // Here, the minor must be 2x2 or larger.
        int b = getBestLine(k, mk);                          // row or column with most zeros
        poly result = pISet(0);                              // This will contain the value of the minor.
        int s = 0; int m = 0; int as = 0; int am = 0;        // counters for summations and multiplications,
                                                             // ..."a*" for accumulated operation counters
        if (b >= 0) {
            // This means that the best line is the row with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorRowKey:
            int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
            poly signPoly = pISet(sign);
            for (int c = 0; c < k; c++) {
                int absoluteC = mk.getAbsoluteColumnIndex(c);      // This iterates over all involved columns.
                MinorKey subMk = mk.getSubMinorKey(b, absoluteC);  // This is MinorKey when we omit row b and column absoluteC.
                if (!isEntryZero(b, absoluteC)) { // Only then do we have to consider this sub-determinante.
                    PolyMinorValue mv = getMinorPrivate(k - 1, subMk);  // recursive call
                    m += mv.getMultiplications();
                    s += mv.getSummations();
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result = ops(result, signPoly, mv.getResult(), _polyMatrix[b][absoluteC]); // adding sub-determinante times matrix entry
                                                                                               // times appropriate sign
                    s++; m++; as++, am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
                signPoly = pISet(sign);
            }
            // p_Delete(&signPoly, currRing);
        }
        else {
            b = - b - 1;
            // This means that the best line is the column with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorColumnKey:
            int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
            poly signPoly = pISet(sign);
            for (int r = 0; r < k; r++) {
                int absoluteR = mk.getAbsoluteRowIndex(r);        // This iterates over all involved rows.
                MinorKey subMk = mk.getSubMinorKey(absoluteR, b); // This is MinorKey when we omit row absoluteR and column b.
                if (!isEntryZero(absoluteR, b)) { // Only then do we have to consider this sub-determinante.
                    PolyMinorValue mv = getMinorPrivate(k - 1, subMk);  // recursive call
                    m += mv.getMultiplications();
                    s += mv.getSummations();
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result = ops(result, signPoly, mv.getResult(), _polyMatrix[absoluteR][b]); // adding sub-determinante times matrix entry
                                                                                               // times appropriate sign
                    s++; m++; as++, am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
                signPoly = pISet(sign);
            }
            // p_Delete(&signPoly, currRing);
        }
        s--; as--; // first summation was 0 + ..., so we do not count it
        if (s < 0) s = 0; // may happen when all subminors are zero and no summation needs to be performed
        if (as < 0) as = 0; // may happen when all subminors are zero and no summation needs to be performed
        PolyMinorValue newMV(result, m, s, am, as, -1, -1); // "-1" is to signal that any statistics about the
                                                            // number of retrievals does not make sense, as we
                                                            // do not use a cache.
        // p_Delete(&result, currRing);
        return newMV;
    }
}

PolyMinorValue PolyMinorProcessor::getMinorPrivate(const int k, const MinorKey& mk,
                                                   const bool multipleMinors,
                                                   Cache<MinorKey, PolyMinorValue>& cch) {
    assert(k > 0); // k is the minor's dimension; the minor must be at least 1x1
    // The method works by recursion, and using Lapace's Theorem along the row/column with the most zeros.
    if (k == 1) {
        return PolyMinorValue(_polyMatrix[mk.getAbsoluteRowIndex(0)][mk.getAbsoluteColumnIndex(0)],
                              0, 0, 0, 0, -1, -1); // we set "-1" as, for k == 1, we do not have any cache retrievals
    }
    else {
        int b = getBestLine(k, mk);                          // row or column with most zeros
        poly result = pISet(0);                              // This will contain the value of the minor.
        int s = 0; int m = 0; int as = 0; int am = 0;        // counters for summations and multiplications,
                                                             // ..."a*" for accumulated operation counters
        PolyMinorValue mv = PolyMinorValue(0, 0, 0, 0, 0, 0, 0);     // for storing all intermediate minors
        if (b >= 0) {
            // This means that the best line is the row with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorRowKey:
            int sign = (mk.getRelativeRowIndex(b) % 2 == 0 ? 1 : -1);
            poly signPoly = pISet(sign);
            for (int c = 0; c < k; c++) {
                int absoluteC = mk.getAbsoluteColumnIndex(c);      // This iterates over all involved columns.
                MinorKey subMk = mk.getSubMinorKey(b, absoluteC);  // This is MinorKey when we omit row b and column absoluteC.
                if (!isEntryZero(b, absoluteC)) { // Only then do we have to consider this sub-determinante.
                    if (cch.hasKey(subMk)) { // trying to find the result in the cache
                        mv = cch.getValue(subMk);
                        mv.incrementRetrievals(); // once more, we made use of the cached value for key mk
                        cch.put(subMk, mv); // We need to do this with "put", as the (altered) number of retrievals may have
                                            // an impact on the internal ordering among cache entries.
                    }
                    else {
                        mv = getMinorPrivate(k - 1, subMk, multipleMinors, cch); // recursive call
                        // As this minor was not in the cache, we count the summations and
                        // multiplications that we needed to do in the recursive call:
                        m += mv.getMultiplications();
                        s += mv.getSummations();
                    }
                    // In any case, we count all nested operations in the accumulative counters:
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result = ops(result, signPoly, mv.getResult(), _polyMatrix[b][absoluteC]); // adding sub-determinante times matrix entry
                                                                                               // times appropriate sign
                    s++; m++; as++; am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
                signPoly = pISet(sign);
            }
        }
        else {
            b = - b - 1;
            // This means that the best line is the column with absolute (0-based) index b.
            // Using Laplace, the sign of the contributing minors must be iterating;
            // the initial sign depends on the relative index of b in minorColumnKey:
            int sign = (mk.getRelativeColumnIndex(b) % 2 == 0 ? 1 : -1);
            poly signPoly = pISet(sign);
            for (int r = 0; r < k; r++) {
                int absoluteR = mk.getAbsoluteRowIndex(r);        // This iterates over all involved rows.
                MinorKey subMk = mk.getSubMinorKey(absoluteR, b); // This is MinorKey when we omit row absoluteR and column b.
                if (!isEntryZero(absoluteR, b)) { // Only then do we have to consider this sub-determinante.
                    if (cch.hasKey(subMk)) { // trying to find the result in the cache
                        mv = cch.getValue(subMk);
                        mv.incrementRetrievals(); // once more, we made use of the cached value for key mk
                        cch.put(subMk, mv); // We need to do this with "put", as the (altered) number of retrievals may have
                                            // an impact on the internal ordering among cache entries.
                    }
                    else {
                        mv = getMinorPrivate(k - 1, subMk, multipleMinors, cch); // recursive call
                        // As this minor was not in the cache, we count the summations and
                        // multiplications that we needed to do in the recursive call:
                        m += mv.getMultiplications();
                        s += mv.getSummations();
                    }
                    // In any case, we count all nested operations in the accumulative counters:
                    am += mv.getAccumulatedMultiplications();
                    as += mv.getAccumulatedSummations();
                    result = ops(result, signPoly, mv.getResult(), _polyMatrix[absoluteR][b]); // adding sub-determinante times matrix entry
                                                                                               // times appropriate sign
                    s++; m++; as++; am++; // This is for the summation and multiplication in the previous line of code.
                }
                sign = - sign; // alternating the sign
                signPoly = pISet(sign);
            }
        }
        // Let's cache the newly computed minor:
        int potentialRetrievals = NumberOfRetrievals(_containerRows, _containerColumns, _minorSize, k, multipleMinors);
        s--; as--; // first summation was 0 + ..., so we do not count it
        if (s < 0) s = 0; // may happen when all subminors are zero and no summation needs to be performed
        if (as < 0) as = 0; // may happen when all subminors are zero and no summation needs to be performed
        PolyMinorValue newMV = PolyMinorValue(result, m, s, am, as, 1, potentialRetrievals);
        cch.put(mk, newMV); // Here's the actual put inside the cache.
        return newMV;
    }
}





