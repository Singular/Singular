/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfNewtonPolygon.cc
 *
 * This file provides functions to compute the Newton polygon of a bivariate
 * polynomial
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"

#include <stdlib.h>

#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_algorithm.h"
#include "cf_primes.h"
#include "cf_reval.h"
#include "cf_factory.h"
#include "gfops.h"
#include "cfNewtonPolygon.h"
#include "templates/ftmpl_functions.h"

static
void translate (int** points, int* point, int sizePoints) //make point to 0
{
  for (int i= 0; i < sizePoints; i++)
  {
    points[i] [0] -= point [0];
    points[i] [1] -= point [1];
  }
}

static
int smallestPointIndex (int** points, int sizePoints)
{
  int min= 0;
  for (int i= 1; i < sizePoints; i++)
  {
    if (points[i][0] < points[min][0] ||
        (points[i] [0] == points[min] [0] && points[i] [1] < points[min] [1]))
      min= i;
  }
  return min;
}

static
void swap (int** points, int i, int j)
{
  int* tmp= points[i];
  points[i]= points[j];
  points[j]= tmp;
}

static
bool isLess (int* point1, int* point2)
{
  long area= point1[0]*point2[1]- point1[1]*point2[0];
  if (area > 0) return true;
  if (area == 0)
  {
    return (abs (point1[0]) + abs (point1[1]) >
            abs (point2[0]) + abs (point2[1]));
  }
  return false;
}

static
void quickSort (int lo, int hi, int** points)
{
  int i= lo, j= hi;
  int* point= new int [2];
  point [0]= points [(lo+hi)/2] [0];
  point [1]= points [(lo+hi)/2] [1];
  while (i <= j)
  {
    while (isLess (points [i], point) && i < hi) i++;
    while (isLess (point, points[j]) && j > lo) j--;
    if (i <= j)
    {
      swap (points, i, j);
      i++;
      j--;
    }
  }
  delete [] point;
  if (lo < j) quickSort (lo, j, points);
  if (i < hi) quickSort (i, hi, points);
}

static
void sort (int** points, int sizePoints)
{
  quickSort (1, sizePoints - 1, points);
}

// check whether p2 is convex
static
bool isConvex (int* point1, int* point2, int* point3)
{
  long relArea= (point1[0] - point2[0])*(point3[1] - point2[1]) -
                (point1[1] - point2[1])*(point3[0] - point2[0]);
  if (relArea < 0)
    return true;
  if (relArea == 0)
  {
    return !(abs (point1[0] - point3[0]) + abs (point1[1] - point3[1]) >=
             (abs (point2[0] - point1[0]) + abs (point2[1] - point1[1]) +
             abs (point2[0] - point3[0]) + abs (point2[1] - point3[1])));
  }
  return false;
}

static
bool isConvex (int** points, int i)
{
  return isConvex (points[i - 1], points [i], points [i + 1]);
}

int grahamScan (int** points, int sizePoints)
{
  swap (points, 0, smallestPointIndex (points, sizePoints));
  int * minusPoint= new int [2];
  minusPoint [0]= points[0] [0];
  minusPoint [1]= points[0] [1];
  translate (points, minusPoint, sizePoints);
  sort (points, sizePoints);
  minusPoint[0]= - minusPoint[0];
  minusPoint[1]= - minusPoint[1];
  translate (points, minusPoint, sizePoints); //reverse translation
  delete [] minusPoint;
  int i= 3, k= 3;
  while (k < sizePoints)
  {
    swap (points, i, k);
    while (!isConvex (points, i - 1))
    {
      swap (points, i - 1, i);
      i--;
    }
    k++;
    i++;
  }
  if (i + 1 <= sizePoints || i == sizePoints)
  {
    long relArea=
    (points [i-2][0] - points [i-1][0])*(points [0][1] - points [i-1][1])-
    (points [i-2][1] - points [i-1][1])*(points [0][0] - points [i-1][0]);
    if (relArea == 0)
    {
      if (abs (points [i-2][0] - points [0][0]) +
          abs (points [i-2][1] - points [0][1]) >=
          abs (points [i-1][0] - points [i-2][0]) +
          abs (points [i-1][1] - points [i-2][1]) +
          abs (points [i-1][0] - points [0][0]) +
          abs (points [i-1][1] - points [0][1]))
          i--;
    }
  }
  return i;
}

//points[i] [0] is x-coordinate, points [i] [1] is y-coordinate
int polygon (int** points, int sizePoints)
{
  if (sizePoints < 3) return sizePoints;
  return grahamScan (points, sizePoints);
}

static
int* getDegrees (const CanonicalForm& F, int& sizeOfOutput)
{
  if (F.inCoeffDomain())
  {
    int* result= new int [1];
    result [0]= 0;
    sizeOfOutput= 1;
    return result;
  }
  sizeOfOutput= size (F);
  int* result= new int [sizeOfOutput];
  int j= 0;
  for (CFIterator i= F; i.hasTerms(); i++, j++)
    result [j]= i.exp();
  return result;
}

