#ifndef TEST_MINORS_H
#define TEST_MINORS_H

#include "Minor.h"

void minorUsageInfo();
int testIntMinors (const int dummy);
int* randomMatrix(const int rowCount, const int columnCount, const int randomSeed,
                  const int zeroPercentage, const int entryBound);
void testOneMinor(std::string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                  int minorSize, int randomSeed, int cacheEntries, int cacheWeight);
void testAllMinors(std::string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                   int minorRows, int minorColumns, int minorSize, int randomSeed, int cacheEntries, int cacheWeight);
int testAllPolyMinors(matrix m, int minorSize, int cacheEntries, int cacheWeight, int strategies,
                      int dumpMinors, int dumpResults, int dumpComplete, int dumpConsole);
void testAllMinorsUntil(std::string testHeader, int rowCount, int columnCount, int entryBound, int zeroPercentage,
                        int minorSize, int randomSeed, int cacheEntries, int cacheWeight, int targetMinor,
                        bool checkForEquality, int maxLoops);
void testStuff (const poly p);
void writeMinor(const std::string);
void writeMinorWithResults(const std::string);
void writeMinorWithResults(const long l);
void writeMinor(const int i);
void writeMinorWithResults(const int i);
void writeTheMinorIfNonZero(const PolyMinorValue& pmv);
void writeSep(const std::string s);
void writeSep3(const std::string s, const int i, const std::string t);

#endif
/* TEST_MINORS_H */
