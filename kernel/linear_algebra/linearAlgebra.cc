/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file lineareAlgebra.cc
 *
 * This file implements basic linear algebra functionality.
 *
 * For more general information, see the documentation in
 * lineareAlgebra.h.
 *
 * @author Frank Seelisch
 *
 *
 **/
/*****************************************************************************/

// include header files



#include "kernel/mod2.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "coeffs/mpr_complex.h"
#include "polys/monomials/p_polys.h"
#include "polys/simpleideals.h"
#include "polys/matpol.h"

// #include "kernel/polys.h"

#include "kernel/structs.h"
#include "kernel/ideals.h"
#include "kernel/linear_algebra/linearAlgebra.h"

/**
 * The returned score is based on the implementation of 'nSize' for
 * numbers (, see numbers.h): nSize(n) provides a measure for the
 * complexity of n. Thus, less complex pivot elements will be
 * preferred, and get therefore a smaller pivot score. Consequently,
 * we simply return the value of nSize.
 * An exception to this rule are the ground fields R, long R, and
 * long C: In these, the result of nSize relates to |n|. And since
 * a larger modulus of the pivot element ensures a numerically more
 * stable Gauss elimination, we would like to have a smaller score
 * for larger values of |n|. Therefore, in R, long R, and long C,
 * the negative of nSize will be returned.
 **/
int pivotScore(number n, const ring r)
{
  int s = n_Size(n, r->cf);
  if (rField_is_long_C(r) ||
      rField_is_long_R(r) ||
      rField_is_R(r))
    return -s;
  else
    return s;
}

/**
 * This code computes a score for each non-zero matrix entry in
 * aMat[r1..r2, c1..c2]. If all entries are zero, false is returned,
 * otherwise true. In the latter case, the minimum of all scores
 * is sought, and the row and column indices of the corresponding
 * matrix entry are stored in bestR and bestC.
 **/
bool pivot(const matrix aMat, const int r1, const int r2, const int c1,
           const int c2, int* bestR, int* bestC, const ring R)
{
  int bestScore; int score; bool foundBestScore = false; poly matEntry;

  for (int c = c1; c <= c2; c++)
  {
    for (int r = r1; r <= r2; r++)
    {
      matEntry = MATELEM(aMat, r, c);
      if (matEntry != NULL)
      {
        score = pivotScore(pGetCoeff(matEntry), R);
        if ((!foundBestScore) || (score < bestScore))
        {
          bestScore = score;
          *bestR = r;
          *bestC = c;
        }
        foundBestScore = true;
      }
    }
  }

  return foundBestScore;
}

bool unitMatrix(const int n, matrix &unitMat, const ring R)
{
  if (n < 1) return false;
  unitMat = mpNew(n, n);
  for (int r = 1; r <= n; r++) MATELEM(unitMat, r, r) = p_One(R);
  return true;
}

void luDecomp(const matrix aMat, matrix &pMat, matrix &lMat, matrix &uMat,
              const ring R)
{
  int rr = aMat->rows();
  int cc = aMat->cols();
  pMat = mpNew(rr, rr);
  uMat = mp_Copy(aMat,R); /* copy aMat into uMat: */

  /* we use an int array to store all row permutations;
     note that we only make use of the entries [1..rr] */
  int* permut = new int[rr + 1];
  for (int i = 1; i <= rr; i++) permut[i] = i;

  /* fill lMat with the (rr x rr) unit matrix */
  unitMatrix(rr, lMat,R);

  int bestR; int bestC; int intSwap; poly pSwap; int cOffset = 0;
  for (int r = 1; r < rr; r++)
  {
    if (r > cc) break;
    while ((r + cOffset <= cc) &&
           (!pivot(uMat, r, rr, r + cOffset, r + cOffset, &bestR, &bestC, R)))
      cOffset++;
    if (r + cOffset <= cc)
    {
      /* swap rows with indices r and bestR in permut */
      intSwap = permut[r];
      permut[r] = permut[bestR];
      permut[bestR] = intSwap;

      /* swap rows with indices r and bestR in uMat;
         it is sufficient to do this for columns >= r + cOffset*/
      for (int c = r + cOffset; c <= cc; c++)
      {
        pSwap = MATELEM(uMat, r, c);
        MATELEM(uMat, r, c) = MATELEM(uMat, bestR, c);
        MATELEM(uMat, bestR, c) = pSwap;
      }

      /* swap rows with indices r and bestR in lMat;
         we must do this only for columns < r */
      for (int c = 1; c < r; c++)
      {
        pSwap = MATELEM(lMat, r, c);
        MATELEM(lMat, r, c) = MATELEM(lMat, bestR, c);
        MATELEM(lMat, bestR, c) = pSwap;
      }

      /* perform next Gauss elimination step, i.e., below the
         row with index r;
         we need to adjust lMat and uMat;
         we are certain that the matrix entry at [r, r + cOffset]
         is non-zero: */
      number pivotElement = pGetCoeff(MATELEM(uMat, r, r + cOffset));
      poly p;
      for (int rGauss = r + 1; rGauss <= rr; rGauss++)
      {
        p = MATELEM(uMat, rGauss, r + cOffset);
        if (p != NULL)
        {
          number n = n_Div(pGetCoeff(p), pivotElement, R->cf);
          n_Normalize(n,R->cf);

          /* filling lMat;
             old entry was zero, so no need to call pDelete(.) */
          MATELEM(lMat, rGauss, r) = p_NSet(n_Copy(n,R->cf),R);

          /* adjusting uMat: */
          MATELEM(uMat, rGauss, r + cOffset) = NULL; p_Delete(&p,R);
          n = n_InpNeg(n,R->cf);
          for (int cGauss = r + cOffset + 1; cGauss <= cc; cGauss++)
          {
            MATELEM(uMat, rGauss, cGauss)
              = p_Add_q(MATELEM(uMat, rGauss, cGauss),
                     pp_Mult_nn(MATELEM(uMat, r, cGauss), n, R), R);
            p_Normalize(MATELEM(uMat, rGauss, cGauss),R);
          }

          n_Delete(&n,R->cf); /* clean up n */
        }
      }
    }
  }

  /* building the permutation matrix from 'permut' */
  for (int r = 1; r <= rr; r++)
    MATELEM(pMat, r, permut[r]) = p_One(R);
  delete[] permut;

  return;
}

/**
 * This code first computes the LU-decomposition of aMat,
 * and then calls the method for inverting a matrix which
 * is given by its LU-decomposition.
 **/
bool luInverse(const matrix aMat, matrix &iMat, const ring R)
{ /* aMat is guaranteed to be an (n x n)-matrix */
  matrix pMat;
  matrix lMat;
  matrix uMat;
  luDecomp(aMat, pMat, lMat, uMat, R);
  bool result = luInverseFromLUDecomp(pMat, lMat, uMat, iMat, R);

  /* clean-up */
  id_Delete((ideal*)&pMat,R);
  id_Delete((ideal*)&lMat,R);
  id_Delete((ideal*)&uMat,R);

  return result;
}

/* Assumes that aMat is already in row echelon form */
int rankFromRowEchelonForm(const matrix aMat)
{
  int rank = 0;
  int rr = aMat->rows(); int cc = aMat->cols();
  int r = 1; int c = 1;
  while ((r <= rr) && (c <= cc))
  {
    if (MATELEM(aMat, r, c) == NULL) c++;
    else { rank++; r++; }
  }
  return rank;
}

int luRank(const matrix aMat, const bool isRowEchelon, const ring R)
{
  if (isRowEchelon) return rankFromRowEchelonForm(aMat);
  else
  { /* compute the LU-decomposition and read off the rank from
       the upper triangular matrix of that decomposition */
    matrix pMat;
    matrix lMat;
    matrix uMat;
    luDecomp(aMat, pMat, lMat, uMat,R);
    int result = rankFromRowEchelonForm(uMat);

    /* clean-up */
    id_Delete((ideal*)&pMat,R);
    id_Delete((ideal*)&lMat,R);
    id_Delete((ideal*)&uMat,R);

    return result;
  }
}