//get points in Z^2 whose convex hull is the Newton polygon
int ** getPoints (const CanonicalForm& F, int& n)
{
  n= size (F);
  int ** points= new int* [n];
  for (int i= 0; i < n; i++)
    points [i]= new int [2];

  int j= 0;
  int * buf;
  int bufSize;
  if (F.isUnivariate() && F.level() == 1)
  {
    for (CFIterator i= F; i.hasTerms(); i++, j++)
    {
      points [j] [0]= i.exp();
      points [j] [1]= 0;
    }
    return points;
  }
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    buf= getDegrees (i.coeff(), bufSize);
    for (int k= 0; k < bufSize; k++, j++)
    {
      points [j] [0]= i.exp();
      points [j] [1]= buf [k];
    }
    delete [] buf;
  }
  return points;
}

int **
merge (int ** points1, int sizePoints1, int ** points2, int sizePoints2,
       int& sizeResult)
{
  int i, j;
  sizeResult= sizePoints1+sizePoints2;
  for (i= 0; i < sizePoints1; i++)
  {
    for (j= 0; j < sizePoints2; j++)
    {
      if (points1[i][0] != points2[j][0])
        continue;
      else
      {
        if (points1[i][1] != points2[j][1])
          continue;
        else
        {
          points2[j][0]= -1;
          points2[j][1]= -1;
          sizeResult--;
        }
      }
    }
  }
  if (sizeResult == 0)
    return points1;

  int ** result= new int *[sizeResult];
  for (i= 0; i < sizeResult; i++)
    result [i]= new int [2];

  int k= 0;
  for (i= 0; i < sizePoints1; i++, k++)
  {
    result[k][0]= points1[i][0];
    result[k][1]= points1[i][1];
  }
  for (i= 0; i < sizePoints2; i++)
  {
    if (points2[i][0] < 0)
      continue;
    else
    {
      result[k][0]= points2[i][0];
      result[k][1]= points2[i][1];
      k++;
    }
  }
  return result;
}

// assumes a bivariate poly as input
int ** newtonPolygon (const CanonicalForm& F, int& sizeOfNewtonPoly)
{
  int sizeF= size (F);
  int ** points= new int* [sizeF];
  for (int i= 0; i < sizeF; i++)
    points [i]= new int [2];
  int j= 0;
  int * buf;
  int bufSize;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    buf= getDegrees (i.coeff(), bufSize);
    for (int k= 0; k < bufSize; k++, j++)
    {
      points [j] [0]= i.exp();
      points [j] [1]= buf [k];
    }
    delete [] buf;
  }

  int n= polygon (points, sizeF);

  int ** result= new int* [n];
  for (int i= 0; i < n; i++)
  {
    result [i]= new int [2];
    result [i] [0]= points [i] [0];
    result [i] [1]= points [i] [1];
  }

  sizeOfNewtonPoly= n;
  for (int i= 0; i < sizeF; i++)
    delete [] points[i];
  delete [] points;

  return result;
}

// assumes a bivariate polys as input
int ** newtonPolygon (const CanonicalForm& F, const CanonicalForm& G,
                      int& sizeOfNewtonPoly)
{
  int sizeF= size (F);
  int ** pointsF= new int* [sizeF];
  for (int i= 0; i < sizeF; i++)
    pointsF [i]= new int [2];
  int j= 0;
  int * buf;
  int bufSize;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    buf= getDegrees (i.coeff(), bufSize);
    for (int k= 0; k < bufSize; k++, j++)
    {
      pointsF [j] [0]= i.exp();
      pointsF [j] [1]= buf [k];
    }
    delete [] buf;
  }

  int sizeG= size (G);
  int ** pointsG= new int* [sizeG];
  for (int i= 0; i < sizeG; i++)
    pointsG [i]= new int [2];
  j= 0;
  for (CFIterator i= G; i.hasTerms(); i++)
  {
    buf= getDegrees (i.coeff(), bufSize);
    for (int k= 0; k < bufSize; k++, j++)
    {
      pointsG [j] [0]= i.exp();
      pointsG [j] [1]= buf [k];
    }
    delete [] buf;
  }

  int sizePoints;
  int ** points= merge (pointsF, sizeF, pointsG, sizeG, sizePoints);

  int n= polygon (points, sizePoints);

  int ** result= new int* [n];
  for (int i= 0; i < n; i++)
  {
    result [i]= new int [2];
    result [i] [0]= points [i] [0];
    result [i] [1]= points [i] [1];
  }

  sizeOfNewtonPoly= n;
  for (int i= 0; i < sizeF; i++)
    delete [] pointsF[i];
  delete [] pointsF;
  for (int i= 0; i < sizeG; i++)
    delete [] pointsG[i];
  delete [] pointsG;

  return result;
}

