


#include "kernel/mod2.h"

// include before anything to avoid clashes with stdio.h included elsewhere
// #include <cstdio>

#include "kernel/linear_algebra/MinorInterface.h"
#include "kernel/linear_algebra/MinorProcessor.h"

#include "polys/simpleideals.h"
#include "coeffs/modulop.h" // for NV_MAX_PRIME

#include "kernel/polys.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/ideals.h"

using namespace std;

/* returns true iff the given polyArray has only number entries;
   if so, the int's corresponding to these numbers will be written
   into intArray[0..(length-1)];
   the method assumes that both polyArray and intArray have valid
   entries for the indices 0..(length-1);
   after the call, zeroCounter contains the number of zero entries
   in the matrix */
bool arrayIsNumberArray (const poly* polyArray, const ideal iSB,
                         const int length, int* intArray,
                         poly* nfPolyArray, int& zeroCounter)
{
  int n = 0; if (currRing != 0) n = currRing->N;
  int characteristic = 0; if (currRing != 0) characteristic = rChar(currRing);
  zeroCounter = 0;
  bool result = true;

  for (int i = 0; i < length; i++)
  {
    nfPolyArray[i] = pCopy(polyArray[i]);
    if (iSB != NULL)
    {
      poly tmp = kNF(iSB, currRing->qideal, nfPolyArray[i]);
      pDelete(&nfPolyArray[i]);
      nfPolyArray[i]=tmp;
    }
    if (nfPolyArray[i] == NULL)
    {
      intArray[i] = 0;
      zeroCounter++;
    }
    else
    {
      bool isConstant = true;
      for (int j = 1; j <= n; j++)
        if (pGetExp(nfPolyArray[i], j) > 0)
          isConstant = false;
      if (!isConstant) result = false;
      else
      {
        intArray[i] = n_Int(pGetCoeff(nfPolyArray[i]), currRing->cf);
        if (characteristic != 0) intArray[i] = intArray[i] % characteristic;
        if (intArray[i] == 0) zeroCounter++;
      }
    }
  }
  return result;
}

/* special implementation for the case that the matrix has only number entries;
   if i is not the zero pointer, then it is assumed to contain a std basis, and
   the number entries of the matrix are then assumed to be reduced w.r.t. i and
   modulo the characteristic of the gound field/ring;
   this method should also work when currRing == null, i.e. when no ring has
   been declared */
ideal getMinorIdeal_Int (const int* intMatrix, const int rowCount,
                         const int columnCount, const int minorSize,
                         const int k, const char* algorithm,
                         const ideal i, const bool allDifferent)
{
  /* setting up a MinorProcessor for matrices with integer entries: */
  IntMinorProcessor mp;
  mp.defineMatrix(rowCount, columnCount, intMatrix);
  int *myRowIndices=(int*)omAlloc(rowCount*sizeof(int));
  for (int j = 0; j < rowCount; j++) myRowIndices[j] = j;
  int *myColumnIndices=(int*)omAlloc(columnCount*sizeof(int));
  for (int j = 0; j < columnCount; j++) myColumnIndices[j] = j;
  mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
  mp.setMinorSize(minorSize);

  /* containers for all upcoming results: */
  IntMinorValue theMinor;
  // int value = 0;
  int collectedMinors = 0;
  int characteristic = 0; if (currRing != 0) characteristic = rChar(currRing);

  /* the ideal to be returned: */
  ideal iii = idInit(1);

  bool zeroOk = ((k < 0) ? true : false); /* for k = 0, all minors are requested,
                                             omitting zero minors */
  bool duplicatesOk = (allDifferent ? false : true);
  int kk = ABS(k); /* absolute value of k */

  /* looping over all minors: */
  while (mp.hasNextMinor() && ((kk == 0) || (collectedMinors < kk)))
  {
    /* retrieving the next minor: */
    theMinor = mp.getNextMinor(characteristic, i, algorithm);
    poly f = NULL;
    if (theMinor.getResult() != 0) f = pISet(theMinor.getResult());
    if (idInsertPolyWithTests(iii, collectedMinors, f, zeroOk, duplicatesOk))
      collectedMinors++;
  }

  /* before we return the result, let's omit zero generators
     in iii which come after the computed minors */
  ideal jjj;
  if (collectedMinors == 0) jjj = idInit(1);
  else                      jjj = idCopyFirstK(iii, collectedMinors);
  idDelete(&iii);
  omFree(myColumnIndices);
  omFree(myRowIndices);
  return jjj;
}