bool upperRightTriangleInverse(const matrix uMat, matrix &iMat,
                               bool diagonalIsOne, const ring R)
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
        MATELEM(iMat, r, r) = p_One(R);
      else
        MATELEM(iMat, r, r) = p_NSet(n_Invers(p_GetCoeff(MATELEM(uMat, r, r),R),R->cf),R);
      for (int c = r + 1; c <= d; c++)
      {
        p = NULL;
        for (int k = r + 1; k <= c; k++)
        {
          q = pp_Mult_qq(MATELEM(uMat, r, k), MATELEM(iMat, k, c),R);
          p = p_Add_q(p, q,R);
        }
        p = p_Neg(p,R);
        p = p_Mult_q(p, p_Copy(MATELEM(iMat, r, r),R),R);
        p_Normalize(p,R);
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
        pNormalize(p);
        MATELEM(iMat, r, c) = p;
      }
    }
  }

  return invertible;
}

/**
 * This code computes the inverse by inverting lMat and uMat, and
 * then performing two matrix multiplications.
 **/
bool luInverseFromLUDecomp(const matrix pMat, const matrix lMat,
                           const matrix uMat, matrix &iMat, const ring R)
{ /* uMat is guaranteed to be quadratic */
  //int d = uMat->rows();

  matrix lMatInverse; /* for storing the inverse of lMat;
                         this inversion will always work                */
  matrix uMatInverse; /* for storing the inverse of uMat, if invertible */

  bool result = upperRightTriangleInverse(uMat, uMatInverse, false);
  if (result)
  {
    /* next will always work, since lMat is known to have all diagonal
       entries equal to 1 */
    lowerLeftTriangleInverse(lMat, lMatInverse, true);
    iMat = mp_Mult(mp_Mult(uMatInverse, lMatInverse,R), pMat,R);

    /* clean-up */
    idDelete((ideal*)&lMatInverse);
    idDelete((ideal*)&uMatInverse);
  }

  return result;
}

bool luSolveViaLUDecomp(const matrix pMat, const matrix lMat,
                        const matrix uMat, const matrix bVec,
                        matrix &xVec, matrix &H)
{
  int m = uMat->rows(); int n = uMat->cols();
  matrix cVec = mpNew(m, 1);  /* for storing pMat * bVec */
  matrix yVec = mpNew(m, 1);  /* for storing the unique solution of
                                 lMat * yVec = cVec */

  /* compute cVec = pMat * bVec but without actual multiplications */
  for (int r = 1; r <= m; r++)
  {
    for (int c = 1; c <= m; c++)
    {
      if (MATELEM(pMat, r, c) != NULL)
        { MATELEM(cVec, r, 1) = pCopy(MATELEM(bVec, c, 1)); break; }
    }
  }

  /* solve lMat * yVec = cVec; this will always work as lMat is invertible;
     moreover, no divisions are needed, since lMat[i, i] = 1, for all i */
  for (int r = 1; r <= m; r++)
  {
    poly p = pNeg(pCopy(MATELEM(cVec, r, 1)));
    for (int c = 1; c < r; c++)
      p = pAdd(p, ppMult_qq(MATELEM(lMat, r, c), MATELEM(yVec, c, 1) ));
    MATELEM(yVec, r, 1) = pNeg(p);
    pNormalize(MATELEM(yVec, r, 1));
  }

  /* determine whether uMat * xVec = yVec is solvable */
  bool isSolvable = true;
  bool isZeroRow;
  int nonZeroRowIndex = 0 ;   // handle case that the matrix is zero
  for (int r = m; r >= 1; r--)
  {
    isZeroRow = true;
    for (int c = 1; c <= n; c++)
      if (MATELEM(uMat, r, c) != NULL) { isZeroRow = false; break; }
    if (isZeroRow)
    {
      if (MATELEM(yVec, r, 1) != NULL) { isSolvable = false; break; }
    }
    else { nonZeroRowIndex = r; break; }
  }

  if (isSolvable)
  {
    xVec = mpNew(n, 1);
    matrix N = mpNew(n, n); int dim = 0;
    poly p; poly q;
    /* solve uMat * xVec = yVec and determine a basis of the solution
       space of the homogeneous system uMat * xVec = 0;
       We do not know in advance what the dimension (dim) of the latter
       solution space will be. Thus, we start with the possibly too wide
       matrix N and later copy the relevant columns of N into H. */
    int nonZeroC  =  0 ;
    int lastNonZeroC = n + 1;

    for (int r = nonZeroRowIndex; r >= 1; r--)
    {
      for (nonZeroC = 1; nonZeroC <= n; nonZeroC++)
        if (MATELEM(uMat, r, nonZeroC) != NULL) break;

      for (int w = lastNonZeroC - 1; w >= nonZeroC + 1; w--)
      {
        /* this loop will only be done when the given linear system has
           more than one, i.e., infinitely many solutions */
        dim++;
        /* now we fill two entries of the dim-th column of N */
        MATELEM(N, w, dim) = pNeg(pCopy(MATELEM(uMat, r, nonZeroC)));
        MATELEM(N, nonZeroC, dim) = pCopy(MATELEM(uMat, r, w));
      }
      for (int d = 1; d <= dim; d++)
      {
        /* here we fill the entry of N at [r, d], for each valid vector
           that we already have in N;
           again, this will only be done when the given linear system has
           more than one, i.e., infinitely many solutions */
        p = NULL;
        for (int c = nonZeroC + 1; c <= n; c++)
          if (MATELEM(N, c, d) != NULL)
            p = pAdd(p, ppMult_qq(MATELEM(uMat, r, c), MATELEM(N, c, d)));
        q = pNSet(nInvers(pGetCoeff(MATELEM(uMat, r, nonZeroC))));
        MATELEM(N, nonZeroC, d) = pMult(pNeg(p), q);
        pNormalize(MATELEM(N, nonZeroC, d));
      }
      p = pNeg(pCopy(MATELEM(yVec, r, 1)));
      for (int c = nonZeroC + 1; c <= n; c++)
        if (MATELEM(xVec, c, 1) != NULL)
          p = pAdd(p, ppMult_qq(MATELEM(uMat, r, c), MATELEM(xVec, c, 1)));
      q = pNSet(nInvers(pGetCoeff(MATELEM(uMat, r, nonZeroC))));
      MATELEM(xVec, nonZeroC, 1) = pMult(pNeg(p), q);
      pNormalize(MATELEM(xVec, nonZeroC, 1));
      lastNonZeroC = nonZeroC;
    }
    for (int w = lastNonZeroC - 1; w >= 1; w--)
    {
       // remaining variables are free
       dim++;
       MATELEM(N, w, dim) = pOne();
    }

    if (dim == 0)
    {
      /* that means the given linear system has exactly one solution;
         we return as H the 1x1 matrix with entry zero */
      H = mpNew(1, 1);
    }
    else
    {
      /* copy the first 'dim' columns of N into H */
      H = mpNew(n, dim);
      for (int r = 1; r <= n; r++)
        for (int c = 1; c <= dim; c++)
          MATELEM(H, r, c) = pCopy(MATELEM(N, r, c));
    }
    /* clean up N */
    idDelete((ideal*)&N);
  }
  idDelete((ideal*)&cVec);
  idDelete((ideal*)&yVec);

  return isSolvable;
}

/* for debugging:
   for printing numbers to the console
   DELETE LATER */
void printNumber(const number z)
{
  if (nIsZero(z)) printf("number = 0\n");
  else
  {
    poly p = pOne();
    pSetCoeff(p, nCopy(z));
    pSetm(p);
    printf("number = %s\n", pString(p));
    pDelete(&p);
  }
}