// assumes first sizePoints entries of points form a Newton polygon
bool isInPolygon (int ** points, int sizePoints, int* point)
{
  int ** buf= new int* [sizePoints + 1];
  for (int i= 0; i < sizePoints; i++)
  {
    buf [i]= new int [2];
    buf [i] [0]= points [i] [0];
    buf [i] [1]= points [i] [1];
  }
  buf [sizePoints]= new int [2];
  buf [sizePoints] [0]= point [0];
  buf [sizePoints] [1]= point [1];
  int sizeBuf= sizePoints + 1;

  swap (buf, 0, smallestPointIndex (buf, sizeBuf));
  int * minusPoint= new int [2];
  minusPoint [0]= buf[0] [0];
  minusPoint [1]= buf[0] [1];
  translate (buf, minusPoint, sizeBuf);
  sort (buf, sizeBuf);
  minusPoint[0]= - minusPoint[0];
  minusPoint[1]= - minusPoint[1];
  translate (buf, minusPoint, sizeBuf); //reverse translation
  delete [] minusPoint;

  if (buf [0] [0] == point [0] && buf [0] [1] == point [1])
  {
    for (int i= 0; i < sizeBuf; i++)
      delete [] buf[i];
    delete [] buf;
    return false;
  }

  for (int i= 1; i < sizeBuf-1; i++)
  {
    if (buf [i] [0] == point [0] && buf [i] [1] == point [1])
    {
      bool result= !isConvex (buf, i);
      for (int i= 0; i < sizeBuf; i++)
        delete [] buf [i];
      delete [] buf;
      return result;
    }
  }

  if (buf [sizeBuf - 1] [0] == point [0] && buf [sizeBuf-1] [1] == point [1])
  {
    buf [1] [0]= point [0];
    buf [1] [1]= point [1];
    buf [2] [0]= buf [0] [0];
    buf [2] [1]= buf [0] [1];
    buf [0] [0]= buf [sizeBuf-2] [0];
    buf [0] [1]= buf [sizeBuf-2] [1];
    bool result= !isConvex (buf, 1);
    for (int i= 0; i < sizeBuf; i++)
      delete [] buf [i];
    delete [] buf;
    return result;
  }
  for (int i= 0; i < sizeBuf; i++)
    delete [] buf [i];
  delete [] buf;

  return false;
}

void lambda (int** points, int sizePoints)
{
  for (int i= 0; i < sizePoints; i++)
    points [i] [1]= points [i] [1] - points [i] [0];
}

void lambdaInverse (int** points, int sizePoints)
{
  for (int i= 0; i < sizePoints; i++)
    points [i] [1]= points [i] [1] + points [i] [0];
}

void tau (int** points, int sizePoints, int k)
{
  for (int i= 0; i < sizePoints; i++)
    points [i] [1]= points [i] [1] + k;
}

void mu (int** points, int sizePoints)
{
  int tmp;
  for (int i= 0; i < sizePoints; i++)
  {
    tmp= points [i] [0];
    points [i] [0]= points [i] [1];
    points [i] [1]= tmp;
  }
}

void getMaxMin (int** points, int sizePoints, int& minDiff, int& minSum,
                int& maxDiff, int& maxSum, int& maxX, int& maxY
               )
{
  minDiff= points [0] [1] - points [0] [0];
  minSum= points [0] [1] + points [0] [0];
  maxDiff= points [0] [1] - points [0] [0];
  maxSum= points [0] [1] + points [0] [0];
  maxX= points [0] [1];
  maxY= points [0] [0];
  int diff, sum;
  for (int i= 1; i < sizePoints; i++)
  {
    diff= points [i] [1] - points [i] [0];
    sum= points [i] [1] + points [i] [0];
    minDiff= tmin (minDiff, diff);
    minSum= tmin (minSum, sum);
    maxDiff= tmax (maxDiff, diff);
    maxSum= tmax (maxSum, sum);
    maxX= tmax (maxX, points [i] [1]);
    maxY= tmax (maxY, points [i] [0]);
  }
}

void mpz_mat_mul (const mpz_t* N, mpz_t*& M)
{
  mpz_t * tmp= new mpz_t[4];

  mpz_init_set (tmp[0], N[0]);
  mpz_mul (tmp[0], tmp[0], M[0]);
  mpz_addmul (tmp[0], N[1], M[2]);

  mpz_init_set (tmp[1], N[0]);
  mpz_mul (tmp[1], tmp[1], M[1]);
  mpz_addmul (tmp[1], N[1], M[3]);

  mpz_init_set (tmp[2], N[2]);
  mpz_mul (tmp[2], tmp[2], M[0]);
  mpz_addmul (tmp[2], N[3], M[2]);

  mpz_init_set (tmp[3], N[2]);
  mpz_mul (tmp[3], tmp[3], M[1]);
  mpz_addmul (tmp[3], N[3], M[3]);

  mpz_set (M[0], tmp[0]);
  mpz_set (M[1], tmp[1]);
  mpz_set (M[2], tmp[2]);
  mpz_set (M[3], tmp[3]);

  mpz_clear (tmp[0]);
  mpz_clear (tmp[1]);
  mpz_clear (tmp[2]);
  mpz_clear (tmp[3]);

  delete [] tmp;
}