/* special implementation for the case that the matrix has non-number,
   i.e., actual polynomial entries;
   if i is not the zero pointer than it is assumed to be a std basis (ideal),
   and the poly matrix is assumed to be already reduced w.r.t. i */
ideal getMinorIdeal_Poly (const poly* polyMatrix, const int rowCount,
                          const int columnCount, const int minorSize,
                          const int k, const char* algorithm,
                          const ideal i, const bool allDifferent)
{
  /* setting up a MinorProcessor for matrices with polynomial entries: */
  PolyMinorProcessor mp;
  mp.defineMatrix(rowCount, columnCount, polyMatrix);
  int *myRowIndices=(int*)omAlloc(rowCount*sizeof(int));
  for (int j = 0; j < rowCount; j++) myRowIndices[j] = j;
  int *myColumnIndices=(int*)omAlloc(columnCount*sizeof(int));
  for (int j = 0; j < columnCount; j++) myColumnIndices[j] = j;
  mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
  mp.setMinorSize(minorSize);

  /* containers for all upcoming results: */
  PolyMinorValue theMinor;
  poly f = NULL;
  int collectedMinors = 0;

  /* the ideal to be returned: */
  ideal iii = idInit(1);

  bool zeroOk = ((k < 0) ? true : false); /* for k = 0, all minors are
                                             requested, omitting zero minors */
  bool duplicatesOk = (allDifferent ? false : true);
  int kk = ABS(k); /* absolute value of k */
#ifdef COUNT_AND_PRINT_OPERATIONS
  printCounters ("starting", true);
  int qqq = 0;
#endif
  /* looping over all minors: */
  while (mp.hasNextMinor() && ((kk == 0) || (collectedMinors < kk)))
  {
    /* retrieving the next minor: */
    theMinor = mp.getNextMinor(algorithm, i);
#if (defined COUNT_AND_PRINT_OPERATIONS) && (COUNT_AND_PRINT_OPERATIONS > 1)
    qqq++;
    Print("after %d", qqq);
    printCounters ("-th minor", false);
#endif
    f = theMinor.getResult();
    if (idInsertPolyWithTests(iii, collectedMinors, pCopy(f),
                              zeroOk, duplicatesOk))
      collectedMinors++;
  }
#ifdef COUNT_AND_PRINT_OPERATIONS
  printCounters ("ending", true);
#endif

  /* before we return the result, let's omit zero generators
     in iii which come after the computed minors */
  idKeepFirstK(iii, collectedMinors);
  omFree(myColumnIndices);
  omFree(myRowIndices);
  return(iii);
}

ideal getMinorIdeal_toBeDone (const matrix mat, const int minorSize,
                              const int k, const char* algorithm,
                              const ideal i, const bool allDifferent)
{
  int rowCount = mat->nrows;
  int columnCount = mat->ncols;
  poly* myPolyMatrix = (poly*)(mat->m);
  ideal iii; /* the ideal to be filled and returned */
  int zz = 0;

  /* divert to special implementations for pure number matrices and actual
     polynomial matrices: */
  int*  myIntMatrix  = (int*)omAlloc(rowCount * columnCount *sizeof(int));
  poly* nfPolyMatrix = (poly*)omAlloc(rowCount * columnCount *sizeof(poly));
  if (arrayIsNumberArray(myPolyMatrix, i, rowCount * columnCount,
                         myIntMatrix, nfPolyMatrix, zz))
    iii = getMinorIdeal_Int(myIntMatrix, rowCount, columnCount, minorSize, k,
                            algorithm, i, allDifferent);
  else
  {
    if ((k == 0) && (strcmp(algorithm, "Bareiss") == 0)
        && (!rField_is_Z(currRing)) && (!allDifferent))
    {
      /* In this case, we call an optimized procedure, dating back to
         Wilfried Pohl. It may be used whenever
         - all minors are requested,
         - requested minors need not be mutually distinct, and
         - coefficients come from a field (i.e., Z is also not allowed
           for this implementation). */
      iii = (i == 0 ? id_Minors(mat, minorSize, currRing)
                    : id_Minors(mat, minorSize, currRing, i));
    }
    else
    {
      iii = getMinorIdeal_Poly(nfPolyMatrix, rowCount, columnCount, minorSize,
                               k, algorithm, i, allDifferent);
    }
  }

  /* clean up */
  omFree(myIntMatrix);
  for (int j = 0; j < rowCount * columnCount; j++) pDelete(&nfPolyMatrix[j]);
  omFree(nfPolyMatrix);

  return iii;
}