/* for debugging:
   for printing matrices to the console
   DELETE LATER */
void printMatrix(const matrix m)
{
  int rr = MATROWS(m); int cc = MATCOLS(m);
  printf("\n-------------\n");
  for (int r = 1; r <= rr; r++)
  {
    for (int c = 1; c <= cc; c++)
      printf("%s  ", pString(MATELEM(m, r, c)));
    printf("\n");
  }
  printf("-------------\n");
}

/**
 * Creates a new complex number from real and imaginary parts given
 * by doubles.
 *
 * @return the new complex number
 **/
number complexNumber(const double r, const double i)
{
  gmp_complex* n= new gmp_complex(r, i);
  return (number)n;
}

/**
 * Returns one over the exponent-th power of ten.
 *
 * The method assumes that the base ring is the complex numbers.
 *
 * return one over the exponent-th power of 10
 **/
number tenToTheMinus(
       const int exponent  /**< [in]  the exponent for 1/10 */
                    )
{
  number ten = complexNumber(10.0, 0.0);
  number result = complexNumber(1.0, 0.0);
  number tmp;
  /* compute 10^{-exponent} inside result by subsequent divisions by 10 */
  for (int i = 1; i <= exponent; i++)
  {
    tmp = nDiv(result, ten);
    nDelete(&result);
    result = tmp;
  }
  nDelete(&ten);
  return result;
}

/* for debugging:
   for printing numbers to the console
   DELETE LATER */
void printSolutions(const int a, const int b, const int c)
{
  printf("\n------\n");
  /* build the polynomial a*x^2 + b*x + c: */
  poly p = NULL; poly q = NULL; poly r = NULL;
  if (a != 0)
  { p = pOne(); pSetExp(p, 1, 2); pSetm(p); pSetCoeff(p, nInit(a)); }
  if (b != 0)
  { q = pOne(); pSetExp(q, 1, 1); pSetm(q); pSetCoeff(q, nInit(b)); }
  if (c != 0)
  { r = pOne(); pSetCoeff(r, nInit(c)); }
  p = pAdd(p, q); p = pAdd(p, r);
  printf("poly = %s\n", pString(p));
  number tol = tenToTheMinus(20);
  number s1; number s2; int nSol = quadraticSolve(p, s1, s2, tol);
  nDelete(&tol);
  printf("solution code = %d\n", nSol);
  if ((1 <= nSol) && (nSol <= 3))
  {
    if (nSol != 3) { printNumber(s1); nDelete(&s1); }
    else { printNumber(s1); nDelete(&s1); printNumber(s2); nDelete(&s2); }
  }
  printf("------\n");
  pDelete(&p);
}

bool realSqrt(const number n, const number tolerance, number &root)
{
  if (!nGreaterZero(n)) return false;
  if (nIsZero(n)) return nInit(0);

  number oneHalf = complexNumber(0.5, 0.0);
  number nHalf   = nMult(n, oneHalf);
  root           = nCopy(n);
  number nOld    = complexNumber(10.0, 0.0);
  number nDiff   = nCopy(nOld);

  while (nGreater(nDiff, tolerance))
  {
    nDelete(&nOld);
    nOld = root;
    root = nAdd(nMult(oneHalf, nOld), nDiv(nHalf, nOld));
    nDelete(&nDiff);
    nDiff = nSub(nOld, root);
    if (!nGreaterZero(nDiff)) nDiff = nInpNeg(nDiff);
  }

  nDelete(&nOld); nDelete(&nDiff); nDelete(&oneHalf); nDelete(&nHalf);
  return true;
}

int quadraticSolve(const poly p, number &s1, number &s2,
                   const number tolerance)
{
  poly q = pCopy(p);
  int result;

  if (q == NULL) result = -1;
  else
  {
    int degree = pGetExp(q, 1);
    if (degree == 0) result = 0;   /* constant polynomial <> 0 */
    else
    {
      number c2 = nInit(0);   /* coefficient of var(1)^2 */
      number c1 = nInit(0);   /* coefficient of var(1)^1 */
      number c0 = nInit(0);   /* coefficient of var(1)^0 */
      if (pGetExp(q, 1) == 2)
      { nDelete(&c2); c2 = nCopy(pGetCoeff(q)); q = q->next; }
      if ((q != NULL) && (pGetExp(q, 1) == 1))
      { nDelete(&c1); c1 = nCopy(pGetCoeff(q)); q = q->next; }
      if ((q != NULL) && (pGetExp(q, 1) == 0))
      { nDelete(&c0); c0 = nCopy(pGetCoeff(q)); q = q->next; }

      if (degree == 1)
      {
        c0 = nInpNeg(c0);
        s1 = nDiv(c0, c1);
        result = 1;
      }
      else
      {
        number tmp = nMult(c0, c2);
        number tmp2 = nAdd(tmp, tmp); nDelete(&tmp);
        number tmp4 = nAdd(tmp2, tmp2); nDelete(&tmp2);
        number discr = nSub(nMult(c1, c1), tmp4); nDelete(&tmp4);
        if (nIsZero(discr))
        {
          tmp = nAdd(c2, c2);
          s1 = nDiv(c1, tmp); nDelete(&tmp);
          s1 = nInpNeg(s1);
          result = 2;
        }
        else if (nGreaterZero(discr))
        {
          realSqrt(discr, tolerance, tmp);   /* sqrt of the discriminant */
          tmp2 = nSub(tmp, c1);
          tmp4 = nAdd(c2, c2);
          s1 = nDiv(tmp2, tmp4); nDelete(&tmp2);
          tmp = nInpNeg(tmp);
          tmp2 = nSub(tmp, c1); nDelete(&tmp);
          s2 = nDiv(tmp2, tmp4); nDelete(&tmp2); nDelete(&tmp4);
          result = 3;
        }
        else
        {
          discr = nInpNeg(discr);
          realSqrt(discr, tolerance, tmp);   /* sqrt of |discriminant| */
          tmp2 = nAdd(c2, c2);
          tmp4 = nDiv(tmp, tmp2); nDelete(&tmp);
          tmp = nDiv(c1, tmp2); nDelete(&tmp2);
          tmp = nInpNeg(tmp);
          s1 = (number)new gmp_complex(((gmp_complex*)tmp)->real(),
                                       ((gmp_complex*)tmp4)->real());
          tmp4 = nInpNeg(tmp4);
          s2 = (number)new gmp_complex(((gmp_complex*)tmp)->real(),
                                       ((gmp_complex*)tmp4)->real());
          nDelete(&tmp); nDelete(&tmp4);
          result = 3;
        }
        nDelete(&discr);
      }
      nDelete(&c0); nDelete(&c1); nDelete(&c2);
    }
  }
  pDelete(&q);

  return result;
}

number euclideanNormSquared(const matrix aMat)
{
  int rr = MATROWS(aMat);
  number result = nInit(0);
  number tmp1; number tmp2;
  for (int r = 1; r <= rr; r++)
    if (MATELEM(aMat, r, 1) != NULL)
    {
      tmp1 = nMult(pGetCoeff(MATELEM(aMat, r, 1)),
                   pGetCoeff(MATELEM(aMat, r, 1)));
      tmp2 = nAdd(result, tmp1); nDelete(&result); nDelete(&tmp1);
      result = tmp2;
    }
  return result;
}

/* Returns a new number which is the absolute value of the coefficient
   of the given polynomial.
 *
 * The method assumes that the coefficient has imaginary part zero. */
number absValue(poly p)
{
  if (p == NULL) return nInit(0);
  number result = nCopy(pGetCoeff(p));
  if (!nGreaterZero(result)) result = nInpNeg(result);
  return result;
}