void mpz_mat_inv (mpz_t*& M)
{
  mpz_t det;
  mpz_init_set (det, M[0]);
  mpz_mul (det, det, M[3]);
  mpz_submul (det, M[1], M[2]);

  mpz_t tmp;
  mpz_init_set (tmp, M[0]);
  mpz_divexact (tmp, tmp, det);
  mpz_set (M[0], M[3]);
  mpz_divexact (M[0], M[0], det);
  mpz_set (M[3], tmp);

  mpz_neg (M[1], M[1]);
  mpz_divexact (M[1], M[1], det);
  mpz_neg (M[2], M[2]);
  mpz_divexact (M[2], M[2], det);

  mpz_clear (det);
  mpz_clear (tmp);
}

void convexDense(int** points, int sizePoints, mpz_t*& M, mpz_t*& A)
{
  if (sizePoints < 3)
  {
    if (sizePoints == 2)
    {
      mpz_t u,v,g,maxX,maxY;
      mpz_init (u);
      mpz_init (v);
      mpz_init (g);
      mpz_init_set_si (maxX,
                       (points[1][1] < points[0][1])?points[0][1]:points[1][1]);
      mpz_init_set_si (maxY,
                       (points[1][0] < points[0][0])?points[0][0]:points[1][0]);
      mpz_gcdext (g, u, v, maxX, maxY);
      if (points [0] [1] != points [0] [0] && points [1] [0] != points [1] [1])
      {
        mpz_set (A[0], u);
        mpz_mul (A[0], A[0], maxX);
        mpz_set (M[2], maxY);
        mpz_divexact (M[2], M[2], g);
        mpz_set (A[1], M[2]);
        mpz_neg (A[1], A[1]);
        mpz_mul (A[1], A[1], maxX);
        mpz_neg (u, u);
        mpz_set (M[0], u);
        mpz_set (M[1], v);
        mpz_set (M[3], maxX);
        mpz_divexact (M[3], M[3], g);
      }
      else
      {
        mpz_set (M[0], u);
        mpz_set (M[1], v);
        mpz_set (M[2], maxY);
        mpz_divexact (M[2], M[2], g);
        mpz_neg (M[2], M[2]);
        mpz_set (M[3], maxX);
        mpz_divexact (M[3], M[3], g);
      }
      mpz_clear (u);
      mpz_clear (v);
      mpz_clear (g);
      mpz_clear (maxX);
      mpz_clear (maxY);
    }
    else if (sizePoints == 1)
    {
      mpz_set_si (M[0], 1);
      mpz_set_si (M[3], 1);
    }
    return;
  }
  mpz_set_si (M[0], 1);
  mpz_set_si (M[3], 1);

  mpz_t * Mu= new mpz_t[4];
  mpz_init_set_si (Mu[1], 1);
  mpz_init_set_si (Mu[2], 1);
  mpz_init (Mu[0]);
  mpz_init (Mu[3]);

  mpz_t * Lambda= new mpz_t[4];
  mpz_init_set_si (Lambda[0], 1);
  mpz_init_set_si (Lambda[1], -1);
  mpz_init_set_si (Lambda[3], 1);
  mpz_init (Lambda[2]);

  mpz_t * InverseLambda= new mpz_t[4];
  mpz_init_set_si (InverseLambda[0], 1);
  mpz_init_set_si (InverseLambda[1], 1);
  mpz_init_set_si (InverseLambda[3], 1);
  mpz_init (InverseLambda[2]);

  mpz_t tmp;
  mpz_init (tmp);
  int minDiff, minSum, maxDiff, maxSum, maxX, maxY, b, d, f, h;
  getMaxMin(points, sizePoints, minDiff, minSum, maxDiff, maxSum, maxX, maxY);
  do
  {
    if (maxX < maxY)
    {
      mu (points, sizePoints);

      mpz_mat_mul (Mu, M);

      mpz_set (tmp, A[0]);
      mpz_set (A[0], A[1]);
      mpz_set (A[1], tmp);
    }
    getMaxMin(points, sizePoints, minDiff, minSum, maxDiff, maxSum, maxX, maxY);
    b= maxX - maxDiff;
    d= maxX + maxY - maxSum;
    f= maxY + minDiff;
    h= minSum;
    if (b + f > maxY)
    {
      lambda (points, sizePoints);
      tau (points, sizePoints, maxY - f);

      mpz_mat_mul (Lambda, M);

      if (maxY-f > 0)
        mpz_add_ui (A[0], A[0], maxY-f);
      else
        mpz_add_ui (A[0], A[0], f-maxY);
      maxX= maxX + maxY - b - f;
    }
    else if (d + h > maxY)
    {
      lambdaInverse (points, sizePoints);
      tau (points, sizePoints, -h);

      mpz_mat_mul (InverseLambda, M);

      if (h < 0)
        mpz_add_ui (A[0], A[0], -h);
      else
        mpz_sub_ui (A[0], A[0], h);
      maxX= maxX + maxY - d - h;
    }
    else
    {
      mpz_clear (tmp);
      mpz_clear (Mu[0]);
      mpz_clear (Mu[1]);
      mpz_clear (Mu[2]);
      mpz_clear (Mu[3]);
      delete [] Mu;

      mpz_clear (Lambda[0]);
      mpz_clear (Lambda[1]);
      mpz_clear (Lambda[2]);
      mpz_clear (Lambda[3]);
      delete [] Lambda;

      mpz_clear (InverseLambda[0]);
      mpz_clear (InverseLambda[1]);
      mpz_clear (InverseLambda[2]);
      mpz_clear (InverseLambda[3]);
      delete [] InverseLambda;

      return;
    }
  } while (1);
}

