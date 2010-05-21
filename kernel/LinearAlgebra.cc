#include "mod2.h"
#include "structs.h"
#include "polys.h"
#include "ideals.h"
#include "numbers.h"
#include "matpol.h"
#include "LinearAlgebra.h"

int pivotStrategy(const number n)
{ /* bear in mind that n is guaranteed to be non-zero */
  return 0;
  
  /* in K(x): degree(denominator) - degree(numerator) -> maximal!
     in Q:    abs(n) -> maximal
     sonst:   0 */
}

bool pivot(const matrix aMat, const int r1, const int r2, const int c1,
           const int c2, int* bestR, int* bestC)
{
  int bestScore = -1; int score; poly matEntry;

  for (int c = c1; c <= c2; c++)
  {
    for (int r = r1; r <= r2; r++)
    {
      matEntry = MATELEM(aMat, r, c);
      score = (matEntry == NULL) ? -1 : pivotStrategy(pGetCoeff(matEntry));
      if (score > bestScore)
      {
        bestScore = score;
        *bestR = r;
        *bestC = c;
      }
    }
  }

  return (bestScore != -1);
}

void luDecomp(matrix &pMat, matrix &lMat, matrix uMat)
{
  int rr = uMat->rows();
  int cc = uMat->cols();
  pMat = mpNew(rr, rr);
  lMat = mpNew(rr, rr);
  /* we use an int array to store all row permutations;
     note that we only make use of the entries [1..rr] */
  int* permut = new int[rr + 1];
  for (int i = 1; i <= rr; i++) permut[i] = i;
  
  /* fill lMat with the (rr x rr) unit matrix */
  for (int r = 1; r <= rr; r++) MATELEM(lMat, r, r) = pOne();

  int bestR; int bestC; int intSwap; poly pSwap;
  for (int r = 1; r < rr; r++)
  {
    if ((r <= cc) && (pivot(uMat, r, rr, r, r, &bestR, &bestC)))
    {
      /* swap rows with indices r and bestR in permut */
      intSwap = permut[r];
      permut[r] = permut[bestR];
      permut[bestR] = intSwap;

      /* swap rows with indices r and bestR in uMat;
         it is sufficient to do this for columns >= r */
      for (int c = r; c <= cc; c++)
      {
        pSwap = MATELEM(uMat, r, c);
        MATELEM(uMat, r, c) = MATELEM(uMat, bestR, c);
        MATELEM(uMat, bestR, c) = pSwap;
      }

      /* swap rows with indices r and bestR in lMat;
         we must do this for columns < r */
      for (int c = 1; c < r; c++)
      {
        pSwap = MATELEM(lMat, r, c);
        MATELEM(lMat, r, c) = MATELEM(lMat, bestR, c);
        MATELEM(lMat, bestR, c) = pSwap;
      }

      /* perform next Gauss elimination step, i.e., below the
         row with index r; we need to adjust lMat and uMat;
         we know that the matrix entry at [r, r] is non-zero: */
      number pivotElement = pGetCoeff(MATELEM(uMat, r, r));
      poly p; number n;
      for (int rGauss = r + 1; rGauss <= rr; rGauss++)
      {
        p = MATELEM(uMat, rGauss, r);
        if (p != NULL)
        {
          n = nDiv(pGetCoeff(p), pivotElement);

          /* filling lMat;
             old entry was zero, so no need to call pDelete(.) */
          MATELEM(lMat, rGauss, r) = pNSet(nCopy(n));

          /* adjusting uMat: */
          MATELEM(uMat, rGauss, r) = NULL; pDelete(&p);
          n = nNeg(n);
          for (int cGauss = r + 1; cGauss <= cc; cGauss++)
            MATELEM(uMat, rGauss, cGauss) = pAdd(MATELEM(uMat, rGauss, cGauss),
                                                     ppMult_nn(MATELEM(uMat, r,
                                                                  cGauss), n));

          nDelete(&n); /* clean up n */
        }
      }
    }
  }

  /* building the permutation matrix from 'permut' */
  for (int r = 1; r <= rr; r++)
    MATELEM(pMat, r, permut[r]) = pOne();
  delete[] permut;

  return;
}

