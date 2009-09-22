#include "mod2.h"
#include "structs.h"
#include "polys.h"
#include "matpol.h"
#include <Cache.h>
#include <Minor.h>
#include <MinorProcessor.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "TestMinors.h"
#include <stdio.h>

using namespace std;

bool writeMinorOutputToFiles = false;
bool writeMinorOutputToConsole = false;
bool writeMinorOutputToResults = false;
bool writeMinorOutputToComplete = false;
bool writeNonZeroMinorsToFile = false;

/**
* The file name for complete output (as the content of the console);<br>
* This file will be constructed in the same path where the executable resides.
*/
char* filenameForCompleteOutput = "minor_output_complete.txt";

/**
* The file name for condensed result output, only;<br>
* This file will be constructed in the same path where the executable resides.
*/
char* filenameForResultOutput = "minor_output_results.txt";

char* filenameForNonZeroMinors = "non_zero_minors.txt";
poly zeroPoly = pISet(0);
int nonZeroCounter = 0;

/**
* An ofstream for writing a file with the same content as the console
*/
ofstream outputFileComplete;

/**
* An ofstream for writing a file with condensed results, only
*/
ofstream outputFileResults;

ofstream outputFileMinors;

void minorUsageInfo()
{
  std::cout << "\nWrite 'system(\"minors\", 0)' to run 5 default tests with a random";
  std::cout << "\ninteger matrix. This test, for which no ring needs to be declared,";
  std::cout << "\nwill generate two files including all results and runtimes (in the";
  std::cout << "\nfolder of the SINGULAR executable.";
  std::cout << "\n\nWrite 'system(m, k, nCache, wCache, strategies";
  std::cout << "\n              dumpMinors, dumpResults, dumpComplete, dumpConsole)'";
  std::cout << "\nto compute all k x k minors of the poly matrix m. Depending on the int";
  std::cout << "\n'strategies', e.g. = 420, these tests will first be performed without a";
  std::cout << "\ncache (due to lowest digit '0') and afterwards with a cache, deploying";
  std::cout << "\nthe strategies 2 and 4. The cache has a maximum number of cached entries";
  std::cout << "\nof 'nCache' and a maximum weight of 'wCache'. (The latter parameter";
  std::cout << "\nrelates to the total number of monomials in the cached polynomials.)";
  std::cout << "\nNote that a ring needs to be defined beforehand, and the poly matrix m.";
  std::cout << "\nSet 'dumpMinors' to 1 in order to write all non-zero minors to a file.";
  std::cout << "\nWith 'dumpResults' equal to 1, an additional file will be written";
  std::cout << "\nwhich contains an overview over all results and runtimes. Set";
  std::cout << "\n'dumpComplete' to 1 in order to obtain a very detailed file output of";
  std::cout << "\nall results (All files will be created in the folder of the SINGULAR";
  std::cout << "\nexecutable.)";
  std::cout << "\nSet 'dumpConsole' to 1 in order to write everything also to the console.\n";
}

/**
* A method for obtaining a random matrix with specified properties.
* The returned structure is 1-dimensional, i.e., the matrix from
* top to bottom, left to right.
*/
int* randomMatrix(const int rowCount, const int columnCount, const int randomSeed,
                  const int zeroPercentage, const int entryBound) {
    int theSize = rowCount * columnCount;
    int theMatrix[theSize]; // we will use a 1-dimensional structure
    srand(randomSeed); // random seed for ensuring reproducability of experiments
    for (int i = 0; i < theSize; i++) {
        if ((rand() % 100) < zeroPercentage)
            theMatrix[i] = 0;
        else
            theMatrix[i] = 1 + (rand() % entryBound); // ranges from 1 to entryBound, including both
    }
    return theMatrix;
}

void writeTheMinorIfNonZero(const PolyMinorValue& pmv) {
    if (!pEqualPolys(pmv.getResult(), zeroPoly))
    {
      nonZeroCounter++;
      if (writeNonZeroMinorsToFile)
      {
        outputFileMinors << std::endl << nonZeroCounter << ". " << pString(pmv.getResult());
      }
    }
}

void writeSep(const string s) {
  outputFileMinors << s;
}