CanonicalForm
compress (const CanonicalForm& F, mpz_t*& M, mpz_t*& A, bool computeMA)
{
  int n;
  int ** newtonPolyg= NULL;
  if (computeMA)
  {
    newtonPolyg= newtonPolygon (F, n);
    convexDense (newtonPolyg, n, M, A);
  }

  CanonicalForm result= 0;
  Variable x= Variable (1);
  Variable y= Variable (2);

  mpz_t expX, expY, minExpX, minExpY;
  mpz_init (expX);
  mpz_init (expY);
  mpz_init (minExpX);
  mpz_init (minExpY);

  int k= 0;
  Variable alpha;
  mpz_t * exps= new mpz_t [2*size (F)];
  int count= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      mpz_set (expX, A[0]);
      mpz_set (expY, A[1]);
      mpz_addmul_ui (expX, M[1], i.exp());
      mpz_addmul_ui (expY, M[3], i.exp());

      if (k == 0)
      {
        mpz_set (minExpX, expX);
        mpz_set (minExpY, expY);
        k= 1;
      }
      else
      {
        if (mpz_cmp (minExpY, expY) > 0)
          mpz_set (minExpY, expY);
        if (mpz_cmp (minExpX, expX) > 0)
          mpz_set (minExpX, expX);
      }
      mpz_init_set (exps[count], expX);
      count++;
      mpz_init_set (exps[count], expY);
      count++;
      continue;
    }
    CFIterator j= i.coeff();
    if (k == 0)
    {
      mpz_set (expX, A[0]);
      mpz_addmul_ui (expX, M[1], i.exp());
      mpz_addmul_ui (expX, M[0], j.exp());

      mpz_set (expY, A[1]);
      mpz_addmul_ui (expY, M[3], i.exp());
      mpz_addmul_ui (expY, M[2], j.exp());

      mpz_set (minExpX, expX);
      mpz_set (minExpY, expY);
      mpz_init_set (exps[count], expX);
      count++;
      mpz_init_set (exps[count], expY);
      count++;
      j++;
      k= 1;
    }

    for (; j.hasTerms(); j++)
    {
      mpz_set (expX, A[0]);
      mpz_addmul_ui (expX, M[1], i.exp());
      mpz_addmul_ui (expX, M[0], j.exp());

      mpz_set (expY, A[1]);
      mpz_addmul_ui (expY, M[3], i.exp());
      mpz_addmul_ui (expY, M[2], j.exp());

      mpz_init_set (exps[count], expX);
      count++;
      mpz_init_set (exps[count], expY);
      count++;
      if (mpz_cmp (minExpY, expY) > 0)
        mpz_set (minExpY, expY);
      if (mpz_cmp (minExpX, expX) > 0)
        mpz_set (minExpX, expX);
    }
  }

  count= 0;
  int mExpX= mpz_get_si (minExpX);
  int mExpY= mpz_get_si (minExpY);
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      result += i.coeff()*power (x, mpz_get_si (exps[count])-mExpX)*
                power (y, mpz_get_si (exps[count+1])-mExpY);
      count += 2;
      continue;
    }
    CFIterator j= i.coeff();
    for (; j.hasTerms(); j++)
    {
      result += j.coeff()*power (x, mpz_get_si (exps[count])-mExpX)*
                power (y, mpz_get_si (exps[count+1])-mExpY);
      count += 2;
    }
  }

  CanonicalForm tmp= LC (result);
  if (tmp.inPolyDomain() && degree (tmp) <= 0)
  {
    int d= degree (result);
    Variable x= result.mvar();
    result -= tmp*power (x, d);
    result += Lc (tmp)*power (x, d);
  }

  if (computeMA)
  {
    for (int i= 0; i < n; i++)
      delete [] newtonPolyg [i];
    delete [] newtonPolyg;
    mpz_mat_inv (M);
  }

  delete [] exps;
  mpz_clear (expX);
  mpz_clear (expY);
  mpz_clear (minExpX);
  mpz_clear (minExpY);

  return result;
}

