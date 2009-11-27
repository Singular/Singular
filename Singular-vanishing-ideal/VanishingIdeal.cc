#include "mod2.h"

#ifdef HAVE_VANISHING_IDEAL

#include "structs.h"
#include "polys.h"
#include "ideals.h"
#include "VanishingIdeal.h"
#include "FactoredNumber.h"

using namespace std;

ideal gBForVanishingIdealDirect (const bool printOperationDetails)
{
  ideal iii = idInit(1, 0);
  idInsertPoly(iii, pISet(17)); // will include po only if it is not the zero polynomial
  idSkipZeroes(iii);  // remove zero generators (resulting from block-wise allocation of memory)
  return iii;
}

ideal gBForVanishingIdealRecursive (const bool printOperationDetails)
{
  ideal iii = idInit(1, 0);
  idInsertPoly(iii, pISet(3)); // will include po only if it is not the zero polynomial
  idSkipZeroes(iii);  // remove zero generators (resulting from block-wise allocation of memory)
  return iii;
}

poly normalForm (const poly f, const bool printOperationDetails)
{
  poly p = NULL;
  return p;
}

bool isZeroFunction (const poly f, const bool printOperationDetails)
{
  printf("\nFactoredNumber(%d) = %s\n", 1, FactoredNumber(1).toString());
  printf("\nFactoredNumber(%d) = %s\n", 5, FactoredNumber(5).toString());
  printf("\nFactoredNumber(%d) = %s\n", 10, FactoredNumber(10).toString());
  printf("\nFactoredNumber(%d) = %s\n", 50, FactoredNumber(50).toString());
  printf("\nFactoredNumber(%d) = %s\n", 128, FactoredNumber(128).toString());
  printf("\nFactoredNumber(%d) = %s\n", -1, FactoredNumber(2, 64).toString());
  return false;
}

/* expects a >= 1, b >= 1 */
int helper_gcd (const int a, const int b)
{
  if ((a == 1) || (b == 1)) return 1;
  int m = (a > b) ? a : b;
  int n = (a > b) ? b : a;
  int temp = 0;
  /* m >= n */
  while ((n != 1) && (n != 0))
  {
    temp = m - n;
    m = (temp > n) ? temp : n;
    n = (temp > n) ? n : temp;
  }
  if (n == 1) return 1;
  if (n == 0) return m;
}

/* expects m >= 1 */
int smarandache (const int m, const bool printOperationDetails)
{
  /* naive algorithm */
  int lambda = 1;
  int factorial = 1;
  int mm = m;
  int temp = 0;
  int multCounter = 0;
  int gcdCounter = 0;
  int divCounter = 0;
  while (factorial % mm != 0)
  {
    lambda++;
    factorial = factorial * lambda; multCounter++;
    temp = helper_gcd(mm, factorial); gcdCounter++;
    mm = mm / temp; divCounter++;
    factorial = factorial / temp; divCounter++;
  }
  if (printOperationDetails)
  {
    printf("\nperformed operations:\n   int-int-multiplications: %d,\n   int-int-gcd's: %d,\n   int-int-divisions: %d.\n\n",
           multCounter, gcdCounter, divCounter);
  }
  return lambda;
}

#endif
/* HAVE_VANISHING_IDEAL */