void writeSep3(const string s, const int i, const string t)
{
  outputFileMinors << s << i << t;
}

/**
* A method for simultaneously writing an output string to std::cout and the output file
* specified by \c filenameForCompleteOutput.
* @param s the string to be written
*/
void writeMinor(const string s) {
    if (writeMinorOutputToConsole) std::cout << s;
    if (writeMinorOutputToComplete) outputFileComplete << s;
}

/**
* A method for simultaneously writing an output string to std::cout and the output files
* specified by \c filenameForCompleteOutput and \c filenameForResultOutput.
* @param s the string to be written
*/
void writeMinorWithResults(const string s) {
    if (writeMinorOutputToConsole) std::cout << s;
    if (writeMinorOutputToComplete) outputFileComplete << s;
    if (writeMinorOutputToResults) outputFileResults << s;
}

/**
* A method for simultaneously writing an output string to std::cout and the output files
* specified by \c filenameForCompleteOutput and \c filenameForResultOutput.
* @param s the string to be written
*/
void writeMinorWithResults(const long l) {
    if (writeMinorOutputToConsole) std::cout << l;
    if (writeMinorOutputToComplete) outputFileComplete << l;
    if (writeMinorOutputToResults) outputFileResults << l;
}

/**
* A method for simultaneously writing an integer to std::cout and the output file
* specified by \c filenameForCompleteOutput.
* @param i the integer to be written
*/
void writeMinor(const int i) {
    if (writeMinorOutputToConsole) std::cout << i;
    if (writeMinorOutputToComplete) outputFileComplete << i;
}

/**
* A method for simultaneously writing an integer to std::cout and the output files
* specified by \c filenameForCompleteOutput and \c filenameForResultOutput.
* @param i the integer to be written
*/
void writeMinorWithResults(const int i) {
    if (writeMinorOutputToConsole) std::cout << i;
    if (writeMinorOutputToComplete) outputFileComplete << i;
    if (writeMinorOutputToResults) outputFileResults << i;
}

/**
* A method for testing the implementation.<br>
* All intermediate and final results will be printed to both the console and
* into files with the names specified by filenameForCompleteOutput and
* filenameForResultOutput, which both reside in the path of the compiled executable.
* @param argc not used
* @param *argv[] not used
*/
int testIntMinors (const int dummy) {
  writeMinorOutputToFiles = true;

  filenameForCompleteOutput = "minor_output_complete_longs.txt";
  filenameForResultOutput = "minor_output_results_longs.txt";

  if (writeMinorOutputToFiles)
  {
    outputFileComplete.open(filenameForCompleteOutput);
    outputFileResults.open(filenameForResultOutput);
  }

  // computes just one minor:
  testOneMinor("Test I", 7, 10, 50, 20, 5, 471, 70, 1000);
  writeMinor("\n\n");

  // computes all (1470) minors of a specified size (6x6):
  testAllMinors("Test II", 7, 10, 50, 20, 7, 10, 6, 471, 200, 10000);
  writeMinor("\n\n");

  // looks for minor == 3000; to this end, it needs to compute 1001 minors:
  testAllMinorsUntil("Test III", 100, 60, 10, 10, 6, 471, 300, 10000, 3000, true, 10000);
  writeMinor("\n\n");

  // looks for the first non-zero minor (6x6); to this end, it needs to compute 476 minors:
  testAllMinorsUntil("Test IV", 100, 60, 10, 75, 6, 4712, 300, 10000, 0, false, 10000);
  writeMinor("\n\n");

  // looks for minor == 265986; to this end, it needs to compute 16 minors:
  testAllMinorsUntil("Test V", 100, 60, 10, 10, 6, 471, 300, 10000, 265986, true, 10000);

  if (writeMinorOutputToFiles)
  {
    outputFileComplete.close();
    outputFileResults.close();
  }

  return 0;
}

void testStuff (const poly p)
{
  std::cout << std::endl << "poly = " << pString(p);
  std::cout << std::endl << "length of poly = " << pLength(p);
}

