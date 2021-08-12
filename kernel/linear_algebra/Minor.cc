


#include "kernel/mod2.h"

#include "kernel/linear_algebra/Minor.h"

#include "kernel/structs.h"
#include "kernel/polys.h"

using namespace std;

void MinorKey::reset()
{
  _numberOfRowBlocks = 0;
  _numberOfColumnBlocks = 0;
  omfree(_rowKey);
  _rowKey = NULL;
  omfree(_columnKey);
  _columnKey = NULL;
}

MinorKey::MinorKey (const MinorKey& mk)
{
  _numberOfRowBlocks = mk.getNumberOfRowBlocks();
  _numberOfColumnBlocks = mk.getNumberOfColumnBlocks();;

  /* allocate memory for new entries in _rowKey and _columnKey */
  _rowKey = (unsigned*)omAlloc(_numberOfRowBlocks*sizeof(unsigned));
  _columnKey = (unsigned*)omAlloc(_numberOfColumnBlocks*sizeof(unsigned));

  /* copying values from parameter arrays to private arrays */
  for (int r = 0; r < _numberOfRowBlocks; r++)
    _rowKey[r] = mk.getRowKey(r);
  for (int c = 0; c < _numberOfColumnBlocks; c++)
    _columnKey[c] = mk.getColumnKey(c);
}

MinorKey& MinorKey::operator=(const MinorKey& mk)
{
  omfree(_rowKey); _rowKey = NULL;
  omfree(_columnKey); _columnKey = NULL;
  _numberOfRowBlocks = 0;
  _numberOfColumnBlocks = 0;

  _numberOfRowBlocks = mk.getNumberOfRowBlocks();
  _numberOfColumnBlocks = mk.getNumberOfColumnBlocks();;

  /* allocate memory for new entries in _rowKey and _columnKey */
  _rowKey = (unsigned*)omalloc(_numberOfRowBlocks*sizeof(unsigned));
  _columnKey = (unsigned*)omalloc(_numberOfColumnBlocks*sizeof(unsigned));

  /* copying values from parameter arrays to private arrays */
  for (int r = 0; r < _numberOfRowBlocks; r++)
      _rowKey[r] = mk.getRowKey(r);
  for (int c = 0; c < _numberOfColumnBlocks; c++)
      _columnKey[c] = mk.getColumnKey(c);

  return *this;
}

void MinorKey::set(const int lengthOfRowArray, const unsigned int* rowKey,
                   const int lengthOfColumnArray,
                   const unsigned int* columnKey)
{
  /* free memory of _rowKey and _columnKey */
  if (_numberOfRowBlocks > 0) { omFree(_rowKey); }
  if (_numberOfColumnBlocks > 0) { omFree(_columnKey); }

  _numberOfRowBlocks = lengthOfRowArray;
  _numberOfColumnBlocks = lengthOfColumnArray;

  /* allocate memory for new entries in _rowKey and _columnKey; */
  _rowKey = (unsigned*)omAlloc(_numberOfRowBlocks*sizeof(unsigned));
  _columnKey = (unsigned*)omAlloc(_numberOfColumnBlocks*sizeof(unsigned));

  /* copying values from parameter arrays to private arrays */
  for (int r = 0; r < _numberOfRowBlocks; r++)
    _rowKey[r] = rowKey[r];
  for (int c = 0; c < _numberOfColumnBlocks; c++)
    _columnKey[c] = columnKey[c];
}

MinorKey::MinorKey(const int lengthOfRowArray,
                   const unsigned int* const rowKey,
                   const int lengthOfColumnArray,
                   const unsigned int* const columnKey)
{
  _numberOfRowBlocks = lengthOfRowArray;
  _numberOfColumnBlocks = lengthOfColumnArray;

  /* allocate memory for new entries in _rowKey and _columnKey */
  _rowKey = (unsigned*)omalloc(_numberOfRowBlocks*sizeof(unsigned));
  _columnKey = (unsigned*)omalloc(_numberOfColumnBlocks*sizeof(unsigned));

  /* copying values from parameter arrays to private arrays */
  for (int r = 0; r < _numberOfRowBlocks; r++)
    _rowKey[r] = rowKey[r];

  for (int c = 0; c < _numberOfColumnBlocks; c++)
    _columnKey[c] = columnKey[c];
}

MinorKey::~MinorKey()
{
  _numberOfRowBlocks = 0;
  _numberOfColumnBlocks = 0;
  omfree(_rowKey); _rowKey = NULL;
  omfree(_columnKey); _columnKey = NULL;
}

//void MinorKey::print() const
//{
//  PrintS(this->toString().c_str());
//}