bool luInverse(const matrix aMat, matrix &iMat)
{ /* aMat is guaranteed to be quadratic */
  int d = aMat->rows();

  /* preparing to call luInverseFromLUDecomp */
  matrix pMat = mpNew(d, d);
  matrix lMat = mpNew(d, d);
  matrix uMat = mpNew(d, d);
  /* copy aMat to uMat: */
  for (int r = 1; r <= d; r++)
    for (int c = 1; c <= d; c++)
      MATELEM(uMat, r, c) = pCopy(aMat->m[c - 1 + (r - 1) * d]);

  luDecomp(pMat, lMat, uMat);
  bool result = luInverseFromLUDecomp(pMat, lMat, uMat, iMat);

  /* clean-up */
  idDelete((ideal*)&pMat);
  idDelete((ideal*)&lMat);
  idDelete((ideal*)&uMat);

  return result;
}

bool upperRightTriangleInverse(const matrix uMat, matrix &iMat,
                               bool diagonalIsOne)
{
  int d = uMat->rows();
  poly p; poly q;

  /* check whether uMat is invertible */
  bool invertible = diagonalIsOne;
  if (!invertible)
  {
    invertible = true;
    for (int r = 1; r <= d; r++)
    {
      if (MATELEM(uMat, r, r) == NULL)
      {
        invertible = false;
        break;
      }
    }
  }

  if (invertible)
  {
    iMat = mpNew(d, d);
    for (int r = d; r >= 1; r--)
    {
      if (diagonalIsOne)
        MATELEM(iMat, r, r) = pOne();
      else
        MATELEM(iMat, r, r) = pNSet(nInvers(pGetCoeff(MATELEM(uMat, r, r))));
      for (int c = r + 1; c <= d; c++)
      {
        p = NULL;
        for (int k = r + 1; k <= c; k++)
        {
          q = ppMult_qq(MATELEM(uMat, r, k), MATELEM(iMat, k, c));
          p = pAdd(p, q);
        }
        p = pNeg(p);
        p = pMult(p, pCopy(MATELEM(iMat, r, r)));
        MATELEM(iMat, r, c) = p;
      }
    }
  }

  return invertible;
}

bool lowerLeftTriangleInverse(const matrix lMat, matrix &iMat,
                              bool diagonalIsOne)
{
  int d = lMat->rows(); poly p; poly q;

  /* check whether lMat is invertible */
  bool invertible = diagonalIsOne;
  if (!invertible)
  {
    invertible = true;
    for (int r = 1; r <= d; r++)
    {
      if (MATELEM(lMat, r, r) == NULL)
      {
        invertible = false;
        break;
      }
    }
  }

  if (invertible)
  {
    iMat = mpNew(d, d);
    for (int c = d; c >= 1; c--)
    {
      if (diagonalIsOne)
        MATELEM(iMat, c, c) = pOne();
      else
        MATELEM(iMat, c, c) = pNSet(nInvers(pGetCoeff(MATELEM(lMat, c, c))));
      for (int r = c + 1; r <= d; r++)
      {
        p = NULL;
        for (int k = c; k <= r - 1; k++)
        {
          q = ppMult_qq(MATELEM(lMat, r, k), MATELEM(iMat, k, c));
          p = pAdd(p, q);
        }
        p = pNeg(p);
        p = pMult(p, pCopy(MATELEM(iMat, c, c)));
        MATELEM(iMat, r, c) = p;
      }
    }
  }

  return invertible;
}

bool luInverseFromLUDecomp(const matrix pMat, const matrix lMat, const matrix uMat, matrix &iMat)
{ /* uMat is guaranteed to be quadratic */
  int d = uMat->rows();

  matrix lMinus1Mat; /* for storing the inverse of lMat */
  matrix uMinus1Mat; /* for storing the inverse of uMat, if existing */

  bool result = upperRightTriangleInverse(uMat, uMinus1Mat, false);
  if (result)
  {
    lowerLeftTriangleInverse(lMat, lMinus1Mat, true);
    iMat = mpMult(mpMult(uMinus1Mat, lMinus1Mat), pMat);
    
    /* clean-up */
    idDelete((ideal*)&lMinus1Mat);
    idDelete((ideal*)&uMinus1Mat);
  }

  return result;
}