CanonicalForm
decompress (const CanonicalForm& F, const mpz_t* inverseM, const mpz_t * A)
{
  CanonicalForm result= 0;
  Variable x= Variable (1);
  Variable y= Variable (2);

  mpz_t expX, expY, minExpX, minExpY;
  mpz_init (expX);
  mpz_init (expY);
  mpz_init (minExpX);
  mpz_init (minExpY);

  mpz_t * exps= new mpz_t [2*size(F)];
  int count= 0;
  if (F.isUnivariate() && F.level() == 1)
  {
    CFIterator i= F;

    mpz_set_si (expX, i.exp());
    mpz_sub (expX, expX, A[0]);
    mpz_mul (expX, expX, inverseM[0]);
    mpz_submul (expX, inverseM[1], A[1]);

    mpz_set_si (expY, i.exp());
    mpz_sub (expY, expY, A[0]);
    mpz_mul (expY, expY, inverseM[2]);
    mpz_submul (expY, inverseM[3], A[1]);

    mpz_set (minExpX, expX);
    mpz_set (minExpY, expY);

    mpz_init_set (exps[count], expX);
    mpz_init_set (exps[count+1], expY);
    count += 2;

    i++;
    for (; i.hasTerms(); i++)
    {
      mpz_set_si (expX, i.exp());
      mpz_sub (expX, expX, A[0]);
      mpz_mul (expX, expX, inverseM[0]);
      mpz_submul (expX, inverseM[1], A[1]);

      mpz_set_si (expY, i.exp());
      mpz_sub (expY, expY, A[0]);
      mpz_mul (expY, expY, inverseM[2]);
      mpz_submul (expY, inverseM[3], A[1]);

      mpz_init_set (exps[count], expX);
      mpz_init_set (exps[count+1], expY);
      count += 2;

      if (mpz_cmp (minExpY, expY) > 0)
        mpz_set (minExpY, expY);
      if (mpz_cmp (minExpX, expX) > 0)
        mpz_set (minExpX, expX);
    }

    int mExpX= mpz_get_si (minExpX);
    int mExpY= mpz_get_si (minExpY);
    count= 0;
    for (i= F; i.hasTerms(); i++)
    {
      result += i.coeff()*power (x, mpz_get_si (exps[count])-mExpX)*
                power (y, mpz_get_si (exps[count+1])-mExpY);
      count += 2;
    }

    mpz_clear (expX);
    mpz_clear (expY);
    mpz_clear (minExpX);
    mpz_clear (minExpY);

    delete [] exps;
    return result/ Lc (result); //normalize
  }

  mpz_t tmp;
  mpz_init (tmp);
  int k= 0;
  Variable alpha;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      mpz_set_si (expX, i.exp());
      mpz_sub (expX, expX, A[1]);
      mpz_mul (expX, expX, inverseM[1]);
      mpz_submul (expX, A[0], inverseM[0]);

      mpz_set_si (expY, i.exp());
      mpz_sub (expY, expY, A[1]);
      mpz_mul (expY, expY, inverseM[3]);
      mpz_submul (expY, A[0], inverseM[2]);

      if (k == 0)
      {
        mpz_set (minExpX, expX);
        mpz_set (minExpY, expY);
        k= 1;
      }
      else
      {
        if (mpz_cmp (minExpY, expY) > 0)
          mpz_set (minExpY, expY);
        if (mpz_cmp (minExpX, expX) > 0)
          mpz_set (minExpX, expX);
      }
      mpz_init_set (exps[count], expX);
      mpz_init_set (exps[count+1], expY);
      count += 2;
      continue;
    }
    CFIterator j= i.coeff();
    if (k == 0)
    {
      mpz_set_si (expX, j.exp());
      mpz_sub (expX, expX, A[0]);
      mpz_mul (expX, expX, inverseM[0]);
      mpz_set_si (tmp, i.exp());
      mpz_sub (tmp, tmp, A[1]);
      mpz_addmul (expX, tmp, inverseM[1]);

      mpz_set_si (expY, j.exp());
      mpz_sub (expY, expY, A[0]);
      mpz_mul (expY, expY, inverseM[2]);
      mpz_set_si (tmp, i.exp());
      mpz_sub (tmp, tmp, A[1]);
      mpz_addmul (expY, tmp, inverseM[3]);

      mpz_set (minExpX, expX);
      mpz_set (minExpY, expY);

      mpz_init_set (exps[count], expX);
      mpz_init_set (exps[count+1], expY);
      count += 2;

      j++;
      k= 1;
    }

    for (; j.hasTerms(); j++)
    {
      mpz_set_si (expX, j.exp());
      mpz_sub (expX, expX, A[0]);
      mpz_mul (expX, expX, inverseM[0]);
      mpz_set_si (tmp, i.exp());
      mpz_sub (tmp, tmp, A[1]);
      mpz_addmul (expX, tmp, inverseM[1]);

      mpz_set_si (expY, j.exp());
      mpz_sub (expY, expY, A[0]);
      mpz_mul (expY, expY, inverseM[2]);
      mpz_set_si (tmp, i.exp());
      mpz_sub (tmp, tmp, A[1]);
      mpz_addmul (expY, tmp, inverseM[3]);

      mpz_init_set (exps[count], expX);
      mpz_init_set (exps[count+1], expY);
      count += 2;

      if (mpz_cmp (minExpY, expY) > 0)
        mpz_set (minExpY, expY);
      if (mpz_cmp (minExpX, expX) > 0)
        mpz_set (minExpX, expX);
    }
  }

  int mExpX= mpz_get_si (minExpX);
  int mExpY= mpz_get_si (minExpY);
  count= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      result += i.coeff()*power (x, mpz_get_si (exps[count])-mExpX)*
                power (y, mpz_get_si (exps[count+1])-mExpY);
      count += 2;
      continue;
    }
    for (CFIterator j= i.coeff(); j.hasTerms(); j++)
    {
      result += j.coeff()*power (x, mpz_get_si (exps[count])-mExpX)*
                power (y, mpz_get_si (exps[count+1])-mExpY);
      count += 2;
    }
  }

  mpz_clear (expX);
  mpz_clear (expY);
  mpz_clear (minExpX);
  mpz_clear (minExpY);
  mpz_clear (tmp);

  delete [] exps;

  return result/Lc (result); //normalize
}

