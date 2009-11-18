#ifndef TEST_MINORS_H
#define TEST_MINORS_H

#ifdef HAVE_MINOR

#include "Minor.h"
#include "PrettyPrinter.h"

void minorUsageInfo();
int testIntMinors (const int dummy);
int testAllPolyMinors(matrix m, int minorSize, int strategies, int cacheEntries, int cacheWeight,
                      int dumpMinors, int dumpResults, int dumpComplete, int dumpConsole);
ideal testAllPolyMinorsAsIdeal(matrix m, int minorSize, int strategy, int cacheEntries, int cacheWeight);
ideal testAllIntMinorsAsIdeal(matrix m, int minorSize, int strategy, int cacheEntries, int cacheWeight, int characteristic);
void testStuff (const poly p);

#endif // HAVE_MINOR

#endif
/* TEST_MINORS_H */