bool subMatrix(const matrix aMat, const int rowIndex1, const int rowIndex2,
               const int colIndex1, const int colIndex2, matrix &subMat)
{
  if (rowIndex1 > rowIndex2) return false;
  if (colIndex1 > colIndex2) return false;
  int rr = rowIndex2 - rowIndex1 + 1;
  int cc = colIndex2 - colIndex1 + 1;
  subMat = mpNew(rr, cc);
  for (int r = 1; r <= rr; r++)
    for (int c = 1; c <= cc; c++)
      MATELEM(subMat, r, c) =
        pCopy(MATELEM(aMat, rowIndex1 + r - 1, colIndex1 + c - 1));
  return true;
}

bool charPoly(const matrix aMat, poly &charPoly)
{
  if (MATROWS(aMat) != 2) return false;
  if (MATCOLS(aMat) != 2) return false;
  number b = nInit(0); number t;
  if (MATELEM(aMat, 1, 1) != NULL)
  { t = nAdd(b, pGetCoeff(MATELEM(aMat, 1, 1))); nDelete(&b); b = t;}
  if (MATELEM(aMat, 2, 2) != NULL)
  { t = nAdd(b, pGetCoeff(MATELEM(aMat, 2, 2))); nDelete(&b); b = t;}
  b = nInpNeg(b);
  number t1;
  if ((MATELEM(aMat, 1, 1) != NULL) && (MATELEM(aMat, 2, 2) != NULL))
    t1 = nMult(pGetCoeff(MATELEM(aMat, 1, 1)),
               pGetCoeff(MATELEM(aMat, 2, 2)));
  else t1 = nInit(0);
  number t2;
  if ((MATELEM(aMat, 1, 2) != NULL) && (MATELEM(aMat, 2, 1) != NULL))
    t2 = nMult(pGetCoeff(MATELEM(aMat, 1, 2)),
               pGetCoeff(MATELEM(aMat, 2, 1)));
  else t2 = nInit(0);
  number c = nSub(t1, t2); nDelete(&t1); nDelete(&t2);
  poly p = pOne(); pSetExp(p, 1, 2); pSetm(p);
  poly q = NULL;
  if (!nIsZero(b))
  { q = pOne(); pSetExp(q, 1, 1); pSetm(q); pSetCoeff(q, b); }
  poly r = NULL;
  if (!nIsZero(c))
  { r = pOne(); pSetCoeff(r, c); }
  p = pAdd(p, q); p = pAdd(p, r);
  charPoly = p;
  return true;
}

void swapRows(int row1, int row2, matrix& aMat)
{
  poly p;
  int cc = MATCOLS(aMat);
  for (int c = 1; c <= cc; c++)
  {
    p = MATELEM(aMat, row1, c);
    MATELEM(aMat, row1, c) = MATELEM(aMat, row2, c);
    MATELEM(aMat, row2, c) = p;
  }
}

void swapColumns(int column1, int column2,  matrix& aMat)
{
  poly p;
  int rr = MATROWS(aMat);
  for (int r = 1; r <= rr; r++)
  {
    p = MATELEM(aMat, r, column1);
    MATELEM(aMat, r, column1) = MATELEM(aMat, r, column2);
    MATELEM(aMat, r, column2) = p;
  }
}

void matrixBlock(const matrix aMat, const matrix bMat, matrix &block)
{
  int rowsA = MATROWS(aMat);
  int rowsB = MATROWS(bMat);
  int n = rowsA + rowsB;
  block = mpNew(n, n);
  for (int i = 1; i <= rowsA; i++)
    for (int j = 1; j <= rowsA; j++)
      MATELEM(block, i, j) = pCopy(MATELEM(aMat, i, j));
  for (int i = 1; i <= rowsB; i++)
    for (int j = 1; j <= rowsB; j++)
      MATELEM(block, i + rowsA, j + rowsA) = pCopy(MATELEM(bMat, i, j));
}

/**
 * Computes information related to one householder transformation step for
 * constructing the Hessenberg form of a given non-derogative matrix.
 *
 * The method assumes that all matrix entries are numbers coming from some
 * subfield of the reals. And that v has a non-zero first entry v_1 and a
 * second non-zero entry somewhere else.
 * Given such a vector v, it computes a number r (which will be the return
 * value of the method), a vector u and a matrix P such that:
 * 1) P * v = r * e_1,
 * 2) P = E - u * u^T,
 * 3) P = P^{-1}.
 * Note that enforcing norm(u) = sqrt(2), which is done in the algorithm,
 * guarantees property 3). This can be checked by expanding P^2 using
 * property 2).
 *
 * @return the number r
 **/
number hessenbergStep(
      const matrix vVec,     /**< [in]  the input vector v */
      matrix &uVec,          /**< [out] the output vector u */
      matrix &pMat,          /**< [out] the output matrix P */
      const number tolerance /**< [in]  accuracy for square roots */
                      )
{
  int rr = MATROWS(vVec);
  number vNormSquared = euclideanNormSquared(vVec);
  number vNorm; realSqrt(vNormSquared, tolerance, vNorm);
  /* v1 is guaranteed to be non-zero: */
  number v1 = pGetCoeff(MATELEM(vVec, 1, 1));
  bool v1Sign = true; if (nGreaterZero(v1)) v1Sign = false;

  number v1Abs = nCopy(v1); if (v1Sign) v1Abs = nInpNeg(v1Abs);
  number t1 = nDiv(v1Abs, vNorm);
  number one = nInit(1);
  number t2 = nAdd(t1, one); nDelete(&t1);
  number denominator; realSqrt(t2, tolerance, denominator); nDelete(&t2);
  uVec = mpNew(rr, 1);
  t1 = nDiv(v1Abs, vNorm);
  t2 = nAdd(t1, one); nDelete(&t1);
  t1 = nDiv(t2, denominator); nDelete(&t2);
  MATELEM(uVec, 1, 1) = pOne();
  pSetCoeff(MATELEM(uVec, 1, 1), t1);   /* we know that t1 != 0 */
  for (int r = 2; r <= rr; r++)
  {
    if (MATELEM(vVec, r, 1) != NULL)
      t1 = nCopy(pGetCoeff(MATELEM(vVec, r, 1)));
    else t1 = nInit(0);
    if (v1Sign) t1 = nInpNeg(t1);
    t2 = nDiv(t1, vNorm); nDelete(&t1);
    t1 = nDiv(t2, denominator); nDelete(&t2);
    if (!nIsZero(t1))
    {
      MATELEM(uVec, r, 1) = pOne();
      pSetCoeff(MATELEM(uVec, r, 1), t1);
    }
    else nDelete(&t1);
  }
  nDelete(&denominator);

  /* finished building vector u; now turn to pMat */
  pMat = mpNew(rr, rr);
  /* we set P := E - u * u^T, as desired */
  for (int r = 1; r <= rr; r++)
    for (int c = 1; c <= rr; c++)
    {
      if ((MATELEM(uVec, r, 1) != NULL) && (MATELEM(uVec, c, 1) != NULL))
        t1 = nMult(pGetCoeff(MATELEM(uVec, r, 1)),
                   pGetCoeff(MATELEM(uVec, c, 1)));
      else t1 = nInit(0);
      if (r == c) { t2 = nSub(one, t1); nDelete(&t1); }
      else          t2 = nInpNeg(t1);
      if (!nIsZero(t2))
      {
        MATELEM(pMat, r, c) = pOne();
        pSetCoeff(MATELEM(pMat, r, c), t2);
      }
      else nDelete(&t2);
    }
  nDelete(&one);
  /* finished building pMat; now compute the return value */
  t1 = vNormSquared; if (v1Sign) t1 = nInpNeg(t1);
  t2 = nMult(v1, vNorm);
  number t3 = nAdd(t1, t2); nDelete(&t1); nDelete(&t2);
  t1 = nAdd(v1Abs, vNorm); nDelete(&v1Abs); nDelete(&vNorm);
  t2 = nDiv(t3, t1); nDelete(&t1); nDelete(&t3);
  t2 = nInpNeg(t2);
  return t2;
}