//assumes the input is a Newton polygon of a bivariate polynomial which is
//primitive wrt. x and y, i.e. there is at least one point of the polygon lying
//on the x-axis and one lying on the y-axis
int* getRightSide (int** polygon, int sizeOfPolygon, int& sizeOfOutput)
{
  int maxY= polygon [0][0];
  int indexY= 0;
  for (int i= 1; i < sizeOfPolygon; i++)
  {
    if (maxY < polygon [i][0])
    {
      maxY= polygon [i][0];
      indexY= i;
    }
    else if (maxY == polygon [i][0])
    {
      if (polygon [indexY][1] < polygon[i][1])
        indexY= i;
    }
    if (maxY > polygon [i][0])
      break;
  }

  int count= -1;
  for (int i= indexY; i < sizeOfPolygon; i++)
  {
    if (polygon[i][0] == 0)
    {
      count= i - indexY;
      break;
    }
  }

  int * result;
  int index= 0;
  if (count < 0)
  {
    result= new int [sizeOfPolygon - indexY];
    sizeOfOutput= sizeOfPolygon - indexY;
    count= sizeOfPolygon - indexY - 1;
    result [0]= polygon[sizeOfPolygon - 1][0] - polygon [0] [0];
    index= 1;
  }
  else
  {
    sizeOfOutput= count;
    result= new int [count];
  }

  for (int i= indexY + count; i > indexY; i--, index++)
    result [index]= polygon [i - 1] [0] - polygon [i] [0];

  return result;
}

bool irreducibilityTest (const CanonicalForm& F)
{
  ASSERT (getNumVars (F) == 2, "expected bivariate polynomial");
  ASSERT (getCharacteristic() == 0, "expected polynomial over integers or rationals");

  int sizeOfNewtonPolygon;
  int ** newtonPolyg= newtonPolygon (F, sizeOfNewtonPolygon);
  if (sizeOfNewtonPolygon == 3)
  {
    bool check1=
        (newtonPolyg[0][0]==0 || newtonPolyg[1][0]==0 || newtonPolyg[2][0]==0);
    if (check1)
    {
      bool check2=
        (newtonPolyg[0][1]==0 || newtonPolyg[1][1]==0 || newtonPolyg[2][0]==0);
      if (check2)
      {
        bool isRat= isOn (SW_RATIONAL);
        if (isRat)
          Off (SW_RATIONAL);
        CanonicalForm tmp= gcd (newtonPolyg[0][0],newtonPolyg[0][1]); // maybe it's better to use plain intgcd
        tmp= gcd (tmp, newtonPolyg[1][0]);
        tmp= gcd (tmp, newtonPolyg[1][1]);
        tmp= gcd (tmp, newtonPolyg[2][0]);
        tmp= gcd (tmp, newtonPolyg[2][1]);
        if (isRat)
          On (SW_RATIONAL);
        for (int i= 0; i < sizeOfNewtonPolygon; i++)
          delete [] newtonPolyg [i];
        delete [] newtonPolyg;
        return (tmp==1);
      }
    }
  }
  for (int i= 0; i < sizeOfNewtonPolygon; i++)
    delete [] newtonPolyg [i];
  delete [] newtonPolyg;
  return false;
}

bool absIrredTest (const CanonicalForm& F)
{
  ASSERT (getNumVars (F) == 2, "expected bivariate polynomial");
  ASSERT (factorize (F).length() <= 2, " expected irreducible polynomial");

  int sizeOfNewtonPolygon;
  int ** newtonPolyg= newtonPolygon (F, sizeOfNewtonPolygon);
  bool isRat= isOn (SW_RATIONAL);
  if (isRat)
    Off (SW_RATIONAL);
  int p=getCharacteristic();
  int d=1;
  char bufGFName='Z';
  bool GF= (CFFactory::gettype()==GaloisFieldDomain);
  if (GF)
  {
    d= getGFDegree();
    bufGFName=gf_name;
  }

  setCharacteristic(0);

  CanonicalForm g= gcd (newtonPolyg[0][0], newtonPolyg[0][1]); //maybe it's better to use plain intgcd

  int i= 1;
  while (!g.isOne() && i < sizeOfNewtonPolygon)
  {
    g= gcd (g, newtonPolyg[i][0]);
    g= gcd (g, newtonPolyg[i][1]);
    i++;
  }

  bool result= g.isOne();

  if (GF)
    setCharacteristic (p, d, bufGFName);
  else
    setCharacteristic(p);

  if (isRat)
    On (SW_RATIONAL);

  for (int i= 0; i < sizeOfNewtonPolygon; i++)
    delete [] newtonPolyg[i];

  delete [] newtonPolyg;

  return result;
}