int testAllPolyMinors(matrix mat, int minorSize, int cacheEntries, int cacheWeight, int strategies,
                      int dumpMinors, int dumpResults, int dumpComplete, int dumpConsole) {
  writeNonZeroMinorsToFile = (dumpMinors == 1 ? true : false);
  writeMinorOutputToResults = (dumpResults == 1 ? true : false);
  writeMinorOutputToComplete = (dumpComplete == 1 ? true : false);
  writeMinorOutputToConsole = (dumpConsole == 1 ? true : false);

  filenameForCompleteOutput = "minor_output_complete_polys.txt";
  filenameForResultOutput = "minor_output_results_polys.txt";
  filenameForNonZeroMinors = "non_zero_poly_minors.txt";

  if (writeMinorOutputToComplete) outputFileComplete.open(filenameForCompleteOutput);
  if (writeMinorOutputToResults) outputFileResults.open(filenameForResultOutput);
  if (writeNonZeroMinorsToFile) outputFileMinors.open(filenameForNonZeroMinors);

  int rowCount = mat->nrows;
  int columnCount = mat->ncols;
  long totalTimeStart, totalTime, printTimeStart, printTime;
  string testHeader = "COMPUTE ALL MINORS IN A POLY MATRIX";

  writeMinor(testHeader);
  writeMinor("\n"); for (int i = 0; i < int(testHeader.size()); i++) writeMinor("="); // underlines the header string
  writeMinor("\nTesting the successive computation of all minors of a given size without and with cache, respectively.");
  writeMinor("\nIn the case of computing with cache, different caching strategies may be deployed.");
  writeMinor("\nThe provided matrix is expected to have SINGULAR polys as entries.");

  poly* myPolyMatrix = (poly*)(mat->m);
  PolyMinorProcessor mp = PolyMinorProcessor();
  mp.defineMatrix(rowCount, columnCount, myPolyMatrix);

  /* The next lines are for defining the sub-matrix of myPolyMatrix
     from which we want to compute all k x k - minors.
     In the given setting, we want the entire matrix to form the sub-matrix. */
  int minorRows = rowCount;
  int minorColumns = columnCount;
  int myRowIndices[minorRows];  for (int i = 0; i < minorRows; i++) myRowIndices[i] = i;
  int myColumnIndices[minorColumns]; for (int i = 0; i < minorColumns; i++) myColumnIndices[i] = i;

  // setting sub-matrix and size of minors of interest within that sub-matrix:
  mp.defineSubMatrix(minorRows, myRowIndices, minorColumns, myColumnIndices);
  mp.setMinorSize(minorSize);

  // define the cache:
  Cache<MinorKey, PolyMinorValue> cch(cacheEntries, cacheWeight);

  // container for all upcoming results
  PolyMinorValue theMinor;

  // counters...
  int k = 1;
  int totalMultiplications = 0;
  int totalSummations = 0;
  int totalMultiplicationsAccumulated = 0;
  int totalSummationsAccumulated = 0;

  // target for retrieving and writing momentary row and column indices:
  int myIndexArray[32];

  if (strategies % 10 == 0)
  {
    strategies = strategies / 10;
    writeMinor("\n\nResults - "); writeMinor(testHeader); writeMinor(" - no cache");
    writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);

    writeSep("non-zero minors - no cache\n==========================");
    nonZeroCounter = 0;
  
    printTime = 0; totalTimeStart = clock();
    // iteration over all minors of size "minorSize x minorSize"
    while (mp.hasNextMinor()) {
        // retrieving the minor:
        theMinor = mp.getNextMinor();
        printTimeStart = clock();
        writeTheMinorIfNonZero(theMinor);

        // updating counters:
        totalMultiplications += theMinor.getMultiplications();
        totalSummations += theMinor.getSummations();
        totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
        totalSummationsAccumulated += theMinor.getAccumulatedSummations();
  
        // writing current row indices:
        mp.getCurrentRowIndices(myIndexArray);
        writeMinor("\n"); writeMinor(k++); writeMinor(". minor (rows: ");
        for (int i = 0; i < minorSize; i++) {
            if (i != 0) writeMinor(", ");
            writeMinor(myIndexArray[i]);
        };
  
        // writing current column indices:
        mp.getCurrentColumnIndices(myIndexArray);
        writeMinor("; columns: ");
        for (int i = 0; i < minorSize; i++) {
            if (i != 0) writeMinor(", ");
            writeMinor(myIndexArray[i]);
        };
        writeMinor(") = ");
  
        // write the actual value of the minor:
        writeMinor(theMinor.toString());
        printTime += clock() - printTimeStart;
    };
    totalTime = clock() - totalTimeStart - printTime;

    // writing summarized information
    writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
    writeMinorWithResults(" - no cache");
    writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
    writeMinorWithResults(" multiplications and ");
    writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
    writeMinorWithResults("\nnumber of non-zero minors = "); writeMinorWithResults(nonZeroCounter);
    writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");
  }

  for (int strategy = 1; strategy <= 5; strategy++) {
    if (strategies % 10 == strategy)
    {
      strategies = strategies / 10;
      // setting sub-matrix, size of minors of interest within that sub-matrix, and strategy:
      mp.defineSubMatrix(minorRows, myRowIndices, minorColumns, myColumnIndices);
      mp.setMinorSize(minorSize);
      MinorValue::SetRankingStrategy(strategy);
  
      // counters...
      k = 1;
      totalMultiplications = 0;
      totalSummations = 0;
      totalMultiplicationsAccumulated = 0;
      totalSummationsAccumulated = 0;
  
      // cleaning up and redefinition of the cache:
      cch.clear();
      Cache<MinorKey, PolyMinorValue> cch(cacheEntries, cacheWeight);
  
      writeMinor("\n\nResults - "); writeMinor(testHeader); writeMinor(" - using cache - deploying caching strategy #"); writeMinor(strategy);
      writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);
  
      writeSep3("\n\nnon-zero minors - using cache - deploying caching strategy #", strategy,
                  "\n=============================================================");
      nonZeroCounter = 0;
  
      printTime = 0; totalTimeStart = clock();
      // iteration over all minors of size "minorSize x minorSize"
      while (mp.hasNextMinor()) {
          // retrieving the minor:
          theMinor = mp.getNextMinor(cch);
          printTimeStart = clock();
          writeTheMinorIfNonZero(theMinor);
  
          // updating counters:
          totalMultiplications += theMinor.getMultiplications();
          totalSummations += theMinor.getSummations();
          totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
          totalSummationsAccumulated += theMinor.getAccumulatedSummations();
  
          // writing current row indices:
          mp.getCurrentRowIndices(myIndexArray);
          writeMinor("\n"); writeMinor(k++); writeMinor(". minor (rows: ");
          for (int i = 0; i < minorSize; i++) {
              if (i != 0) writeMinor(", ");
              writeMinor(myIndexArray[i]);
          };
  
          // writing current column indices:
          mp.getCurrentColumnIndices(myIndexArray);
          writeMinor("; columns: ");
          for (int i = 0; i < minorSize; i++) {
              if (i != 0) writeMinor(", ");
              writeMinor(myIndexArray[i]);
          };
          writeMinor(") = ");
  
          // writeMinor the actual value of the minor:
          writeMinor(theMinor.toString());
          printTime += clock() - printTimeStart;
      };
      totalTime = clock() - totalTimeStart - printTime;
  
      // writing summarized information
      writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
      writeMinorWithResults(" - using cache - deploying caching strategy #");
      writeMinorWithResults(strategy);
      writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
      writeMinorWithResults(" multiplications and ");
      writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
      writeMinor("\n(computation without reuse would need "); writeMinor(totalMultiplicationsAccumulated);
      writeMinor(" and "); writeMinor(totalSummationsAccumulated); writeMinor(" summations)");
      writeMinorWithResults("\nnumber of non-zero minors = "); writeMinorWithResults(nonZeroCounter);
      writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");
      writeMinor("\nThe cache looks like this:\n"); writeMinor(cch.toString());
    }
  }

  if (writeMinorOutputToComplete) outputFileComplete.close();
  if (writeMinorOutputToResults) outputFileResults.close();
  if (writeNonZeroMinorsToFile) outputFileMinors.close();

  return 0;
}

