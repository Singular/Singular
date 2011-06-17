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
 * @internal
 * @version \$Id$
 *
 **/
/*****************************************************************************/

#include <config.h>
#include <stdlib.h>

#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_algorithm.h"
#include "cfNewtonPolygon.h"
#include "templates/ftmpl_functions.h"
#include "algext.h"

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
  int area= point1[0]*point2[1]- point1[1]*point2[0];
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
  int relArea= (point1[0] - point2[0])*(point3[1] - point2[1]) -
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
  if (i + 1 < sizePoints)
  {
    int relArea=
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
  for (int i= n; i < sizeF; i++)
    delete [] points[i];
  delete [] points;

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

#ifdef HAVE_NTL
void convexDense(int** points, int sizePoints, mat_ZZ& M, vec_ZZ& A)
{
  if (sizePoints < 3)
  {
    if (sizePoints == 2)
    {
      int maxX= (points [1] [1] < points [0] [1])?points [0] [1]:points [1] [1];
      int maxY= (points [1] [0] < points [0] [0])?points [0] [0]:points [1] [0];
      long u,v,g;
      XGCD (g, u, v, maxX, maxY);
      M.SetDims (2,2);
      A.SetLength (2);
      if (points [0] [1] != points [0] [0] && points [1] [0] != points [1] [1])
      {
        M (1,1)= -u;
        M (1,2)= v;
        M (2,1)= maxY/g;
        M (2,2)= maxX/g;
        A (1)= u*maxX;
        A (2)= -(maxY/g)*maxX;
      }
      else
      {
        M (1,1)= u;
        M (1,2)= v;
        M (2,1)= -maxY/g;
        M (2,2)= maxX/g;
        A (1)= to_ZZ (0);
        A (2)= to_ZZ (0);
      }
    }
    else if (sizePoints == 1)
    {
      ident (M, 2);
      A.SetLength (2);
      A (1)= to_ZZ (0);
      A (2)= to_ZZ (0);
    }
    return;
  }
  A.SetLength (2);
  A (1)= to_ZZ (0);
  A (2)= to_ZZ (0);
  ident (M, 2);
  mat_ZZ Mu;
  Mu.SetDims (2, 2);
  Mu (2,1)= to_ZZ (1);
  Mu (1,2)= to_ZZ (1);
  Mu (1,1)= to_ZZ (0);
  Mu (2,2)= to_ZZ (0);
  mat_ZZ Lambda;
  Lambda.SetDims (2, 2);
  Lambda (1,1)= to_ZZ (1);
  Lambda (1,2)= to_ZZ (-1);
  Lambda (2,2)= to_ZZ (1);
  Lambda (2,1)= to_ZZ (0);
  mat_ZZ InverseLambda;
  InverseLambda.SetDims (2,2);
  InverseLambda (1,1)= to_ZZ (1);
  InverseLambda (1,2)= to_ZZ (1);
  InverseLambda (2,2)= to_ZZ (1);
  InverseLambda (2,1)= to_ZZ (0);
  ZZ tmp;
  int minDiff, minSum, maxDiff, maxSum, maxX, maxY, b, d, f, h;
  getMaxMin (points, sizePoints, minDiff, minSum, maxDiff, maxSum, maxX, maxY);
  do
  {
    if (maxX < maxY)
    {
      mu (points, sizePoints);
      M= Mu*M;
      tmp= A (1);
      A (1)= A (2);
      A (2)= tmp;
    }
    getMaxMin (points, sizePoints, minDiff, minSum, maxDiff, maxSum, maxX, maxY);
    b= maxX - maxDiff;
    d= maxX + maxY - maxSum;
    f= maxY + minDiff;
    h= minSum;
    if (b + f > maxY)
    {
      lambda (points, sizePoints);
      tau (points, sizePoints, maxY - f);
      M= Lambda*M;
      A [0] += (maxY-f);
      maxX= maxX + maxY - b - f;
    }
    else if (d + h > maxY)
    {
      lambdaInverse (points, sizePoints);
      tau (points, sizePoints, -h);
      M= InverseLambda*M;
      A [0] += (-h);
      maxX= maxX + maxY - d - h;
    }
    else
      return;
  } while (1);
}

CanonicalForm
compress (const CanonicalForm& F, mat_ZZ& M, vec_ZZ& A)
{
  int n;
  int ** newtonPolyg= newtonPolygon (F, n);
  convexDense (newtonPolyg, n, M, A);
  CanonicalForm result= 0;
  ZZ expX, expY;
  Variable x= Variable (1);
  Variable y= Variable (2);

  ZZ minExpX, minExpY;

  int k= 0;
  Variable alpha;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      expX= i.exp()*M (1,2) + A (1);
      expY= i.exp()*M (2,2) + A (2);
      if (k == 0)
      {
        minExpY= expY;
        minExpX= expX;
        k= 1;
      }
      else
      {
        minExpY= (minExpY > expY) ? expY : minExpY;
        minExpX= (minExpX > expX) ? expX : minExpX;
      }
      result += i.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      continue;
    }
    CFIterator j= i.coeff();
    if (k == 0)
    {
      expX= j.exp()*M (1,1) + i.exp()*M (1,2) + A (1);
      expY= j.exp()*M (2,1) + i.exp()*M (2,2) + A (2);
      minExpX= expX;
      minExpY= expY;
      result += j.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      j++;
      k= 1;
    }

    for (; j.hasTerms(); j++)
    {
      expX= j.exp()*M (1,1) + i.exp()*M (1,2) + A (1);
      expY= j.exp()*M (2,1) + i.exp()*M (2,2) + A (2);
      result += j.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      minExpY= (minExpY > expY) ? expY : minExpY;
      minExpX= (minExpX > expX) ? expX : minExpX;
    }
  }

  if (to_long (minExpX) < 0)
  {
    result *= power (x,-to_long(minExpX));
    result /= CanonicalForm (x, 0);
  }
  else
    result /= power (x,to_long(minExpX));

  if (to_long (minExpY) < 0)
  {
    result *= power (y,-to_long(minExpY));
    result /= CanonicalForm (y, 0);
  }
  else
    result /= power (y,to_long(minExpY));

  CanonicalForm tmp= LC (result);
  if (tmp.inPolyDomain() && degree (tmp) <= 0)
  {
    int d= degree (result);
    Variable x= result.mvar();
    result -= tmp*power (x, d);
    result += Lc (tmp)*power (x, d);
  }

  for (int i= 0; i < n; i++)
    delete [] newtonPolyg [i];
  delete [] newtonPolyg;

  M= inv (M);
  return result;
}