/* When called with algorithm == "Bareiss", the coefficients are assumed
   to come from a field or from a ring which does not have zero-divisors
   (other than 0), i.e. from an integral domain.
   E.g. Bareiss may be used over fields or over Z but not over
        Z/6 (which has non-zero zero divisors, namely 2 and 3). */
ideal getMinorIdeal (const matrix mat, const int minorSize, const int k,
                     const char* algorithm, const ideal iSB,
                     const bool allDifferent)
{
  /* Note that this method should be replaced by getMinorIdeal_toBeDone,
     to enable faster computations in the case of matrices which contain
     only numbers. But so far, this method is not yet usable as it replaces
     the numbers by ints which may result in overflows during computations
     of minors. */
  int rowCount = mat->nrows;
  int columnCount = mat->ncols;
  poly* myPolyMatrix = (poly*)(mat->m);
  int length = rowCount * columnCount;
  ideal iii; /* the ideal to be filled and returned */

  if ((k == 0) && (strcmp(algorithm, "Bareiss") == 0)
      && (!rField_is_Ring(currRing)) && (!allDifferent))
  {
    /* In this case, we call an optimized procedure, dating back to
       Wilfried Pohl. It may be used whenever
       - all minors are requested,
       - requested minors need not be mutually distinct, and
       - coefficients come from a field (i.e., the ring Z is not
         allowed for this implementation). */
    iii = (iSB == NULL ? id_Minors(mat, minorSize, currRing)
                       : id_Minors(mat, minorSize, currRing, iSB));
  }
  else
  {
  /* copy all polynomials and reduce them w.r.t. iSB
     (if iSB is present, i.e., not the NULL pointer) */

    poly* nfPolyMatrix = (poly*)omAlloc(length*sizeof(poly));
    if (iSB != NULL)
    {
      for (int i = 0; i < length; i++)
      {
        nfPolyMatrix[i] = kNF(iSB, currRing->qideal,myPolyMatrix[i]);
      }
    }
    else
    {
      for (int i = 0; i < length; i++)
      {
        nfPolyMatrix[i] = pCopy(myPolyMatrix[i]);
      }
    }
    iii = getMinorIdeal_Poly(nfPolyMatrix, rowCount, columnCount, minorSize,
                             k, algorithm, iSB, allDifferent);

    /* clean up */
    for (int j = length-1; j>=0; j--) pDelete(&nfPolyMatrix[j]);
    omFree(nfPolyMatrix);
  }

  return iii;
}

/* special implementation for the case that the matrix has only number entries;
   if i is not the zero pointer, then it is assumed to contain a std basis, and
   the number entries of the matrix are then assumed to be reduced w.r.t. i and
   modulo the characteristic of the gound field/ring;
   this method should also work when currRing == null, i.e. when no ring has
   been declared */
ideal getMinorIdealCache_Int(const int* intMatrix, const int rowCount,
                             const int columnCount, const int minorSize,
                             const int k, const ideal i,
                             const int cacheStrategy, const int cacheN,
                             const int cacheW, const bool allDifferent)
{
  /* setting up a MinorProcessor for matrices with integer entries: */
  IntMinorProcessor mp;
  mp.defineMatrix(rowCount, columnCount, intMatrix);
  int *myRowIndices=(int*)omAlloc(rowCount*sizeof(int));
  for (int j = 0; j < rowCount; j++) myRowIndices[j] = j;
  int *myColumnIndices=(int*)omAlloc(columnCount*sizeof(int));
  for (int j = 0; j < columnCount; j++) myColumnIndices[j] = j;
  mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
  mp.setMinorSize(minorSize);
  MinorValue::SetRankingStrategy(cacheStrategy);
  Cache<MinorKey, IntMinorValue> cch(cacheN, cacheW);

  /* containers for all upcoming results: */
  IntMinorValue theMinor;
  // int value = 0;
  int collectedMinors = 0;
  int characteristic = 0; if (currRing != 0) characteristic = rChar(currRing);

  /* the ideal to be returned: */
  ideal iii = idInit(1);

  bool zeroOk = ((k < 0) ? true : false); /* for k = 0, all minors are
                                             requested, omitting zero minors */
  bool duplicatesOk = (allDifferent ? false : true);
  int kk = ABS(k); /* absolute value of k */

  /* looping over all minors: */
  while (mp.hasNextMinor() && ((kk == 0) || (collectedMinors < kk)))
  {
    /* retrieving the next minor: */
    theMinor = mp.getNextMinor(cch, characteristic, i);
    poly f = NULL;
    if (theMinor.getResult() != 0) f = pISet(theMinor.getResult());
    if (idInsertPolyWithTests(iii, collectedMinors, f, zeroOk, duplicatesOk))
      collectedMinors++;
  }

  /* before we return the result, let's omit zero generators
     in iii which come after the computed minors */
  ideal jjj;
  if (collectedMinors == 0) jjj = idInit(1);
  else                      jjj = idCopyFirstK(iii, collectedMinors);
  idDelete(&iii);
  omFree(myColumnIndices);
  omFree(myRowIndices);
  return jjj;
}