/**
* A method for testing the computation of one minor; without and with cache, respectively.<br>
* All results should be equal no matter whether we do or do not use a cache. Neither should
* the cache strategy influence the mathematical value of the minor.
*/
void testOneMinor(string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                  int minorSize, int randomSeed, int cacheEntries, int cacheWeight) {
    long start, end;

    writeMinor(testHeader);
    writeMinor("\n"); for (int i = 0; i < int(testHeader.size()); i++) writeMinor("="); // underlines the header string
    writeMinor("\nTesting the computation of one minor without and with cache, respectively.");
    writeMinor("\nFor computing with cache, 5 different caching strategies will be deployed.");

    int* myMatrix = randomMatrix(rowCount, columnCount, randomSeed, zeroPercentage, entryBound);

    LongMinorProcessor mp = LongMinorProcessor();
    mp.defineMatrix(rowCount, columnCount, myMatrix);

    int myRowIndices[minorSize];  for (int i = 0; i < minorSize; i++) myRowIndices[i] = i;
    int myColumnIndices[minorSize]; for (int i = 0; i < minorSize; i++) myColumnIndices[i] = columnCount - minorSize + i;

    // We would like to printout mp. For that, we need to provide complete information of
    // what minors we intend to do compute later on.
    // The next two lines of code have just this purpose and nothing more:
    mp.defineSubMatrix(minorSize, myRowIndices, minorSize, myColumnIndices);
    mp.setMinorSize(minorSize);

    writeMinor("\n\n"); writeMinor(mp.toString()); writeMinor("\n\n");

    // compute the minor without cache:
    writeMinorWithResults("Results - "); writeMinorWithResults(testHeader); writeMinorWithResults(" - no cache");
    start = clock();
    LongMinorValue mv = mp.getMinor(minorSize, myRowIndices, myColumnIndices);
    end = clock();
    writeMinorWithResults("\nvalue of minor = "); writeMinorWithResults(mv.toString());
    writeMinorWithResults("\n(time = "); writeMinorWithResults(end - start); writeMinorWithResults(" msec)");

    // define the cache:
    Cache<MinorKey, LongMinorValue> cch = Cache<MinorKey, LongMinorValue>(cacheEntries, cacheWeight);

    // compute minor using the cache, for all implemented caching strategies:
    for (int strategy = 1; strategy <= 5; strategy++) {
        // clear cache:
        cch.clear();
        mp.defineSubMatrix(minorSize, myRowIndices, minorSize, myColumnIndices);

        // compute the minor using the cache and current strategy
        LongMinorValue::SetRankingStrategy(strategy);
        start = clock();
        mv = mp.getMinor(minorSize, myRowIndices, myColumnIndices, cch);
        end = clock();

        writeMinorWithResults("\n\nResults - "); writeMinorWithResults(testHeader);
        writeMinorWithResults(" - using cache - deploying caching strategy #"); writeMinorWithResults(strategy);
        writeMinorWithResults("\nvalue of minor = "); writeMinorWithResults(mv.toString());
        writeMinorWithResults("\n(time = "); writeMinorWithResults(end - start); writeMinorWithResults(" msec)");
        writeMinor("\nThe cache looks like this:\n"); writeMinor(cch.toString());
    }
}