int MinorKey::getAbsoluteRowIndex(const int i) const
{
  /* This method is to return the absolute (0-based) index of the i-th
     row encoded in \a this.
     Example: bit-pattern of rows: "10010001101", i = 3:
        This should yield the 0-based absolute index of the 3-rd bit
        (counted from the right), i.e. 7. */

  int matchedBits = -1; /* counter for matched bits;
                           this needs to reach i, then we're done */
  for (int block = 0; block < getNumberOfRowBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getRowKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold throughout the
       entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) matchedBits++;
      if (matchedBits == i) return exponent + (32 * block);
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* We should never reach this line of code. */
  assume(false);
  return -1;
}

int MinorKey::getAbsoluteColumnIndex(const int i) const
{
  /* This method is to return the absolute (0-based) index of the i-th
     column encoded in \a this.
     Example: bit-pattern of columns: "10010001101", i = 3:
        This should yield the 0-based absolute index of the 3-rd bit
        (counted from the right), i.e. 7. */

  int matchedBits = -1; /* counter for matched bits; this needs to reach i,
                           then we're done */
  for (int block = 0; block < getNumberOfColumnBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getColumnKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold throughout the
       entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) matchedBits++;
      if (matchedBits == i) return exponent + (32 * block);
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* We should never reach this line of code. */
  assume(false);
  return -1;
}

void MinorKey::getAbsoluteRowIndices(int* const target) const
{
  int i = 0; /* index for filling the target array */
  for (int block = 0; block < getNumberOfRowBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getRowKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold throughout the
       entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) target[i++] = exponent + (32 * block);
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
}

void MinorKey::getAbsoluteColumnIndices(int* const target) const
{
  int i = 0; /* index for filling the target array */
  for (int block = 0; block < getNumberOfColumnBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getColumnKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold throughout the
       entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) target[i++] = exponent + (32 * block);
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
}

int MinorKey::getRelativeRowIndex(const int i) const
{
  /* This method is to return the relative (0-based) index of the row
     with absolute index \c i.
     Example: bit-pattern of rows: "10010001101", i = 7:
        This should yield the 0-based relative index of the bit
        corresponding to row no. 7, i.e. 3. */

  int matchedBits = -1; /* counter for matched bits; this is going to
                           contain our return value */
  for (int block = 0; block < getNumberOfRowBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getRowKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold throughout the
       entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) matchedBits++;
      if (exponent + (32 * block) == i) return matchedBits;
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* We should never reach this line of code. */
  assume(false);
  return -1;
}