void hessenberg(const matrix aMat, matrix &pMat, matrix &hessenbergMat,
                const number tolerance, const ring R)
{
  int n = MATROWS(aMat);
  unitMatrix(n, pMat);
  subMatrix(aMat, 1, n, 1, n, hessenbergMat);
  for (int c = 1; c <= n; c++)
  {
    /* find one or two non-zero entries in the current column */
    int r1 = 0; int r2 = 0;
    for (int r = c + 1; r <= n; r++)
      if (MATELEM(hessenbergMat, r, c) != NULL)
      {
        if      (r1 == 0)   r1 = r;
        else if (r2 == 0) { r2 = r; break; }
      }
    if (r1 != 0)
    { /* At least one entry in the current column is non-zero. */
      if (r1 != c + 1)
      { /* swap rows to bring non-zero element to row with index c + 1 */
        swapRows(r1, c + 1, hessenbergMat);
        /* now also swap columns to reflect action of permutation
           from the right-hand side */
        swapColumns(r1, c + 1, hessenbergMat);
        /* include action of permutation also in pMat */
        swapRows(r1, c + 1, pMat);
      }
      if (r2 != 0)
      { /* There is at least one more non-zero element in the current
           column. So let us perform a hessenberg step in order to make
           all additional non-zero elements zero. */
        matrix v; subMatrix(hessenbergMat, c + 1, n, c, c, v);
        matrix u; matrix pTmp;
        number r = hessenbergStep(v, u, pTmp, tolerance);
        idDelete((ideal*)&v); idDelete((ideal*)&u); nDelete(&r);
        /* pTmp just acts on the lower right block of hessenbergMat;
           i.e., it needs to be extended by a unit matrix block at the top
           left in order to define a whole transformation matrix;
           this code may be optimized */
        unitMatrix(c, u);
        matrix pTmpFull; matrixBlock(u, pTmp, pTmpFull);
        idDelete((ideal*)&u); idDelete((ideal*)&pTmp);
        /* now include pTmpFull in pMat (by letting it act from the left) */
        pTmp = mp_Mult(pTmpFull, pMat,R); idDelete((ideal*)&pMat);
        pMat = pTmp;
        /* now let pTmpFull act on hessenbergMat from the left and from the
           right (note that pTmpFull is self-inverse) */
        pTmp = mp_Mult(pTmpFull, hessenbergMat,R);
        idDelete((ideal*)&hessenbergMat);
        hessenbergMat = mp_Mult(pTmp, pTmpFull, R);
        idDelete((ideal*)&pTmp); idDelete((ideal*)&pTmpFull);
        /* as there may be inaccuracy, we erase those entries of hessenbergMat
           which must have become zero by the last transformation */
        for (int r = c + 2; r <= n; r++)
          pDelete(&MATELEM(hessenbergMat, r, c));
      }
    }
  }
}

/**
 * Performs one transformation step on the given matrix H as part of
 * the gouverning QR double shift algorithm.
 * The method will change the given matrix H side-effect-wise. The resulting
 * matrix H' will be in Hessenberg form.
 * The iteration index is needed, since for the 11th and 21st iteration,
 * the transformation step is different from the usual step, to avoid
 * convergence problems of the gouverning QR double shift process (who is
 * also the only caller of this method).
 **/
void mpTrafo(
      matrix &H,             /**< [in/out]  the matrix to be transformed */
      int it,                /**< [in]      iteration index */
      const number tolerance,/**< [in]      accuracy for square roots */
      const ring R
            )
{
  int n = MATROWS(H);
  number trace; number det; number tmp1; number tmp2; number tmp3;

  if ((it != 11) && (it != 21)) /* the standard case */
  {
    /* in this case 'trace' will really be the trace of the lowermost
       (2x2) block of hMat */
    trace = nInit(0);
    det = nInit(0);
    if (MATELEM(H, n - 1, n - 1) != NULL)
    {
      tmp1 = nAdd(trace, pGetCoeff(MATELEM(H, n - 1, n - 1)));
      nDelete(&trace);
      trace = tmp1;
    }
    if (MATELEM(H, n, n) != NULL)
    {
      tmp1 = nAdd(trace, pGetCoeff(MATELEM(H, n, n)));
      nDelete(&trace);
      trace = tmp1;
    }
    /* likewise 'det' will really be the determinante of the lowermost
       (2x2) block of hMat */
    if ((MATELEM(H, n - 1, n - 1 ) != NULL) && (MATELEM(H, n, n) != NULL))
    {
      tmp1 = nMult(pGetCoeff(MATELEM(H, n - 1, n - 1)),
                   pGetCoeff(MATELEM(H, n, n)));
      tmp2 = nAdd(tmp1, det); nDelete(&tmp1); nDelete(&det);
      det = tmp2;
    }
    if ((MATELEM(H, n - 1, n) != NULL) && (MATELEM(H, n, n - 1) != NULL))
    {
      tmp1 = nMult(pGetCoeff(MATELEM(H, n - 1, n)),
                   pGetCoeff(MATELEM(H, n, n - 1)));
      tmp2 = nSub(det, tmp1); nDelete(&tmp1); nDelete(&det);
      det = tmp2;
    }
  }
  else
  {
    /* for it = 11 or it = 21, we use special formulae to avoid convergence
       problems of the gouverning QR double shift algorithm (who is the only
       caller of this method) */
    /* trace = 3/2 * (|hMat[n, n-1]| + |hMat[n-1, n-2]|) */
    tmp1 = nInit(0);
    if (MATELEM(H, n, n - 1) != NULL)
    { nDelete(&tmp1); tmp1 = nCopy(pGetCoeff(MATELEM(H, n, n - 1))); }
    if (!nGreaterZero(tmp1)) tmp1 = nInpNeg(tmp1);
    tmp2 = nInit(0);
    if (MATELEM(H, n - 1, n - 2) != NULL)
    { nDelete(&tmp2); tmp2 = nCopy(pGetCoeff(MATELEM(H, n - 1, n - 2))); }
    if (!nGreaterZero(tmp2)) tmp2 = nInpNeg(tmp2);
    tmp3 = nAdd(tmp1, tmp2); nDelete(&tmp1); nDelete(&tmp2);
    tmp1 = nInit(3); tmp2 = nInit(2);
    trace = nDiv(tmp1, tmp2); nDelete(&tmp1); nDelete(&tmp2);
    tmp1 = nMult(tmp3, trace); nDelete(&trace);
    trace = tmp1;
    /* det = (|hMat[n, n-1]| + |hMat[n-1, n-2]|)^2 */
    det = nMult(tmp3, tmp3); nDelete(&tmp3);
  }
  matrix c = mpNew(n, 1);
  trace = nInpNeg(trace);
  MATELEM(c,1,1) = pAdd(pAdd(pAdd(ppMult_qq(MATELEM(H,1,1), MATELEM(H,1,1)),
                                  ppMult_qq(MATELEM(H,1,2), MATELEM(H,2,1))),
                             __pp_Mult_nn(MATELEM(H,1,1), trace, currRing)),
                        __p_Mult_nn(pOne(), det,currRing));
  MATELEM(c,2,1) = pAdd(pMult(pCopy(MATELEM(H,2,1)),
                              pAdd(pCopy(MATELEM(H,1,1)),
                                   pCopy(MATELEM(H,2,2)))),
                        __pp_Mult_nn(MATELEM(H,2,1), trace,currRing));
  MATELEM(c,3,1) = ppMult_qq(MATELEM(H,2,1), MATELEM(H,3,2));
  nDelete(&trace); nDelete(&det);

  /* for applying hessenbergStep, we need to make sure that c[1, 1] is
     not zero */
  if ((MATELEM(c,1,1) != NULL) &&
      ((MATELEM(c,2,1) != NULL) || (MATELEM(c,3,1) != NULL)))
  {
    matrix uVec; matrix hMat;
    tmp1 = hessenbergStep(c, uVec, hMat, tolerance); nDelete(&tmp1);
    /* now replace H by hMat * H * hMat: */
    matrix wMat = mp_Mult(hMat, H,R); idDelete((ideal*)&H);
    matrix H1 = mp_Mult(wMat, hMat,R);
    idDelete((ideal*)&wMat); idDelete((ideal*)&hMat);
    /* now need to re-establish Hessenberg form of H1 and put it in H */
    hessenberg(H1, wMat, H, tolerance,R);
    idDelete((ideal*)&wMat); idDelete((ideal*)&H1);
  }
  else if ((MATELEM(c,1,1) == NULL) && (MATELEM(c,2,1) != NULL))
  {
    swapRows(1, 2, H);
    swapColumns(1, 2, H);
  }
  else if ((MATELEM(c,1,1) == NULL) && (MATELEM(c,3,1) != NULL))
  {
    swapRows(1, 3, H);
    swapColumns(1, 3, H);
  }
  else
  { /* c is the zero vector or a multiple of e_1;
       no hessenbergStep needed */ }
}