/**
* A method for testing the computation of all minors of a given size within a pre-defined
* sub-matrix of an underlying matrix.<br>
* Again, we do this first without cache, and later using a cache, respectively.<br>
* All results should be equal no matter whether we do or do not use a cache. Neither should the cache strategy
* influence the mathematical value of the minor.
*/
void testAllMinors(string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                   int minorRows, int minorColumns, int minorSize, int randomSeed, int cacheEntries, int cacheWeight) {
    long totalTimeStart, totalTime, printTimeStart, printTime;

    writeMinor(testHeader);
    writeMinor("\n"); for (int i = 0; i < int(testHeader.size()); i++) writeMinor("="); // underlines the header string
    writeMinor("\nTesting the successive computation of all minors of a given size without and with cache, respectively.");
    writeMinor("\nIn the case of computing with cache, 5 different caching strategies will be deployed.");

    int* myMatrix = randomMatrix(rowCount, columnCount, randomSeed, zeroPercentage, entryBound);

    LongMinorProcessor mp = LongMinorProcessor();
    mp.defineMatrix(rowCount, columnCount, myMatrix);

    int myRowIndices[minorRows];  for (int i = 0; i < minorRows; i++) myRowIndices[i] = i;
    int myColumnIndices[minorColumns]; for (int i = 0; i < minorColumns; i++) myColumnIndices[i] = columnCount - minorColumns + i;

    // setting sub-matrix and size of minors of interest within that sub-matrix:
    mp.defineSubMatrix(minorRows, myRowIndices, minorColumns, myColumnIndices);
    mp.setMinorSize(minorSize);

    writeMinor("\n\n"); writeMinor(mp.toString());

    // define the cache:
    Cache<MinorKey, LongMinorValue> cch = Cache<MinorKey, LongMinorValue>(cacheEntries, cacheWeight);

    // container for all upcoming results
    LongMinorValue theMinor;

    // counters...
    int k = 1;
    int totalMultiplications = 0;
    int totalSummations = 0;
    int totalMultiplicationsAccumulated = 0;
    int totalSummationsAccumulated = 0;

    // target for retrieving and writing momentary row and column indices:
    int myIndexArray[32];

    writeMinor("\n\nResults - "); writeMinor(testHeader); writeMinor(" - no cache");
    writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);

    printTime = 0; totalTimeStart = clock();
    // iteration over all minors of size "minorSize x minorSize"
    while (mp.hasNextMinor()) {
        // retrieving the minor:
        theMinor = mp.getNextMinor();
        printTimeStart = clock();

        // updating counters:
        totalMultiplications += theMinor.getMultiplications();
        totalSummations += theMinor.getSummations();
        totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
        totalSummationsAccumulated += theMinor.getAccumulatedSummations();

        // writing current row indices:
        mp.getCurrentRowIndices(myIndexArray);
        writeMinor("\n"); writeMinor(k++); writeMinor(". minor (rows: ");
        for (int i = 0; i < minorSize; i++) {
            if (i != 0) writeMinor(", ");
            writeMinor(myIndexArray[i]);
        };

        // writing current column indices:
        mp.getCurrentColumnIndices(myIndexArray);
        writeMinor("; columns: ");
        for (int i = 0; i < minorSize; i++) {
            if (i != 0) writeMinor(", ");
            writeMinor(myIndexArray[i]);
        };
        writeMinor(") = ");

        // write the actual value of the minor:
        writeMinor(theMinor.toString());
        printTime += clock() - printTimeStart;
    };
    totalTime = clock() - totalTimeStart - printTime;
    // writing summarized information
    writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
    writeMinorWithResults(" - no cache");
    writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
    writeMinorWithResults(" multiplications and ");
    writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
    writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");

    for (int strategy = 1; strategy <= 5; strategy++) {
        // setting sub-matrix, size of minors of interest within that sub-matrix, and strategy:
        mp.defineSubMatrix(minorRows, myRowIndices, minorColumns, myColumnIndices);
        mp.setMinorSize(minorSize);
        LongMinorValue::SetRankingStrategy(strategy);

        // counters...
        k = 1;
        totalMultiplications = 0;
        totalSummations = 0;
        totalMultiplicationsAccumulated = 0;
        totalSummationsAccumulated = 0;

        // cleaning up and redefinition of the cache:
        cch.clear();
        cch = Cache<MinorKey, LongMinorValue>(cacheEntries, cacheWeight);

        writeMinor("\n\nResults - "); writeMinor(testHeader); writeMinor(" - using cache - deploying caching strategy #"); writeMinor(strategy);
        writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);

        printTime = 0; totalTimeStart = clock();
        // iteration over all minors of size "minorSize x minorSize"
        while (mp.hasNextMinor()) {
            // retrieving the minor:
            theMinor = mp.getNextMinor(cch);
            printTimeStart = clock();

            // updating counters:
            totalMultiplications += theMinor.getMultiplications();
            totalSummations += theMinor.getSummations();
            totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
            totalSummationsAccumulated += theMinor.getAccumulatedSummations();

            // writing current row indices:
            mp.getCurrentRowIndices(myIndexArray);
            writeMinor("\n"); writeMinor(k++); writeMinor(". minor (rows: ");
            for (int i = 0; i < minorSize; i++) {
                if (i != 0) writeMinor(", ");
                writeMinor(myIndexArray[i]);
            };

            // writing current column indices:
            mp.getCurrentColumnIndices(myIndexArray);
            writeMinor("; columns: ");
            for (int i = 0; i < minorSize; i++) {
                if (i != 0) writeMinor(", ");
                writeMinor(myIndexArray[i]);
            };
            writeMinor(") = ");

            // writeMinor the actual value of the minor:
            writeMinor(theMinor.toString());
            printTime += clock() - printTimeStart;
        };
        totalTime = clock() - totalTimeStart - printTime;
        // writing summarized information
        writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
        writeMinorWithResults(" - using cache - deploying caching strategy #");
        writeMinorWithResults(strategy);
        writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
        writeMinorWithResults(" multiplications and ");
        writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
        writeMinor("\n(computation without reuse would need "); writeMinor(totalMultiplicationsAccumulated);
        writeMinor(" and "); writeMinor(totalSummationsAccumulated); writeMinor(" summations)");
        writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");
        writeMinor("\nThe cache looks like this:\n"); writeMinor(cch.toString());
    }
}

