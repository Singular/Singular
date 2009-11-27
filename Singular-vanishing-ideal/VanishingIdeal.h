#ifndef VANISHING_IDEAL_H
#define VANISHING_IDEAL_H

#ifdef HAVE_VANISHING_IDEAL

ideal gBForVanishingIdealDirect (const bool printOperationDetails);
ideal gBForVanishingIdealRecursive (const bool printOperationDetails);
poly normalForm (const poly f, const bool printOperationDetails);
int smarandache (const int m, const bool printOperationDetails); // expects m >= 1
bool isZeroFunction (const poly f, const bool printOperationDetails);

#endif
/* HAVE_VANISHING_IDEAL */

#endif
/* VANISHING_IDEAL_H */