/* helper for qrDoubleShift */
bool qrDS(
       const int /*n*/,
       matrix* queue,
       int& queueL,
       number* eigenValues,
       int& eigenValuesL,
       const number tol1,
       const number tol2,
       const ring R
         )
{
  bool deflationFound = true;
  /* we loop until the working queue is empty,
     provided we always find deflation */
  while (deflationFound && (queueL > 0))
  {
    /* take out last queue entry */
    matrix currentMat = queue[queueL - 1]; queueL--;
    int m = MATROWS(currentMat);
    if (m == 1)
    {
      number newEigenvalue;
      /* the entry at [1, 1] is the eigenvalue */
      if (MATELEM(currentMat, 1, 1) == NULL) newEigenvalue = nInit(0);
      else newEigenvalue = nCopy(pGetCoeff(MATELEM(currentMat, 1, 1)));
      eigenValues[eigenValuesL++] = newEigenvalue;
    }
    else if (m == 2)
    {
      /* there are two eigenvalues which come as zeros of the characteristic
         polynomial */
      poly p; charPoly(currentMat, p);
      number s1; number s2;
      int nSol = quadraticSolve(p, s1, s2, tol2); pDelete(&p);
      assume(nSol >= 2);
      eigenValues[eigenValuesL++] = s1;
      /* if nSol = 2, then s1 is a double zero, and s2 is invalid: */
      if (nSol == 2) s2 = nCopy(s1);
      eigenValues[eigenValuesL++] = s2;
    }
    else /* m > 2 */
    {
      /* bring currentMat into Hessenberg form to fasten computations: */
      matrix mm1; matrix mm2;
      hessenberg(currentMat, mm1, mm2, tol2,R);
      idDelete((ideal*)&currentMat); idDelete((ideal*)&mm1);
      currentMat = mm2;
      int it = 1; bool doLoop = true;
      while (doLoop && (it <= 30 * m))
      {
        /* search for deflation */
        number w1; number w2;
        number test1; number test2; bool stopCriterion = false; int k;
        for (k = 1; k < m; k++)
        {
          test1 = absValue(MATELEM(currentMat, k + 1, k));
          w1 = absValue(MATELEM(currentMat, k, k));
          w2 = absValue(MATELEM(currentMat, k + 1, k + 1));
          test2 = nMult(tol1, nAdd(w1, w2));
          nDelete(&w1); nDelete(&w2);
          if (!nGreater(test1, test2)) stopCriterion = true;
          nDelete(&test1); nDelete(&test2);
          if (stopCriterion) break;
        }
        if (k < m)   /* found deflation at position (k + 1, k) */
        {
          pDelete(&MATELEM(currentMat, k + 1, k)); /* make this entry zero */
          subMatrix(currentMat, 1, k, 1, k, queue[queueL++]);
          subMatrix(currentMat, k + 1, m, k + 1, m, queue[queueL++]);
          doLoop = false;
        }
        else   /* no deflation found yet */
        {
          mpTrafo(currentMat, it, tol2,R);
          it++;   /* try again */
        }
      }
      if (doLoop)   /* could not find deflation for currentMat */
      {
        deflationFound = false;
      }
      idDelete((ideal*)&currentMat);
    }
  }
  return deflationFound;
}

/**
 * Tries to find the number n in the array nn[0..nnLength-1].
 *
 * The method assumes that the ground field is the complex numbers.
 * n and an entry of nn will be regarded equal when the absolute
 * value of their difference is not larger than the given tolerance.
 * In this case, the index of the respective entry of nn is returned,
 * otherwise -1.
 *
 * @return the index of n in nn (up to tolerance) or -1
 **/
int similar(
       const number* nn,       /**< [in] array of numbers to look-up */
       const int nnLength,     /**< [in] length of nn                */
       const number n,         /**< [in] number to loop-up in nn     */
       const number tolerance  /**< [in] tolerance for comparison    */
           )
{
  int result = -1;
  number tt = nMult(tolerance, tolerance);
  number nr = (number)new gmp_complex(((gmp_complex*)n)->real());
  number ni = (number)new gmp_complex(((gmp_complex*)n)->imag());
  number rr; number ii;
  number w1; number w2; number w3; number w4; number w5;
  for (int i = 0; i < nnLength; i++)
  {
    rr = (number)new gmp_complex(((gmp_complex*)nn[i])->real());
    ii = (number)new gmp_complex(((gmp_complex*)nn[i])->imag());
    w1 = nSub(nr, rr); w2 = nMult(w1, w1);
    w3 = nSub(ni, ii); w4 = nMult(w3, w3);
    w5 = nAdd(w2, w4);
    if (!nGreater(w5, tt)) result = i;
    nDelete(&w1); nDelete(&w2); nDelete(&w3); nDelete(&w4);
                nDelete(&w5); nDelete(&rr); nDelete(&ii);
    if (result != -1) break;
  }
  nDelete(&tt); nDelete(&nr); nDelete(&ni);
  return result;
}

/* This codes assumes that there are at least two variables in the current
   base ring. No assumption is made regarding the monomial ordering. */