int MinorKey::getRelativeColumnIndex(const int i) const
{
  /* This method is to return the relative (0-based) index
     of the column with absolute index \c i.
     Example: bit-pattern of columns: "10010001101", i = 7:
        This should yield the 0-based relative index of the bit
        corresponding to column no. 7, i.e. 3. */

  int matchedBits = -1; /* counter for matched bits; this is going
                           to contain our return value */
  for (int block = 0; block < getNumberOfColumnBlocks(); block ++)
  {
    /* start with lowest bits, i.e. in block No. 0 */
    /* the bits in this block of 32 bits: */
    unsigned int blockBits = getColumnKey(block);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* The invariant "shiftedBit = 2^exponent" will hold
       throughout the entire while loop. */
    while (exponent < 32)
    {
      if (shiftedBit & blockBits) matchedBits++;
      if (exponent + (32 * block) == i) return matchedBits;
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* We should never reach this line of code. */
  assume(false);
  return -1;
}

unsigned int MinorKey::getRowKey(const int blockIndex) const
{
  return _rowKey[blockIndex];
}

unsigned int MinorKey::getColumnKey(const int blockIndex) const
{
  return _columnKey[blockIndex];
}

int MinorKey::getNumberOfRowBlocks() const
{
  return _numberOfRowBlocks;
}

int MinorKey::getNumberOfColumnBlocks() const
{
  return _numberOfColumnBlocks;
}

#ifndef SING_NDEBUG
int MinorKey::getSetBits(const int a) const
{
  int b = 0;
  if (a == 1)
  { /* rows */
    for (int i = 0; i < _numberOfRowBlocks; i++)
    {
      unsigned int m = _rowKey[i];
      unsigned int k = 1;
      for (int j = 0; j < 32; j++)
      {
        /* k = 2^j */
        if (m & k) b++;
        k = k << 1;
      }
    }
  }
  else
  { /* columns */
    for (int i = 0; i < _numberOfColumnBlocks; i++)
    {
      unsigned int m = _columnKey[i];
      unsigned int k = 1;
      for (int j = 0; j < 32; j++)
      {
        /* k = 2^j */
        if (m & k) b++;
        k = k << 1;
      }
    }
  }
  return b;
}
#endif

MinorKey MinorKey::getSubMinorKey (const int absoluteEraseRowIndex,
                                   const int absoluteEraseColumnIndex) const
{
  int rowBlock = absoluteEraseRowIndex / 32;
  int exponent = absoluteEraseRowIndex % 32;
  unsigned int newRowBits = getRowKey(rowBlock) - (1 << exponent);
  int highestRowBlock = getNumberOfRowBlocks() - 1;
  /* highestRowBlock will finally contain the highest block index with
     non-zero bit pattern */
  if ((newRowBits == 0) && (rowBlock == highestRowBlock))
  {
    /* we have thus nullified the highest block;
       we can now forget about the highest block... */
    highestRowBlock -= 1;
    while (getRowKey(highestRowBlock) == 0) /* ...and maybe even some more
                                               zero-blocks */
      highestRowBlock -= 1;
  }
  /* highestRowBlock now contains the highest row block index with non-zero
     bit pattern */

  int columnBlock = absoluteEraseColumnIndex / 32;
  exponent = absoluteEraseColumnIndex % 32;
  unsigned int newColumnBits = getColumnKey(columnBlock) - (1 << exponent);
  int highestColumnBlock = getNumberOfColumnBlocks() - 1;
  /* highestColumnBlock will finally contain the highest block index with
     non-zero bit pattern */
  if ((newColumnBits == 0) && (columnBlock == highestColumnBlock))
  {
    /* we have thus nullified the highest block;
       we can now forget about the highest block... */
    highestColumnBlock -= 1;
    while (getColumnKey(highestColumnBlock) == 0) /* ...and maybe even some
                                                     more zero-blocks */
      highestColumnBlock -= 1;
  }
  /* highestColumnBlock now contains the highest column block index with
     non-zero bit pattern */

  MinorKey result(highestRowBlock + 1, _rowKey, highestColumnBlock + 1,
                  _columnKey);
  /* This is just a copy with maybe some leading bit blocks omitted. We still
     need to re-define the row block at index 'rowBlock' and the column block
     at index 'columnBlock': */
  if ((newRowBits != 0) || (rowBlock < getNumberOfRowBlocks() - 1))
    result.setRowKey(rowBlock, newRowBits);
  if ((newColumnBits != 0) || (columnBlock < getNumberOfColumnBlocks() - 1))
    result.setColumnKey(columnBlock, newColumnBits);

  #ifndef SING_NDEBUG
  /* let's check that the number of selected rows and columns are equal;
     (this check is only performed in the debug version) */
  assume(result.getSetBits(1) == result.getSetBits(2));
  #endif

  return result;
}

void MinorKey::setRowKey (const int blockIndex, const unsigned int rowKey)
{
    _rowKey[blockIndex] = rowKey;
}

void MinorKey::setColumnKey (const int blockIndex,
                             const unsigned int columnKey)
{
    _columnKey[blockIndex] = columnKey;
}

int MinorKey::compare (const MinorKey& that) const
{
  /* compare by rowKeys first; in case of equality, use columnKeys */
  if (this->getNumberOfRowBlocks() < that.getNumberOfRowBlocks())
    return -1;
  if (this->getNumberOfRowBlocks() > that.getNumberOfRowBlocks())
    return 1;
  /* Here, numbers of rows are equal. */
  for (int r = this->getNumberOfRowBlocks() - 1; r >= 0; r--)
  {
    if (this->getRowKey(r) < that.getRowKey(r)) return -1;
    if (this->getRowKey(r) > that.getRowKey(r)) return 1;
  }
  /* Here, this and that encode ecaxtly the same sets of rows.
     Now, we take a look at the columns. */
  if (this->getNumberOfColumnBlocks() < that.getNumberOfColumnBlocks())
    return -1;
  if (this->getNumberOfColumnBlocks() > that.getNumberOfColumnBlocks())
    return 1;
  /* Here, numbers of columns are equal. */
  for (int c = this->getNumberOfColumnBlocks() - 1; c >= 0; c--)
  {
    if (this->getColumnKey(c) < that.getColumnKey(c)) return -1;
    if (this->getColumnKey(c) > that.getColumnKey(c)) return 1;
  }
  /* Here, this and that encode exactly the same sets of rows and columns. */
  return 0;
}

/* just to make the compiler happy;
   this method should never be called */
bool MinorKey::operator==(const MinorKey& mk) const
{
  assume(false);
  return this->compare(mk) == 0;
}

/* just to make the compiler happy;
   this method should never be called */
bool MinorKey::operator<(const MinorKey& mk) const
{
  assume(false);
  return this->compare(mk) == -1;
}

void MinorKey::selectFirstRows (const int k, const MinorKey& mk)
{
  int hitBits = 0;      /* the number of bits we have hit; in the end, this
                           has to be equal to k, the dimension of the minor */
  int blockIndex = -1;  /* the index of the current int in mk */
  unsigned int highestInt = 0;  /* the new highest block of this MinorKey */
  /* We determine which ints of mk we can copy. Their indices will be
     0, 1, ..., blockIndex - 1. And highestInt is going to capture the highest
     int (which may be only a portion of the corresponding int in mk.
     We loop until hitBits = k: */
  while (hitBits < k)
  {
    blockIndex++;
    highestInt = 0;
    unsigned int currentInt = mk.getRowKey(blockIndex);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* invariant in the loop: shiftedBit = 2^exponent */
    while (exponent < 32 && hitBits < k)
    {
      if (shiftedBit & currentInt)
      {
        highestInt += shiftedBit;
        hitBits++;
      }
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* free old memory */
  omfree(_rowKey);
  _rowKey = NULL;
  _numberOfRowBlocks = blockIndex + 1;
  /* allocate memory for new entries in _rowKey; */
  _rowKey = (unsigned*)omAlloc(_numberOfRowBlocks*sizeof(unsigned));
  /* copying values from mk to this MinorKey */
  for (int r = 0; r < blockIndex; r++)
    _rowKey[r] = mk.getRowKey(r);
  _rowKey[blockIndex] = highestInt;
}

void MinorKey::selectFirstColumns (const int k, const MinorKey& mk)
{
  int hitBits = 0;      /* the number of bits we have hit; in the end, this
                           has to be equal to k, the dimension of the minor */
  int blockIndex = -1;  /* the index of the current int in mk */
  unsigned int highestInt = 0;  /* the new highest block of this MinorKey */
  /* We determine which ints of mk we can copy. Their indices will be
     0, 1, ..., blockIndex - 1. And highestInt is going to capture the highest
     int (which may be only a portion of the corresponding int in mk.
     We loop until hitBits = k: */
  while (hitBits < k)
  {
    blockIndex++;
    highestInt = 0;
    unsigned int currentInt = mk.getColumnKey(blockIndex);
    unsigned int shiftedBit = 1;
    int exponent = 0;
    /* invariant in the loop: shiftedBit = 2^exponent */
    while (exponent < 32 && hitBits < k)
    {
      if (shiftedBit & currentInt)
      {
        highestInt += shiftedBit;
        hitBits++;
      }
      shiftedBit = shiftedBit << 1;
      exponent++;
    }
  }
  /* free old memory */
  omfree(_columnKey); _columnKey = NULL;
  _numberOfColumnBlocks = blockIndex + 1;
  /* allocate memory for new entries in _columnKey; */
  _columnKey = (unsigned*)omAlloc(_numberOfColumnBlocks*sizeof(unsigned));
  /*  copying values from mk to this MinorKey */
  for (int c = 0; c < blockIndex; c++)
    _columnKey[c] = mk.getColumnKey(c);
  _columnKey[blockIndex] = highestInt;
}

bool MinorKey::selectNextRows (const int k, const MinorKey& mk)
{
  /* We need to compute the set of k rows which must all be contained in mk.
     AND: This set must be the least possible of this kind which is larger
          than the currently encoded set of rows. (Here, '<' is w.r.t. to the
          natural ordering on multi-indices.
     Example: mk encodes the rows according to the bit pattern 11010111,
              k = 3, this MinorKey encodes 10010100. Then, the method must
              shift the set of rows in this MinorKey to 11000001 (, and
              return true). */

  /* The next two variables will finally name a row which is
     (1) currently not yet among the rows in this MinorKey, but
     (2) among the rows in mk, and
     (3) which is "higher" than the lowest row in this MinorKey, and
     (4) which is the lowest possible choice such that (1) - (3) hold.
     If we should not be able to find such a row, then there is no next
     subset of rows. In this case, the method will return false; otherwise
     always true. */
  int newBitBlockIndex = 0;        /* the block index of the bit */
  unsigned int newBitToBeSet = 0;  /* the bit as 2^e, where 0 <= e <= 31 */

  /* number of ints (representing rows) in this MinorKey: */
  int blockCount = this->getNumberOfRowBlocks();
  /* for iterating along the blocks of mk: */
  int mkBlockIndex = mk.getNumberOfRowBlocks();

  int hitBits = 0;    /* the number of bits we have hit */
  int bitCounter = 0; /* for storing the number of bits hit before a
                         specific moment; see below */
  while (hitBits < k)
  {
    mkBlockIndex--;
    unsigned int currentInt = mk.getRowKey(mkBlockIndex);
    unsigned int shiftedBit = 1 << 31; /* initially, this equals 2^31, i.e.
                                          the highest bit */
    while (hitBits < k && shiftedBit > 0)
    {
      if ((blockCount - 1 >= mkBlockIndex) &&
        (shiftedBit & this->getRowKey(mkBlockIndex))) hitBits++;
      else if (shiftedBit & currentInt)
      {
        newBitToBeSet = shiftedBit;
        newBitBlockIndex = mkBlockIndex;
        bitCounter = hitBits; /* So, whenever we set newBitToBeSet, we want
                                 to remember the momentary number of hit
                                 bits. This will later be needed; see below. */
      }
      shiftedBit = shiftedBit >> 1;
    }
  }
  if (newBitToBeSet == 0)
  {
    return false;
  }
  else
  {
    /* Note that the following must hold when reaching this line of code:
       (1) The row with bit newBitToBeSet in this->getRowKey(newBitBlockIndex)
           is currently not among the rows in this MinorKey, but
       (2) it is among the rows in mk, and
       (3) it is higher than the lowest row in this MinorKey, and
       (4) it is the lowest possible choice such that (1) - (3) hold.
       In the above example, we would reach this line with
       newBitToBeSet == 2^6 and bitCounter == 1 (resulting from the bit 2^7).
       */

    if (blockCount - 1 < newBitBlockIndex)
    { /* In this case, _rowKey is too small. */
      /* free old memory */
      omFree(_rowKey); _rowKey = NULL;
      _numberOfRowBlocks = newBitBlockIndex + 1;
      /* allocate memory for new entries in _rowKey; */
      _rowKey = (unsigned*)omAlloc(_numberOfRowBlocks*sizeof(unsigned));
      /* initializing entries to zero */
        for (int r = 0; r < _numberOfRowBlocks; r++) _rowKey[r] = 0;
    }
    else
    {
      /* We need to delete all bits in _rowKey[newBitBlockIndex] that are
         below newBitToBeSet: */
      unsigned int anInt = this->getRowKey(newBitBlockIndex);
      unsigned int deleteBit = newBitToBeSet >> 1; // in example: = 2^5
      while (deleteBit > 0)
      {
        if (anInt & deleteBit) anInt -= deleteBit;
        deleteBit = deleteBit >> 1;
      };
      _rowKey[newBitBlockIndex] = anInt;
      /* ...and we delete all entries in _rowKey[i] for
         0 <= i < newBitBlockIndex */
      for (int i = 0; i < newBitBlockIndex; i++)
        _rowKey[i] = 0;
    }

    /* We have now deleted all bits from _rowKey[...] below the bit
       2^newBitToBeSet.
       In the example we shall have at this point: _rowKey[...] = 10000000.
       Now let's set the new bit: */
    _rowKey[newBitBlockIndex] += newBitToBeSet;
    /* in the example: _rowKey[newBitBlockIndex] = 11000000 */
    bitCounter++; /* This is now the number of correct bits in _rowKey[...];
                     i.e. in the example this will be equal to 2. */

    /* Now we only need to fill _rowKey[...] with the lowest possible bits
       until it consists of exactly k bits. (We know that we need to set
       exactly (k - bitCounter) additional bits.) */
    mkBlockIndex = -1;
    while (bitCounter < k)
    {
      mkBlockIndex++;
      unsigned int currentInt = mk.getRowKey(mkBlockIndex);
      unsigned int shiftedBit = 1;
      int exponent = 0;
      /* invariant: shiftedBit = 2^exponent */
      while (bitCounter < k && exponent < 32)
      {
        if (shiftedBit & currentInt)
        {
          _rowKey[mkBlockIndex] += shiftedBit;
          bitCounter++;
        };
        shiftedBit = shiftedBit << 1;
        exponent++;
      }
    };
    /* in the example, we shall obtain _rowKey[...] = 11000001 */
    return true;
  }
}

bool MinorKey::selectNextColumns (const int k, const MinorKey& mk)
{
  /* We need to compute the set of k columns which must all be contained in mk.
     AND: This set must be the least possible of this kind which is larger
          than the currently encoded set of columns. (Here, '<' is w.r.t. to
          the natural ordering on multi-indices.
     Example: mk encodes the columns according to the bit pattern 11010111,
              k = 3, this MinorKey encodes 10010100. Then, the method must
              shift the set of columns in this MinorKey to 11000001 (, and
              return true). */

  /* The next two variables will finally name a column which is
     (1) currently not yet among the columns in this MinorKey, but
     (2) among the columns in mk, and
     (3) which is "higher" than the lowest column in this MinorKey, and
     (4) which is the lowest possible choice such that (1) - (3) hold.
     If we should not be able to find such a column, then there is no next
     subset of columns. In this case, the method will return false; otherwise
     always true. */
  int newBitBlockIndex = 0;        /* the block index of the bit */
  unsigned int newBitToBeSet = 0;  /* the bit as 2^e, where 0 <= e <= 31 */

   /* number of ints (representing columns) in this MinorKey: */
  int blockCount = this->getNumberOfColumnBlocks();
  /* for iterating along the blocks of mk: */
  int mkBlockIndex = mk.getNumberOfColumnBlocks();

  int hitBits = 0;    /* the number of bits we have hit */
  int bitCounter = 0; /* for storing the number of bits hit before a specific
                         moment; see below */
  while (hitBits < k)
  {
    mkBlockIndex--;
    unsigned int currentInt = mk.getColumnKey(mkBlockIndex);
    unsigned int shiftedBit = 1 << 31; /* initially, this equals 2^31, i.e.
                                          the highest bit */
    while (hitBits < k && shiftedBit > 0)
    {
      if ((blockCount - 1 >= mkBlockIndex) &&
        (shiftedBit & this->getColumnKey(mkBlockIndex))) hitBits++;
      else if (shiftedBit & currentInt)
      {
        newBitToBeSet = shiftedBit;
        newBitBlockIndex = mkBlockIndex;
        bitCounter = hitBits; /* So, whenever we set newBitToBeSet, we want to
                                 remember the momentary number of hit bits.
                                 This will later be needed; see below. */
      }
      shiftedBit = shiftedBit >> 1;
    }
  }
  if (newBitToBeSet == 0)
  {
    return false;
  }
  else
  {
    /* Note that the following must hold when reaching this line of code:
       (1) The column with bit newBitToBeSet in
           this->getColumnKey(newBitBlockIndex) is currently not among the
           columns in this MinorKey, but
       (2) it is among the columns in mk, and
       (3) it is higher than the lowest columns in this MinorKey, and
       (4) it is the lowest possible choice such that (1) - (3) hold.
       In the above example, we would reach this line with
       newBitToBeSet == 2^6 and bitCounter == 1 (resulting from the bit 2^7).
       */

    if (blockCount - 1 < newBitBlockIndex)
    { /* In this case, _columnKey is too small. */
        /* free old memory */
        omFree( _columnKey); _columnKey = NULL;
        _numberOfColumnBlocks = newBitBlockIndex + 1;
        /* allocate memory for new entries in _columnKey; */
        _columnKey = (unsigned*)omAlloc(_numberOfColumnBlocks*sizeof(unsigned));
        /* initializing entries to zero */
        for (int c = 0; c < _numberOfColumnBlocks; c++) _columnKey[c] = 0;
    }
    else
    {
      /* We need to delete all bits in _columnKey[newBitBlockIndex] that are
         below newBitToBeSet: */
      unsigned int anInt = this->getColumnKey(newBitBlockIndex);
      unsigned int deleteBit = newBitToBeSet >> 1; /* in example: = 2^5 */
      while (deleteBit > 0)
      {
        if (anInt & deleteBit) anInt -= deleteBit;
        deleteBit = deleteBit >> 1;
      };
      _columnKey[newBitBlockIndex] = anInt;
      /* ...and we delete all entries in _columnKey[i] fo
         0 <= i < newBitBlockIndex */
      for (int i = 0; i < newBitBlockIndex; i++)
        _columnKey[i] = 0;
    }
    /* We have now deleted all bits from _columnKey[...] below the bit
       2^newBitToBeSet. In the example we shall have at this point:
       _columnKey[...] = 10000000. Now let's set the new bit: */
    _columnKey[newBitBlockIndex] += newBitToBeSet;
    /* in the example: _columnKey[newBitBlockIndex] = 11000000 */
    bitCounter++; /* This is now the number of correct bits in
                     _columnKey[...]; i.e. in the example this will be equal
                     to 2. */

    /* Now we only need to fill _columnKey[...] with the lowest possible bits
       until it consists of exactly k bits. (We know that we need to set
       exactly (k - bitCounter) additional bits.) */
    mkBlockIndex = -1;
    while (bitCounter < k)
    {
      mkBlockIndex++;
      unsigned int currentInt = mk.getColumnKey(mkBlockIndex);
      unsigned int shiftedBit = 1;
      int exponent = 0;
      /* invariant: shiftedBit = 2^exponent */
      while (bitCounter < k && exponent < 32)
      {
        if (shiftedBit & currentInt)
        {
          _columnKey[mkBlockIndex] += shiftedBit;
          bitCounter++;
        };
        shiftedBit = shiftedBit << 1;
        exponent++;
      }
    };
    /* in the example, we shall obtain _columnKey[...] = 11000001 */
    return true;
  }
}

string MinorKey::toString() const
{ return ""; }
/*
  string t;
  string s = "(";
  unsigned int z = 0;
  for (int r = this->getNumberOfRowBlocks() - 1; r >= 0; r--)
  {
    t = "";
    z = this->getRowKey(r);
    while (z != 0)
    {
      if ((z % 2) != 0) t = "1" + t; else t = "0" + t;
      z = z / 2;
    }
    if (r < this->getNumberOfRowBlocks() - 1)
      t = string(32 - t.length(), '0') + t;
    s += t;
  }
  s += ", ";
  for (int c = this->getNumberOfColumnBlocks() - 1; c >= 0; c--)
  {
    t = "";
    z = this->getColumnKey(c);
    while (z != 0)
    {
      if ((z % 2) != 0) t = "1" + t; else t = "0" + t;
      z = z / 2;
    }
    if (c < this->getNumberOfColumnBlocks() - 1)
      t = string(32 - t.length(), '0') + t;
    s += t;
  }
  s += ")";
  return s;
}
*/

THREAD_VAR int MinorValue::g_rankingStrategy = -1;

int MinorValue::getWeight () const
{
  assume(false);  /* must be overridden in derived classes */
  return 0;
}

/* just to make the compiler happy;
   this method should never be called */
bool MinorValue::operator==(const MinorValue& mv) const
{
  assume(false);
  return (this == &mv);  /* compare addresses of both objects */
}

string MinorValue::toString () const
{
  assume(false);  /* must be overridden in derived classes */
  return "";
}

/* just to make the compiler happy;
   this method should never be called */
bool MinorValue::operator<(const MinorValue& mv) const
{
  assume(false);
  return (this < &mv);  /* compare addresses of both objects */
}

int MinorValue::getRetrievals() const
{
  return _retrievals;
}

void MinorValue::incrementRetrievals()
{
  _retrievals++;
}

int MinorValue::getPotentialRetrievals() const
{
  return _potentialRetrievals;
}

int MinorValue::getMultiplications() const
{
  return _multiplications;
}

int MinorValue::getAdditions() const
{
  return _additions;
}

int MinorValue::getAccumulatedMultiplications() const
{
  return _accumulatedMult;
}

int MinorValue::getAccumulatedAdditions() const
{
  return _accumulatedSum;
}

void MinorValue::print() const
{
  PrintS(this->toString().c_str());
}


void MinorValue::SetRankingStrategy (const int rankingStrategy)
{
  g_rankingStrategy = rankingStrategy;
  //if (g_rankingStrategy == 6) : rand() is never used
  //{
  //  /* initialize the random generator with system time */
  //  srand ( time(NULL) );
  //}
}

int MinorValue::GetRankingStrategy()
{
  return g_rankingStrategy;
}

/* this is for generically accessing the rank measure regardless of
   which strategy has been set */
int MinorValue::getUtility () const
{
  switch (this->GetRankingStrategy())
  {
    case 1: return this->rankMeasure1();
    case 2: return this->rankMeasure2();
    case 3: return this->rankMeasure3();
    case 4: return this->rankMeasure4();
    case 5: return this->rankMeasure5();
    default: return this->rankMeasure1();
  }
}

/* here are some sensible caching strategies: */
int MinorValue::rankMeasure1 () const
{
  /* number of actually performed multiplications */
  return this->getMultiplications();
}

int MinorValue::rankMeasure2 () const
{
  /* accumulated number of performed multiplications, i.e. all including
     nested multiplications */
  return this->getAccumulatedMultiplications();
}

int MinorValue::rankMeasure3 () const
{
  /* number of performed multiplications, weighted with the ratio of
     not yet performed retrievals over the maximal number of retrievals */
  return this->getMultiplications()
         * (this->getPotentialRetrievals()
         - this->getRetrievals())
         / this->getPotentialRetrievals();
}

int MinorValue::rankMeasure4 () const
{
  /* number of performed multiplications,
     multiplied with the number of not yet performed retrievals */
  return this->getMultiplications()
         * (this->getPotentialRetrievals()
         - this->getRetrievals());
}

int MinorValue::rankMeasure5 () const
{
  /* number of not yet performed retrievals;
     tends to cache entries longer when they are going to be retrieved more
     often in the future */
  return this->getPotentialRetrievals() - this->getRetrievals();
}

int IntMinorValue::getWeight () const
{
  /* put measure for size of MinorValue here, i.e. number of monomials in
     polynomial; so far, we use the accumulated number of multiplications
     (i.e., including all nested ones) to simmulate the size of a polynomial */
  return _accumulatedMult;
}

IntMinorValue::IntMinorValue (const int result, const int multiplications,
                              const int additions,
                              const int accumulatedMultiplications,
                              const int accumulatedAdditions,
                              const int retrievals,
                              const int potentialRetrievals)
{
  _result = result;
  _multiplications = multiplications;
  _additions = additions;
  _accumulatedMult = accumulatedMultiplications;
  _accumulatedSum = accumulatedAdditions;
  _potentialRetrievals = potentialRetrievals;
  _retrievals = retrievals;
}

IntMinorValue::IntMinorValue ()
{
  _result = -1;
  _multiplications = -1;
  _additions = -1;
  _accumulatedMult = -1;
  _accumulatedSum = -1;
  _potentialRetrievals = -1;
  _retrievals = -1;
}

IntMinorValue::~IntMinorValue()
{
}

int IntMinorValue::getResult() const
{
  return _result;
}

string IntMinorValue::toString () const
{
  char h[10];

  /* Let's see whether a cache has been used to compute this MinorValue: */
  bool cacheHasBeenUsed = true;
  if (this->getRetrievals() == -1) cacheHasBeenUsed = false;

  sprintf(h, "%d", this->getResult());
  string s = h;
  s += " [retrievals: ";
  if (cacheHasBeenUsed) { sprintf(h, "%d", this->getRetrievals()); s += h; }
  else s += "/";
  s += " (of ";
  if (cacheHasBeenUsed)
  {
    sprintf(h, "%d", this->getPotentialRetrievals());
    s += h;
  }
  else s += "/";
  s += "), *: ";
  sprintf(h, "%d", this->getMultiplications()); s += h;
  s += " (accumulated: ";
  sprintf(h, "%d", this->getAccumulatedMultiplications()); s += h;
  s += "), +: ";
  sprintf(h, "%d", this->getAdditions()); s += h;
  s += " (accumulated: ";
  sprintf(h, "%d", this->getAccumulatedAdditions()); s += h;
  s += "), rank: ";
  if (cacheHasBeenUsed) { sprintf(h, "%d", this->getUtility()); s += h; }
  else s += "/";
  s += "]";
  return s;
}

IntMinorValue::IntMinorValue (const IntMinorValue& mv)
{
  _result = mv.getResult();
  _retrievals = mv.getRetrievals();
  _potentialRetrievals = mv.getPotentialRetrievals();
  _multiplications = mv.getMultiplications();
  _additions = mv.getAdditions();
  _accumulatedMult = mv.getAccumulatedMultiplications();
  _accumulatedSum = mv.getAccumulatedAdditions();
}

PolyMinorValue::PolyMinorValue (const poly result, const int multiplications,
                                const int additions,
                                const int accumulatedMultiplications,
                                const int accumulatedAdditions,
                                const int retrievals,
                                const int potentialRetrievals)
{
  _result = pCopy(result);
  _multiplications = multiplications;
  _additions = additions;
  _accumulatedMult = accumulatedMultiplications;
  _accumulatedSum = accumulatedAdditions;
  _potentialRetrievals = potentialRetrievals;
  _retrievals = retrievals;
}

PolyMinorValue::PolyMinorValue ()
{
  _result = NULL;
  _multiplications = -1;
  _additions = -1;
  _accumulatedMult = -1;
  _accumulatedSum = -1;
  _potentialRetrievals = -1;
  _retrievals = -1;
}

PolyMinorValue::~PolyMinorValue()
{
  p_Delete(&_result, currRing);
}

poly PolyMinorValue::getResult() const
{
  return _result;
}

int PolyMinorValue::getWeight () const
{
  /* put measure for size of PolyMinorValue here, e.g. the number of monomials
     in the cached polynomial */
  return pLength(_result); // the number of monomials in the polynomial
}

string PolyMinorValue::toString () const
{
  char h[20];

  /* Let's see whether a cache has been used to compute this MinorValue: */
  bool cacheHasBeenUsed = true;
  if (this->getRetrievals() == -1) cacheHasBeenUsed = false;

  string s = pString(_result);
  s += " [retrievals: ";
  if (cacheHasBeenUsed) { sprintf(h, "%d", this->getRetrievals()); s += h; }
  else s += "/";
  s += " (of ";
  if (cacheHasBeenUsed)
  {
    sprintf(h, "%d", this->getPotentialRetrievals());
    s += h;
  }
  else s += "/";
  s += "), *: ";
  sprintf(h, "%d", this->getMultiplications()); s += h;
  s += " (accumulated: ";
  sprintf(h, "%d", this->getAccumulatedMultiplications()); s += h;
  s += "), +: ";
  sprintf(h, "%d", this->getAdditions()); s += h;
  s += " (accumulated: ";
  sprintf(h, "%d", this->getAccumulatedAdditions()); s += h;
  s += "), rank: ";
  if (cacheHasBeenUsed) { sprintf(h, "%d", this->getUtility()); s += h; }
  else s += "/";
  s += "]";
  return s;
}

PolyMinorValue::PolyMinorValue (const PolyMinorValue& mv)
{
  _result = pCopy(mv.getResult());
  _retrievals = mv.getRetrievals();
  _potentialRetrievals = mv.getPotentialRetrievals();
  _multiplications = mv.getMultiplications();
  _additions = mv.getAdditions();
  _accumulatedMult = mv.getAccumulatedMultiplications();
  _accumulatedSum = mv.getAccumulatedAdditions();
}

void PolyMinorValue::operator= (const PolyMinorValue& mv)
{
  if (_result != mv.getResult()) pDelete(&_result);
  _result = pCopy(mv.getResult());
  _retrievals = mv.getRetrievals();
  _potentialRetrievals = mv.getPotentialRetrievals();
  _multiplications = mv.getMultiplications();
  _additions = mv.getAdditions();
  _accumulatedMult = mv.getAccumulatedMultiplications();
  _accumulatedSum = mv.getAccumulatedAdditions();
}