CanonicalForm
decompress (const CanonicalForm& F, const mat_ZZ& inverseM, const vec_ZZ& A)
{
  CanonicalForm result= 0;
  ZZ expX, expY;
  Variable x= Variable (1);
  Variable y= Variable (2);
  ZZ minExpX, minExpY;
  if (F.isUnivariate() && F.level() == 1)
  {
    CFIterator i= F;
    expX= (i.exp() - A (1))*inverseM (1,1) + (-A (2))*inverseM (1,2);
    expY= (i.exp() - A (1))*inverseM (2,1) + (-A (2))*inverseM (2,2);
    minExpX= expX;
    minExpY= expY;
    result += i.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
    i++;
    for (; i.hasTerms(); i++)
    {
      expX= (i.exp() - A (1))*inverseM (1,1) + (-A (2))*inverseM (1,2);
      expY= (i.exp() - A (1))*inverseM (2,1) + (-A (2))*inverseM (2,2);
      result += i.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      minExpY= (minExpY > expY) ? expY : minExpY;
      minExpX= (minExpX > expX) ? expX : minExpX;
    }

    if (to_long (minExpX) < 0)
    {
      result *= power (x,-to_long(minExpX));
      result /= CanonicalForm (x, 0);
    }
    else
      result /= power (x,to_long(minExpX));

    if (to_long (minExpY) < 0)
    {
      result *= power (y,-to_long(minExpY));
      result /= CanonicalForm (y, 0);
    }
    else
      result /= power (y,to_long(minExpY));

    return result/ Lc (result); //normalize
  }

  int k= 0;
  Variable alpha;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain() && hasFirstAlgVar (i.coeff(), alpha))
    {
      expX= -A(1)*inverseM (1,1) + (i.exp() - A (2))*inverseM (1,2);
      expY= -A(1)*inverseM (2,1) + (i.exp() - A (2))*inverseM (2,2);
      if (k == 0)
      {
        minExpY= expY;
        minExpX= expX;
        k= 1;
      }
      else
      {
        minExpY= (minExpY > expY) ? expY : minExpY;
        minExpX= (minExpX > expX) ? expX : minExpX;
      }
      result += i.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      continue;
    }
    CFIterator j= i.coeff();
    if (k == 0)
    {
      expX= (j.exp() - A (1))*inverseM (1,1) + (i.exp() - A (2))*inverseM (1,2);
      expY= (j.exp() - A (1))*inverseM (2,1) + (i.exp() - A (2))*inverseM (2,2);
      minExpX= expX;
      minExpY= expY;
      result += j.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      j++;
      k= 1;
    }

    for (; j.hasTerms(); j++)
    {
      expX= (j.exp() - A (1))*inverseM (1,1) + (i.exp() - A (2))*inverseM (1,2);
      expY= (j.exp() - A (1))*inverseM (2,1) + (i.exp() - A (2))*inverseM (2,2);
      result += j.coeff()*power (x, to_long (expX))*power (y, to_long (expY));
      minExpY= (minExpY > expY) ? expY : minExpY;
      minExpX= (minExpX > expX) ? expX : minExpX;
    }
  }

  if (to_long (minExpX) < 0)
  {
    result *= power (x,-to_long(minExpX));
    result /= CanonicalForm (x, 0);
  }
  else
    result /= power (x,to_long(minExpX));

  if (to_long (minExpY) < 0)
  {
    result *= power (y,-to_long(minExpY));
    result /= CanonicalForm (y, 0);
  }
  else
    result /= power (y,to_long(minExpY));

  return result/Lc (result); //normalize
}
#endif