void henselFactors(const int xIndex, const int yIndex, const poly h,
                   const poly f0, const poly g0, const int d, poly &f, poly &g)
{
  int n = (int)p_Deg(f0,currRing);
  int m = (int)p_Deg(g0,currRing);
  matrix aMat = mpNew(n + m, n + m);     /* matrix A for linear system */
  matrix pMat; matrix lMat; matrix uMat; /* for the decomposition of A */
  f = pCopy(f0); g = pCopy(g0);          /* initially: h = f*g mod <x^1> */

  /* initial step: read off coefficients of f0, and g0 */
  poly p = f0; poly matEntry; number c;
  while (p != NULL)
  {
    c = nCopy(pGetCoeff(p));
    matEntry = pOne(); pSetCoeff(matEntry, c);
    MATELEM(aMat, pGetExp(p, yIndex) + 1, 1) = matEntry;
    p = pNext(p);
  }
  p = g0;
  while (p != NULL)
  {
    c = nCopy(pGetCoeff(p));
    matEntry = pOne(); pSetCoeff(matEntry, c);
    MATELEM(aMat, pGetExp(p, yIndex) + 1, m + 1) = matEntry;
    p = pNext(p);
  }
  /* fill the rest of A */
  for (int row = 2; row <= n + 1; row++)
    for (int col = 2; col <= m; col++)
    {
      if (col > row) break;
      MATELEM(aMat, row, col) = pCopy(MATELEM(aMat, row - 1, col - 1));
    }
  for (int row = n + 2; row <= n + m; row++)
    for (int col = row - n; col <= m; col++)
      MATELEM(aMat, row, col) = pCopy(MATELEM(aMat, row - 1, col - 1));
  for (int row = 2; row <= m + 1; row++)
    for (int col = m + 2; col <= m + n; col++)
    {
      if (col - m > row) break;
      MATELEM(aMat, row, col) = pCopy(MATELEM(aMat, row - 1, col - 1));
    }
  for (int row = m + 2; row <= n + m; row++)
    for (int col = row; col <= m + n; col++)
      MATELEM(aMat, row, col) = pCopy(MATELEM(aMat, row - 1, col - 1));

  /* constructing the LU-decomposition of A */
  luDecomp(aMat, pMat, lMat, uMat);

  /* Before the xExp-th loop, we know that h = f*g mod <x^xExp>.
     Afterwards the algorithm ensures      h = f*g mod <x^(xExp + 1)>.
     Hence in the end we obtain f and g as required, i.e.,
                                           h = f*g mod <x^(d+1)>.
     The algorithm works by solving a (m+n)x(m+n) linear equation system
     A*x = b with constant matrix A (as decomposed above). By theory, the
     system is guaranteed to have a unique solution. */
  poly fg = ppMult_qq(f, g);   /* for storing the product of f and g */
  for (int xExp = 1; xExp <= d; xExp++)
  {
    matrix bVec = mpNew(n + m, 1);     /* b */
    matrix xVec = mpNew(n + m, 1);     /* x */

    p = pCopy(fg);
    p = pAdd(pCopy(h), pNeg(p));       /* p = h - f*g */
    /* we collect all terms in p with x-exponent = xExp and use their
       coefficients to build the vector b */
    int bIsZeroVector = true;
    while (p != NULL)
    {
      if (pGetExp(p, xIndex) == xExp)
      {
        number c = nCopy(pGetCoeff(p));
        poly matEntry = pOne(); pSetCoeff(matEntry, c);
        MATELEM(bVec, pGetExp(p, yIndex) + 1, 1) = matEntry;
        if (matEntry != NULL) bIsZeroVector = false;
      }
      pLmDelete(&p); /* destruct leading term of p and move to next term */
    }
    /* solve the linear equation system */
    if (!bIsZeroVector) /* otherwise x = 0 and f, g do not change */
    {
      matrix notUsedMat;
      luSolveViaLUDecomp(pMat, lMat, uMat, bVec, xVec, notUsedMat);
      idDelete((ideal*)&notUsedMat);
      /* update f and g by newly computed terms, and update f*g */
      poly fNew = NULL; poly gNew = NULL;
      for (int row = 1; row <= m; row++)
      {
        if (MATELEM(xVec, row, 1) != NULL)
        {
          p = pCopy(MATELEM(xVec, row, 1));   /* p = c                  */
          pSetExp(p, xIndex, xExp);           /* p = c * x^xExp         */
          pSetExp(p, yIndex, row - 1);        /* p = c * x^xExp * y^i   */
          pSetm(p);
          gNew = pAdd(gNew, p);
        }
      }
      for (int row = m + 1; row <= m + n; row++)
      {
        if (MATELEM(xVec, row, 1) != NULL)
        {
          p = pCopy(MATELEM(xVec, row, 1));   /* p = c                  */
          pSetExp(p, xIndex, xExp);           /* p = c * x^xExp         */
          pSetExp(p, yIndex, row - m - 1);    /* p = c * x^xExp * y^i   */
          pSetm(p);
          fNew = pAdd(fNew, p);
        }
      }
      fg = pAdd(fg, ppMult_qq(f, gNew));
      fg = pAdd(fg, ppMult_qq(g, fNew));
      fg = pAdd(fg, ppMult_qq(fNew, gNew));
      f = pAdd(f, fNew);
      g = pAdd(g, gNew);
    }
    /* clean-up loop-dependent vectors */
    idDelete((ideal*)&bVec); idDelete((ideal*)&xVec);
  }

  /* clean-up matrices A, P, L and U, and polynomial fg */
  idDelete((ideal*)&aMat); idDelete((ideal*)&pMat);
  idDelete((ideal*)&lMat); idDelete((ideal*)&uMat);
  pDelete(&fg);
}

void lduDecomp(const matrix aMat, matrix &pMat, matrix &lMat, matrix &dMat,
               matrix &uMat, poly &l, poly &u, poly &lTimesU)
{
  int rr = aMat->rows();
  int cc = aMat->cols();
  /* we use an int array to store all row permutations;
     note that we only make use of the entries [1..rr] */
  int* permut = new int[rr + 1];
  for (int i = 1; i <= rr; i++) permut[i] = i;
  /* fill lMat and dMat with the (rr x rr) unit matrix */
  unitMatrix(rr, lMat);
  unitMatrix(rr, dMat);
  uMat = mpNew(rr, cc);
  /* copy aMat into uMat: */
  for (int r = 1; r <= rr; r++)
    for (int c = 1; c <= cc; c++)
      MATELEM(uMat, r, c) = pCopy(MATELEM(aMat, r, c));
  u = pOne(); l = pOne();

  int col = 1; int row = 1;
  while ((col <= cc) & (row < rr))
  {
    int pivotR; int pivotC; bool pivotValid = false;
    while (col <= cc)
    {
      pivotValid = pivot(uMat, row, rr, col, col, &pivotR, &pivotC);
      if (pivotValid)  break;
      col++;
    }
    if (pivotValid)
    {
      if (pivotR != row)
      {
        swapRows(row, pivotR, uMat);
        poly p = MATELEM(dMat, row, row);
        MATELEM(dMat, row, row) = MATELEM(dMat, pivotR, pivotR);
        MATELEM(dMat, pivotR, pivotR) = p;
        swapColumns(row, pivotR, lMat);
        swapRows(row, pivotR, lMat);
        int temp = permut[row];
        permut[row] = permut[pivotR]; permut[pivotR] = temp;
      }
      /* in gg, we compute the gcd of all non-zero elements in
         uMat[row..rr, col];
         the next number is the pivot and thus guaranteed to be different
         from zero: */
      number gg = nCopy(pGetCoeff(MATELEM(uMat, row, col))); number t;
      for (int r = row + 1; r <= rr; r++)
      {
        if (MATELEM(uMat, r, col) != NULL)
        {
          t = gg;
          gg = n_Gcd(t, pGetCoeff(MATELEM(uMat, r, col)),currRing->cf);
          nDelete(&t);
        }
      }
      t = nDiv(pGetCoeff(MATELEM(uMat, row, col)), gg);
      nNormalize(t);   /* this division works without remainder */
      if (!nIsOne(t))
      {
        for (int r = row; r <= rr; r++)
          MATELEM(dMat, r, r)=__p_Mult_nn(MATELEM(dMat, r, r), t,currRing);
        MATELEM(lMat, row, row)=__p_Mult_nn(MATELEM(lMat, row, row), t,currRing);
      }
      l = pMult(l, pCopy(MATELEM(lMat, row, row)));
      u = pMult(u, pCopy(MATELEM(uMat, row, col)));

      for (int r = row + 1; r <= rr; r++)
      {
        if (MATELEM(uMat, r, col) != NULL)
        {
          number g = n_Gcd(pGetCoeff(MATELEM(uMat, row, col)),
                          pGetCoeff(MATELEM(uMat, r, col)),
                          currRing->cf);
          number f1 = nDiv(pGetCoeff(MATELEM(uMat, r, col)), g);
          nNormalize(f1);   /* this division works without remainder */
          number f2 = nDiv(pGetCoeff(MATELEM(uMat, row, col)), g);
          nNormalize(f2);   /* this division works without remainder */
          pDelete(&MATELEM(uMat, r, col)); MATELEM(uMat, r, col) = NULL;
          for (int c = col + 1; c <= cc; c++)
          {
            poly p = MATELEM(uMat, r, c);
            p=__p_Mult_nn(p, f2,currRing);
            poly q = pCopy(MATELEM(uMat, row, c));
            q=__p_Mult_nn(q, f1,currRing); q = pNeg(q);
            MATELEM(uMat, r, c) = pAdd(p, q);
          }
          number tt = nDiv(g, gg);
          nNormalize(tt);   /* this division works without remainder */
          MATELEM(lMat, r, r)=__p_Mult_nn(MATELEM(lMat, r, r), tt, currRing);
	  nDelete(&tt);
          MATELEM(lMat, r, row) = pCopy(MATELEM(lMat, r, r));
          MATELEM(lMat, r, row)=__p_Mult_nn(MATELEM(lMat, r, row), f1,currRing);
          nDelete(&f1); nDelete(&f2); nDelete(&g);
        }
        else
	  MATELEM(lMat, r, r)=__p_Mult_nn(MATELEM(lMat, r, r), t, currRing);
      }
      nDelete(&t); nDelete(&gg);
      col++; row++;
    }
  }
  /* one factor in the product u might be missing: */
  if (row == rr)
  {
    while ((col <= cc) && (MATELEM(uMat, row, col) == NULL)) col++;
    if (col <= cc) u = pMult(u, pCopy(MATELEM(uMat, row, col)));
  }

  /* building the permutation matrix from 'permut' and computing l */
  pMat = mpNew(rr, rr);
  for (int r = 1; r <= rr; r++)
    MATELEM(pMat, r, permut[r]) = pOne();
  delete[] permut;

  lTimesU = ppMult_qq(l, u);
}