/* special implementation for the case that the matrix has non-number,
   i.e. real poly entries;
   if i is not the zero pointer, then it is assumed to contain a std basis,
   and the entries of the matrix are then assumed to be reduced w.r.t. i */
ideal getMinorIdealCache_Poly(const poly* polyMatrix, const int rowCount,
                              const int columnCount, const int minorSize,
                              const int k, const ideal i,
                              const int cacheStrategy, const int cacheN,
                              const int cacheW, const bool allDifferent)
{
  /* setting up a MinorProcessor for matrices with polynomial entries: */
  PolyMinorProcessor mp;
  mp.defineMatrix(rowCount, columnCount, polyMatrix);
  int *myRowIndices=(int*)omAlloc(rowCount*sizeof(int));
  for (int j = 0; j < rowCount; j++) myRowIndices[j] = j;
  int *myColumnIndices=(int*)omAlloc(columnCount*sizeof(int));
  for (int j = 0; j < columnCount; j++) myColumnIndices[j] = j;
  mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
  mp.setMinorSize(minorSize);
  MinorValue::SetRankingStrategy(cacheStrategy);
  Cache<MinorKey, PolyMinorValue> cch(cacheN, cacheW);

  /* containers for all upcoming results: */
  PolyMinorValue theMinor;
  poly f = NULL;
  int collectedMinors = 0;

  /* the ideal to be returned: */
  ideal iii = idInit(1);

  bool zeroOk = ((k < 0) ? true : false); /* for k = 0, all minors are
                                             requested, omitting zero minors */
  bool duplicatesOk = (allDifferent ? false : true);
  int kk = ABS(k); /* absolute value of k */
#ifdef COUNT_AND_PRINT_OPERATIONS
  printCounters ("starting", true);
  int qqq = 0;
#endif
  /* looping over all minors: */
  while (mp.hasNextMinor() && ((kk == 0) || (collectedMinors < kk)))
  {
    /* retrieving the next minor: */
    theMinor = mp.getNextMinor(cch, i);
#if (defined COUNT_AND_PRINT_OPERATIONS) && (COUNT_AND_PRINT_OPERATIONS > 1)
    qqq++;
    Print("after %d", qqq);
    printCounters ("-th minor", false);
#endif
    f = theMinor.getResult();
    if (idInsertPolyWithTests(iii, collectedMinors, pCopy(f), zeroOk,
                              duplicatesOk))
      collectedMinors++;
  }
#ifdef COUNT_AND_PRINT_OPERATIONS
  printCounters ("ending", true);
#endif

  /* before we return the result, let's omit zero generators
     in iii which come after the computed minors */
  ideal jjj;
  if (collectedMinors == 0) jjj = idInit(1);
  else                      jjj = idCopyFirstK(iii, collectedMinors);
  idDelete(&iii);
  omFree(myColumnIndices);
  omFree(myRowIndices);
  return jjj;
}