/**
* A method for testing the computation of all minors of a given size within a pre-defined
* sub-matrix of an underlying large matrix.<br>
* Again, we do this first without cache, and later using a cache, respectively.<br>
* All results should be equal no matter whether we do or do not use a cache. Neither should the cache strategy
* influence the mathematical value of the minor.
* zeroP: this is the probability for zero entries in the matrix;
*        all other matrix entries will range from 1 to entryBound and be equally distributed
*/
void testAllMinorsUntil(string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                        int minorSize, int randomSeed, int cacheEntries, int cacheWeight, int targetMinor,
                        bool checkForEquality, int maxLoops) {
    long totalTimeStart, totalTime, printTimeStart, printTime;

    writeMinor(testHeader);
    writeMinor("\n"); for (int i = 0; i < int(testHeader.size()); i++) writeMinor("="); // underlines the header string
    writeMinor("\nTesting the successive computation of minors of a given size without and with ");
    writeMinor("\ncache, respectively, until a minor with a certain value is found.");
    writeMinor("\nIn the case of computing with cache, 5 different caching strategies will be deployed.");

    writeMinor("\n\nIn this test, the matrix is "); writeMinor(rowCount); writeMinor(" x "); writeMinor(columnCount); writeMinor(".");
    writeMinor("\nThe minor we are looking for is "); writeMinor(minorSize); writeMinor(" x "); writeMinor(minorSize); writeMinor(", and ");
    writeMinor("is supposed to have a value of ");
    if (!checkForEquality) writeMinor("<> ");
    writeMinor(targetMinor); writeMinor(".");
    writeMinor("\nAs an upper bound for the number of loops, at most "); writeMinor(maxLoops); writeMinor(" minors will be computed.");

    int* myMatrix = randomMatrix(rowCount, columnCount, randomSeed, zeroPercentage, entryBound);

    LongMinorProcessor mp = LongMinorProcessor();
    mp.defineMatrix(rowCount, columnCount, myMatrix);

    int myRowIndices[rowCount]; for (int i = 0; i < rowCount; i++) myRowIndices[i] = i; // choosing all rows
    int myColumnIndices[columnCount]; for (int i = 0; i < columnCount; i++) myColumnIndices[i] = i; // choosing all columns

    // define the cache:
    Cache<MinorKey, LongMinorValue> cch = Cache<MinorKey, LongMinorValue>(cacheEntries, cacheWeight);

    // container for all upcoming results
    LongMinorValue theMinor;

    // counters...
    int k = 1;
    int totalMultiplications = 0;
    int totalSummations = 0;
    int totalMultiplicationsAccumulated = 0;
    int totalSummationsAccumulated = 0;

    // setting sub-matrix and size of minors of interest within that sub-matrix:
    mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
    mp.setMinorSize(minorSize);

    writeMinor("\n\n"); writeMinor(mp.toString());

    writeMinor("\n\nResults - "); writeMinor(testHeader); writeMinor(" - no cache");
    writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);
    writeMinor(" until first minor with specified value is found:");

    bool minorFound = false;
    int loops = 0;
    printTime = 0; totalTimeStart = clock();
    // iteration over all minors of size "minorSize x minorSize"
    while (mp.hasNextMinor() && (!minorFound) && (loops < maxLoops)) {

        // retrieving the minor:
        theMinor = mp.getNextMinor();
        printTimeStart = clock();
        minorFound = (checkForEquality ? (theMinor.getResult() == targetMinor) : (theMinor.getResult() != targetMinor));

        // updating counters:
        totalMultiplications += theMinor.getMultiplications();
        totalSummations += theMinor.getSummations();
        totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
        totalSummationsAccumulated += theMinor.getAccumulatedSummations();

        // writing current minor
        writeMinor("\n"); writeMinor(k++); writeMinor(". minor = "); writeMinor(theMinor.getResult());

        loops++;
        printTime += clock() - printTimeStart;
    };
    totalTime = clock() - totalTimeStart - printTime;
    // writing summarized information
    writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
    writeMinorWithResults(" - no cache");
    writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
    writeMinorWithResults(" multiplications and ");
    writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
    writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");

    for (int strategy = 1; strategy <= 5; strategy++) {
        // setting sub-matrix, size of minors of interest within that sub-matrix, and strategy:
        mp.defineSubMatrix(rowCount, myRowIndices, columnCount, myColumnIndices);
        mp.setMinorSize(minorSize);
        LongMinorValue::SetRankingStrategy(strategy);

        // counters...
        k = 1;
        totalMultiplications = 0;
        totalSummations = 0;
        totalMultiplicationsAccumulated = 0;
        totalSummationsAccumulated = 0;

        // cleaning up and redefinition of the cache:
        cch.clear();
        cch = Cache<MinorKey, LongMinorValue>(cacheEntries, cacheWeight);

        writeMinor("\n\nResults - "); writeMinor(testHeader);
        writeMinor(" - using cache - deploying caching strategy #"); writeMinor(strategy);
        writeMinor("\ncomputing all minors of size "); writeMinor(minorSize); writeMinor("x"); writeMinor(minorSize);
        writeMinor(" until first minor with specified value is found:");

        int loops = 0;
        bool minorFound = false;
        printTime = 0; totalTimeStart = clock();
        // iteration over all minors of size "minorSize x minorSize"
        while (mp.hasNextMinor() && (!minorFound) && (loops < maxLoops)) {

            // retrieving the minor:
            theMinor = mp.getNextMinor(cch);
            printTimeStart = clock();
            minorFound = (checkForEquality ? (theMinor.getResult() == targetMinor) : (theMinor.getResult() != targetMinor));

            // updating counters:
            totalMultiplications += theMinor.getMultiplications();
            totalSummations += theMinor.getSummations();
            totalMultiplicationsAccumulated += theMinor.getAccumulatedMultiplications();
            totalSummationsAccumulated += theMinor.getAccumulatedSummations();

            // writing current minor
            writeMinor("\n"); writeMinor(k++); writeMinor(". minor = "); writeMinor(theMinor.getResult());

            loops++;
            printTime += clock() - printTimeStart;
        };
        totalTime = clock() - totalTimeStart - printTime;
        // writing summarized information
        writeMinorWithResults("\n\nOperation counters - "); writeMinorWithResults(testHeader);
        writeMinorWithResults(" - using cache - deploying caching strategy #");
        writeMinorWithResults(strategy);
        writeMinorWithResults("\nperformed in total "); writeMinorWithResults(totalMultiplications);
        writeMinorWithResults(" multiplications and ");
        writeMinorWithResults(totalSummations); writeMinorWithResults(" summations");
        writeMinor("\n(computation without reuse would need "); writeMinor(totalMultiplicationsAccumulated);
        writeMinor(" and "); writeMinor(totalSummationsAccumulated); writeMinor(" summations)");
        writeMinorWithResults("\n(time = "); writeMinorWithResults(totalTime); writeMinorWithResults(" msec)");
        writeMinor("\nThe cache has "); writeMinor(cch.getNumberOfEntries()); writeMinor(" (of max. ");
        writeMinor(cacheEntries); writeMinor(") entries and a weight of ");
        writeMinor(cch.getWeight()); writeMinor(" (of max. ");
        writeMinor(cacheWeight); writeMinor(").");
    }
}