bool luSolveViaLDUDecomp(const matrix pMat, const matrix lMat,
                         const matrix dMat, const matrix uMat,
                         const poly l, const poly u, const poly lTimesU,
                         const matrix bVec, matrix &xVec, matrix &H)
{
  int m = uMat->rows(); int n = uMat->cols();
  matrix cVec = mpNew(m, 1);  /* for storing l * pMat * bVec */
  matrix yVec = mpNew(m, 1);  /* for storing the unique solution of
                                 lMat * yVec = cVec */

  /* compute cVec = l * pMat * bVec but without actual matrix mult. */
  for (int r = 1; r <= m; r++)
  {
    for (int c = 1; c <= m; c++)
    {
      if (MATELEM(pMat, r, c) != NULL)
        { MATELEM(cVec, r, 1) = ppMult_qq(l, MATELEM(bVec, c, 1)); break; }
    }
  }

  /* solve lMat * yVec = cVec; this will always work as lMat is invertible;
     moreover, all divisions are guaranteed to be without remainder */
  poly p; poly q;
  for (int r = 1; r <= m; r++)
  {
    p = pNeg(pCopy(MATELEM(cVec, r, 1)));
    for (int c = 1; c < r; c++)
      p = pAdd(p, ppMult_qq(MATELEM(lMat, r, c), MATELEM(yVec, c, 1) ));
    /* The following division is without remainder. */
    q = pNSet(nInvers(pGetCoeff(MATELEM(lMat, r, r))));
    MATELEM(yVec, r, 1) = pMult(pNeg(p), q);
    pNormalize(MATELEM(yVec, r, 1));
  }

  /* compute u * dMat * yVec and put result into yVec */
  for (int r = 1; r <= m; r++)
  {
    p = ppMult_qq(u, MATELEM(dMat, r, r));
    MATELEM(yVec, r, 1) = pMult(p, MATELEM(yVec, r, 1));
  }

  /* determine whether uMat * xVec = yVec is solvable */
  bool isSolvable = true;
  bool isZeroRow; int nonZeroRowIndex;
  for (int r = m; r >= 1; r--)
  {
    isZeroRow = true;
    for (int c = 1; c <= n; c++)
      if (MATELEM(uMat, r, c) != NULL) { isZeroRow = false; break; }
    if (isZeroRow)
    {
      if (MATELEM(yVec, r, 1) != NULL) { isSolvable = false; break; }
    }
    else { nonZeroRowIndex = r; break; }
  }

  if (isSolvable)
  {
    xVec = mpNew(n, 1);
    matrix N = mpNew(n, n); int dim = 0;
    /* solve uMat * xVec = yVec and determine a basis of the solution
       space of the homogeneous system uMat * xVec = 0;
       We do not know in advance what the dimension (dim) of the latter
       solution space will be. Thus, we start with the possibly too wide
       matrix N and later copy the relevant columns of N into H. */
    int nonZeroC; int lastNonZeroC = n + 1;
    for (int r = nonZeroRowIndex; r >= 1; r--)
    {
      for (nonZeroC = 1; nonZeroC <= n; nonZeroC++)
        if (MATELEM(uMat, r, nonZeroC) != NULL) break;
      for (int w = lastNonZeroC - 1; w >= nonZeroC + 1; w--)
      {
        /* this loop will only be done when the given linear system has
           more than one, i.e., infinitely many solutions */
        dim++;
        /* now we fill two entries of the dim-th column of N */
        MATELEM(N, w, dim) = pNeg(pCopy(MATELEM(uMat, r, nonZeroC)));
        MATELEM(N, nonZeroC, dim) = pCopy(MATELEM(uMat, r, w));
      }
      for (int d = 1; d <= dim; d++)
      {
        /* here we fill the entry of N at [r, d], for each valid vector
           that we already have in N;
           again, this will only be done when the given linear system has
           more than one, i.e., infinitely many solutions */
        p = NULL;
        for (int c = nonZeroC + 1; c <= n; c++)
          if (MATELEM(N, c, d) != NULL)
            p = pAdd(p, ppMult_qq(MATELEM(uMat, r, c), MATELEM(N, c, d)));
        /* The following division may be with remainder but only takes place
           when dim > 0. */
        q = pNSet(nInvers(pGetCoeff(MATELEM(uMat, r, nonZeroC))));
        MATELEM(N, nonZeroC, d) = pMult(pNeg(p), q);
        pNormalize(MATELEM(N, nonZeroC, d));
      }
      p = pNeg(pCopy(MATELEM(yVec, r, 1)));
      for (int c = nonZeroC + 1; c <= n; c++)
        if (MATELEM(xVec, c, 1) != NULL)
          p = pAdd(p, ppMult_qq(MATELEM(uMat, r, c), MATELEM(xVec, c, 1)));
      /* The following division is without remainder. */
      q = pNSet(nInvers(pGetCoeff(MATELEM(uMat, r, nonZeroC))));
      MATELEM(xVec, nonZeroC, 1) = pMult(pNeg(p), q);
      pNormalize(MATELEM(xVec, nonZeroC, 1));
      lastNonZeroC = nonZeroC;
    }

    /* divide xVec by l*u = lTimesU and put result in xVec */
    number z = nInvers(pGetCoeff(lTimesU));
    for (int c = 1; c <= n; c++)
    {
      MATELEM(xVec, c, 1)=__p_Mult_nn(MATELEM(xVec, c, 1), z,currRing);
      pNormalize(MATELEM(xVec, c, 1));
    }
    nDelete(&z);

    if (dim == 0)
    {
      /* that means the given linear system has exactly one solution;
         we return as H the 1x1 matrix with entry zero */
      H = mpNew(1, 1);
    }
    else
    {
      /* copy the first 'dim' columns of N into H */
      H = mpNew(n, dim);
      for (int r = 1; r <= n; r++)
        for (int c = 1; c <= dim; c++)
          MATELEM(H, r, c) = pCopy(MATELEM(N, r, c));
    }
    /* clean up N */
    idDelete((ideal*)&N);
  }

  idDelete((ideal*)&cVec);
  idDelete((ideal*)&yVec);

  return isSolvable;
}