ideal getMinorIdealCache_toBeDone (const matrix mat, const int minorSize,
                                   const int k, const ideal iSB,
                                   const int cacheStrategy, const int cacheN,
                                   const int cacheW, const bool allDifferent)
{
  int rowCount = mat->nrows;
  int columnCount = mat->ncols;
  poly* myPolyMatrix = (poly*)(mat->m);
  ideal iii; /* the ideal to be filled and returned */
  int zz = 0;

  /* divert to special implementation when myPolyMatrix has only number
     entries: */
  int*  myIntMatrix  = (int*)omAlloc(rowCount * columnCount *sizeof(int));
  poly* nfPolyMatrix = (poly*)omAlloc(rowCount * columnCount *sizeof(poly));
  if (arrayIsNumberArray(myPolyMatrix, iSB, rowCount * columnCount,
                         myIntMatrix, nfPolyMatrix, zz))
    iii = getMinorIdealCache_Int(myIntMatrix, rowCount, columnCount,
                                 minorSize, k, iSB, cacheStrategy, cacheN,
                                 cacheW, allDifferent);
  else
    iii = getMinorIdealCache_Poly(nfPolyMatrix, rowCount, columnCount,
                                  minorSize, k, iSB, cacheStrategy, cacheN,
                                  cacheW, allDifferent);

  /* clean up */
  omFree(myIntMatrix);
  for (int j = 0; j < rowCount * columnCount; j++) pDelete(&nfPolyMatrix[j]);
  omFree(nfPolyMatrix);

  return iii;
}

ideal getMinorIdealCache (const matrix mat, const int minorSize, const int k,
                          const ideal iSB, const int cacheStrategy,
                          const int cacheN, const int cacheW,
                          const bool allDifferent)
{
  /* Note that this method should be replaced by getMinorIdealCache_toBeDone,
     to enable faster computations in the case of matrices which contain
     only numbers. But so far, this method is not yet usable as it replaces
     the numbers by ints which may result in overflows during computations
     of minors. */
  int rowCount = mat->nrows;
  int columnCount = mat->ncols;
  poly* myPolyMatrix = (poly*)(mat->m);
  int length = rowCount * columnCount;
  poly* nfPolyMatrix = (poly*)omAlloc(length*sizeof(poly));
  ideal iii; /* the ideal to be filled and returned */

  /* copy all polynomials and reduce them w.r.t. iSB
     (if iSB is present, i.e., not the NULL pointer) */
  for (int i = 0; i < length; i++)
  {
    if (iSB==NULL)
      nfPolyMatrix[i] = pCopy(myPolyMatrix[i]);
    else
      nfPolyMatrix[i] = kNF(iSB, currRing->qideal, myPolyMatrix[i]);
  }

  iii = getMinorIdealCache_Poly(nfPolyMatrix, rowCount, columnCount,
                                minorSize, k, iSB, cacheStrategy,
                                cacheN, cacheW, allDifferent);

  /* clean up */
  for (int j = 0; j < length; j++) pDelete(&nfPolyMatrix[j]);
  omFree(nfPolyMatrix);

  return iii;
}

ideal getMinorIdealHeuristic (const matrix mat, const int minorSize,
                              const int k, const ideal iSB,
                              const bool allDifferent)
{
  int vars = currRing->N;

  /* here comes the heuristic, as of 29 January 2010:

     integral domain and minorSize <= 2                -> Bareiss
     integral domain and minorSize >= 3 and vars <= 2  -> Bareiss
     field case and minorSize >= 3 and vars = 3
       and c in {2, 3, ..., 32749}                     -> Bareiss

     otherwise:
     if not all minors are requested                   -> Laplace, no Caching
     otherwise:
     minorSize >= 3 and vars <= 4 and
       (rowCount over minorSize)*(columnCount over minorSize) >= 100
                                                       -> Laplace with Caching
     minorSize >= 3 and vars >= 5 and
       (rowCount over minorSize)*(columnCount over minorSize) >= 40
                                                       -> Laplace with Caching

     otherwise:                                        -> Laplace, no Caching
  */

  bool b = false; /* Bareiss */
  bool l = false; /* Laplace without caching */
  // bool c = false; /* Laplace with caching */
  if (rField_is_Domain(currRing))
  { /* the field case or ring Z */
    if      (minorSize <= 2)                                     b = true;
    else if (vars <= 2)                                          b = true;
    else if ((!rField_is_Ring(currRing)) && (vars == 3)
             && (currRing->cf->ch >= 2) && (currRing->cf->ch <= NV_MAX_PRIME))
          b = true;
  }
  if (!b)
  { /* the non-Bareiss cases */
    if (k != 0) /* this means, not all minors are requested */   l = true;
    else
    { /* k == 0, i.e., all minors are requested */
      l = true;
    }
  }

  if      (b)    return getMinorIdeal(mat, minorSize, k, "Bareiss", iSB,
                                      allDifferent);
  else if (l)    return getMinorIdeal(mat, minorSize, k, "Laplace", iSB,
                                      allDifferent);
  else /* (c) */ return getMinorIdealCache(mat, minorSize, k, iSB,
                                      3, 200, 100000, allDifferent);
}