bool modularIrredTest (const CanonicalForm& F)
{
  ASSERT (getNumVars (F) == 2, "expected bivariate polynomial");
  ASSERT (factorize (F).length() <= 2, " expected irreducible polynomial");

  bool isRat= isOn (SW_RATIONAL);
  if (isRat)
    Off (SW_RATIONAL);

  if (isRat)
  {
    ASSERT (bCommonDen (F).isOne(), "poly over Z expected");
  }

  CanonicalForm Fp, N= maxNorm (F);
  int tdeg= totaldegree (F);

  int i= 0;
  //TODO: maybe it's better to choose the characteristic as large as possible
  //      as factorization over large finite field will be faster
  //TODO: handle those cases where our factory primes are not enough
  //TODO: factorize coefficients corresponding to the vertices of the Newton
  //      polygon and only try the obtained factors
  if (N < cf_getSmallPrime (cf_getNumSmallPrimes()-1))
  {
    while (i < cf_getNumSmallPrimes() && N > cf_getSmallPrime(i))
    {
      setCharacteristic (cf_getSmallPrime (i));
      Fp= F.mapinto();
      i++;
      if (totaldegree (Fp) == tdeg)
      {
        if (absIrredTest (Fp))
        {
          CFFList factors= factorize (Fp);
          if (factors.length() == 2 && factors.getLast().exp() == 1)
          {
            if (isRat)
              On (SW_RATIONAL);
            setCharacteristic (0);
            return true;
          }
        }
      }
      setCharacteristic (0);
    }
  }
  else
  {
    while (i < cf_getNumPrimes() && N > cf_getPrime (i))
    {
      setCharacteristic (cf_getPrime (i));
      Fp= F.mapinto();
      i++;
      if (totaldegree (Fp) == tdeg)
      {
        if (absIrredTest (Fp))
        {
          CFFList factors= factorize (Fp);
          if (factors.length() == 2 && factors.getLast().exp() == 1)
          {
            if (isRat)
              On (SW_RATIONAL);
            setCharacteristic (0);
            return true;
          }
        }
      }
      setCharacteristic (0);
    }
  }

  if (isRat)
    On (SW_RATIONAL);

  return false;
}

bool
modularIrredTestWithShift (const CanonicalForm& F)
{
  ASSERT (getNumVars (F) == 2, "expected bivariate polynomial");
  ASSERT (factorize (F).length() <= 2, " expected irreducible polynomial");

  bool isRat= isOn (SW_RATIONAL);
  if (isRat)
    Off (SW_RATIONAL);

  if (isRat)
  {
    ASSERT (bCommonDen (F).isOne(), "poly over Z expected");
  }

  Variable x= Variable (1);
  Variable y= Variable (2);
  CanonicalForm Fp;
  int tdeg= totaldegree (F);

  REvaluation E;

  setCharacteristic (2);
  Fp= F.mapinto();

  E= REvaluation (1,2, FFRandom());

  E.nextpoint();

  Fp= Fp (x+E[1], x);
  Fp= Fp (y+E[2], y);

  if (tdeg == totaldegree (Fp))
  {
    if (absIrredTest (Fp))
    {
      CFFList factors= factorize (Fp);
      if (factors.length() == 2 && factors.getLast().exp() == 1)
      {
        if (isRat)
          On (SW_RATIONAL);
        setCharacteristic (0);
        return true;
      }
    }
  }

  E.nextpoint();

  Fp= Fp (x+E[1], x);
  Fp= Fp (y+E[2], y);

  if (tdeg == totaldegree (Fp))
  {
    if (absIrredTest (Fp))
    {
      CFFList factors= factorize (Fp);
      if (factors.length() == 2 && factors.getLast().exp() == 1)
      {
        if (isRat)
          On (SW_RATIONAL);
        setCharacteristic (0);
        return true;
      }
    }
  }

  int i= 0;
  while (cf_getSmallPrime (i) < 102)
  {
    setCharacteristic (cf_getSmallPrime (i));
    i++;
    E= REvaluation (1, 2, FFRandom());

    for (int j= 0; j < 3; j++)
    {
      Fp= F.mapinto();
      E.nextpoint();
      Fp= Fp (x+E[1], x);
      Fp= Fp (y+E[2], y);

      if (tdeg == totaldegree (Fp))
      {
        if (absIrredTest (Fp))
        {
          CFFList factors= factorize (Fp);
          if (factors.length() == 2 && factors.getLast().exp() == 1)
          {
            if (isRat)
              On (SW_RATIONAL);
            setCharacteristic (0);
            return true;
          }
        }
      }
    }
  }

  setCharacteristic (0);
  if (isRat)
    On (SW_RATIONAL);

  return false;
}


