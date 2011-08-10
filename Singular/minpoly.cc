/***********************************************
 * Copyright (C) 2011 Sebastian Jambor         *
 * sebastian@momo.math.rwth-aachen.de          *
 ***********************************************/


#include<cmath>
#include<Singular/mod2.h>
//#include<iomanip>

#include "minpoly.h"

// TODO: avoid code copying, use subclassing instead!

LinearDependencyMatrix::LinearDependencyMatrix (unsigned n, unsigned long p)
{
  this->n = n;
  this->p = p;

  matrix = new unsigned long *[n];
  for(int i = 0; i < n; i++)
  {
    matrix[i] = new unsigned long[2 * n + 1];
  }
  pivots = new unsigned[n];
  tmprow = new unsigned long[2 * n + 1];
  rows = 0;
}

LinearDependencyMatrix::~LinearDependencyMatrix ()
{
  delete[]tmprow;
  delete[]pivots;

  for(int i = 0; i < n; i++)
  {
    delete[](matrix[i]);
  }
  delete[]matrix;
}

void LinearDependencyMatrix::resetMatrix ()
{
  rows = 0;
}

int LinearDependencyMatrix::firstNonzeroEntry (unsigned long *row)
{
  for(int i = 0; i < n; i++)
    if(row[i] != 0)
      return i;

  return -1;
}

void LinearDependencyMatrix::reduceTmpRow ()
{
  for(int i = 0; i < rows; i++)
  {
    unsigned piv = pivots[i];
    unsigned x = tmprow[piv];
    // if the corresponding entry in the row is zero, there is nothing to do
    if(x != 0)
    {
      // subtract tmprow[i] times the i-th row
      for(int j = piv; j < n + rows + 1; j++)
      {
        unsigned long tmp = multMod (matrix[i][j], x, p);
        tmp = p - tmp;
        tmprow[j] += tmp;
        tmprow[j] %= p;
      }
    }
  }
}


void LinearDependencyMatrix::normalizeTmp (unsigned i)
{
  unsigned long inv = modularInverse (tmprow[i], p);
  tmprow[i] = 1;
  for(int j = i + 1; j < 2 * n + 1; j++)
    tmprow[j] = multMod (tmprow[j], inv, p);
}

bool LinearDependencyMatrix::findLinearDependency (unsigned long *newRow,
                                                   unsigned long *dep)
{
  // Copy newRow to tmprow (we need to add RHS)
  for(int i = 0; i < n; i++)
  {
    tmprow[i] = newRow[i];
    tmprow[n + i] = 0;
  }
  tmprow[2 * n] = 0;
  tmprow[n + rows] = 1;

  reduceTmpRow ();

  // Is tmprow reduced to zero? Then we have found a linear dependence.
  // Otherwise, we just add tmprow to the matrix.
  unsigned newpivot = firstNonzeroEntry (tmprow);
  if(newpivot == -1)
  {
    for(int i = 0; i <= n; i++)
    {
      dep[i] = tmprow[n + i];
    }

    return true;
  }
  else
  {
    normalizeTmp (newpivot);

    for(int i = 0; i < 2 * n + 1; i++)
    {
      matrix[rows][i] = tmprow[i];
    }

    pivots[rows] = newpivot;
    rows++;

    return false;
  }
}

#if 0
std::ostream & operator<< (std::ostream & out,
                           const LinearDependencyMatrix & mat)
{
  int width = ((int) log10 (mat.p)) + 1;

  out << "Pivots: " << std::endl;
  for(int j = 0; j < mat.n; j++)
  {
    out << std::setw (width) << mat.pivots[j] << " ";
  }
  out << std::endl;
  out << "matrix:" << std::endl;
  for(int i = 0; i < mat.rows; i++)
  {
    for(int j = 0; j < mat.n; j++)
    {
      out << std::setw (width) << mat.matrix[i][j] << " ";
    }
    out << "| ";
    for(int j = mat.n; j <= 2 * mat.n; j++)
    {
      out << std::setw (width) << mat.matrix[i][j] << " ";
    }
    out << std::endl;
  }
  out << "tmprow: " << std::endl;
  for(int j = 0; j < mat.n; j++)
  {
    out << std::setw (width) << mat.tmprow[j] << " ";
  }
  out << "| ";
  for(int j = mat.n; j <= 2 * mat.n; j++)
  {
    out << std::setw (width) << mat.tmprow[j] << " ";
  }
  out << std::endl;

  return out;
}
#endif


NewVectorMatrix::NewVectorMatrix (unsigned n, unsigned long p)
{
  this->n = n;
  this->p = p;

  matrix = new unsigned long *[n];
  for(int i = 0; i < n; i++)
  {
    matrix[i] = new unsigned long[n];
  }

  pivots = new unsigned[n];
  rows = 0;
}

NewVectorMatrix::~NewVectorMatrix ()
{
  delete pivots;

  for(int i = 0; i < n; i++)
  {
    delete[]matrix[i];
  }
  delete matrix;
}

int NewVectorMatrix::firstNonzeroEntry (unsigned long *row)
{
  for(int i = 0; i < n; i++)
    if(row[i] != 0)
      return i;

  return -1;
}

void NewVectorMatrix::normalizeRow (unsigned long *row, unsigned i)
{
  unsigned long inv = modularInverse (row[i], p);
  row[i] = 1;

  for(int j = i + 1; j < n; j++)
  {
    row[j] = multMod (row[j], inv, p);
  }
}

void NewVectorMatrix::insertRow (unsigned long *row)
{
  for(int i = 0; i < rows; i++)
  {
    unsigned piv = pivots[i];
    unsigned x = row[piv];
    // if the corresponding entry in the row is zero, there is nothing to do
    if(x != 0)
    {
      // subtract row[i] times the i-th row
      for(int j = piv; j < n; j++)
      {
        unsigned long tmp = multMod (matrix[i][j], x, p);
        tmp = p - tmp;
        row[j] += tmp;
        row[j] %= p;
      }
    }
  }

  unsigned piv = firstNonzeroEntry (row);

  if(piv != -1)
  {
    // normalize and insert row into the matrix
    normalizeRow (row, piv);
    for(int i = 0; i < n; i++)
    {
      matrix[rows][i] = row[i];
    }

    pivots[rows] = piv;
    rows++;
  }
}


void NewVectorMatrix::insertMatrix (LinearDependencyMatrix & mat)
{
  // The matrix in LinearDependencyMatrix is already in reduced form.
  // Thus, if the current matrix is empty, we can simply copy the other matrix.
  if(rows == 0)
  {
    for(int i = 0; i < mat.rows; i++)
    {
      for(int j = 0; j < n; j++)
      {
        matrix[i][j] = mat.matrix[i][j];

        rows = mat.rows;
        for(int i = 0; i < rows; i++)
        {
          pivots[i] = mat.pivots[i];
        }
      }
    }
  }
  else
  {
    for(int i = 0; i < mat.rows; i++)
    {
      insertRow (mat.matrix[i]);
    }
  }
}

int NewVectorMatrix::findSmallestNonpivot ()
{
  // This method isn't very efficient, but it is called at most a few times, so efficiency is not important.
  if(rows == n)
    return -1;

  for(int i = 0; i < n; i++)
  {
    bool isPivot = false;
    for(int j = 0; j < rows; j++)
    {
      if(pivots[j] == i)
      {
        isPivot = true;
        break;
      }
    }

    if(!isPivot)
    {
      return i;
    }
  }
}


void vectorMatrixMult (unsigned long *vec, unsigned long **mat,
                       unsigned long *result, unsigned n, unsigned long p)
{
  unsigned long tmp;
  for(int i = 0; i < n; i++)
  {
    result[i] = 0;
    for(int j = 0; j < n; j++)
    {
      tmp = multMod (vec[j], mat[j][i], p);
      result[i] += tmp;
      result[i] %= p;
    }
  }
}


#if 0
void printVec (unsigned long *vec, int n)
{
  for(int i = 0; i < n; i++)
  {
    std::cout << vec[i] << ", ";
  }

  std::cout << std::endl;
}
#endif


unsigned long *computeMinimalPolynomial (unsigned long **matrix, unsigned n,
                                         unsigned long p)
{
  LinearDependencyMatrix lindepmat (n, p);
  NewVectorMatrix newvectormat (n, p);

  unsigned long *result = new unsigned long[n + 1];
  unsigned long *mpvec = new unsigned long[n + 1];
  unsigned long *tmp = new unsigned long[n + 1];

  // initialize result = 1
  for(int i = 0; i <= n; i++)
  {
    result[i] = 0;
  }
  result[0] = 1;

  int degresult = 0;


  int i = 0;

  unsigned long *vec = new unsigned long[n];
  unsigned long *vecnew = new unsigned long[n];

  while(i != -1)
  {
    for(int j = 0; j < n; j++)
    {
      vec[j] = 0;
    }
    vec[i] = 1;

    lindepmat.resetMatrix ();

    while(true)
    {
      bool ld = lindepmat.findLinearDependency (vec, mpvec);

      if(ld)
      {
        break;
      }

      vectorMatrixMult (vec, matrix, vecnew, n, p);
      unsigned long *swap = vec;
      vec = vecnew;
      vecnew = swap;
    }


    unsigned degmpvec = n;
    while(mpvec[degmpvec] == 0)
    {
      degmpvec--;
    }

    // if the dimension is already maximal, i.e., the minimal polynomial of vec has the highest possible degree,
    // then we are done.
    if(degmpvec == n)
    {
      unsigned long *swap = result;
      result = mpvec;
      mpvec = swap;
      i = -1;
    }
    else
    {
      // otherwise, we have to compute the lcm of mpvec and prior result

      // tmp = zeropol
      for(int j = 0; j <= n; j++)
      {
        tmp[j] = 0;
      }
      degresult = lcm (tmp, result, mpvec, p, degresult, degmpvec);
      unsigned long *swap = result;
      result = tmp;
      tmp = swap;

      if(degresult == n)
      {
        // minimal polynomial has highest possible degree, so we are done.
        i = -1;
      }
      else
      {
        newvectormat.insertMatrix (lindepmat);
        i = newvectormat.findSmallestNonpivot ();
      }
    }
  }

  // TODO: take lcms of the different monomials!

  delete[]vecnew;
  delete[]vec;
  delete[]tmp;
  delete[]mpvec;

  return result;
}


void rem (unsigned long *a, unsigned long *q, unsigned long p, int &dega,
          int degq)
{
  while(degq <= dega)
  {
    unsigned d = dega - degq;
    long factor = multMod (a[dega], modularInverse (q[degq], p), p);
    for(int i = degq; i >= 0; i--)
    {
      long tmp = p - multMod (factor, q[i], p);
      a[d + i] += tmp;
      a[d + i] %= p;
    }

    while(a[dega] == 0 && dega >= 0)
    {
      dega--;
    }
  }
}


void quo (unsigned long *a, unsigned long *q, unsigned long p, int &dega,
          int degq)
{
  unsigned degres = dega - degq;
  unsigned long *result = new unsigned long[degres + 1];

  while(degq <= dega)
  {
    unsigned d = dega - degq;
    long inv = modularInverse (q[degq], p);
    result[d] = multMod (a[dega], inv, p);
    for(int i = degq; i >= 0; i--)
    {
      long tmp = p - multMod (result[d], q[i], p);
      a[d + i] += tmp;
      a[d + i] %= p;
    }

    while(a[dega] == 0 && dega >= 0)
    {
      dega--;
    }
  }

  // copy result into a
  for(int i = 0; i <= degres; i++)
  {
    a[i] = result[i];
  }
  // set all following entries of a to zero
  for(int i = degres + 1; i <= degq + degres; i++)
  {
    a[i] = 0;
  }

  dega = degres;

  delete[]result;
}


void mult (unsigned long *result, unsigned long *a, unsigned long *b,
           unsigned long p, int dega, int degb)
{
  // NOTE: we assume that every entry in result is preinitialized to zero!

  for(int i = 0; i <= dega; i++)
  {
    for(int j = 0; j <= degb; j++)
    {
      result[i + j] += multMod (a[i], b[j], p);
      result[i + j] %= p;
    }
  }
}


int gcd (unsigned long *g, unsigned long *a, unsigned long *b,
         unsigned long p, int dega, int degb)
{
  unsigned long *tmp1 = new unsigned long[dega + 1];
  unsigned long *tmp2 = new unsigned long[degb + 1];
  for(int i = 0; i <= dega; i++)
  {
    tmp1[i] = a[i];
  }
  for(int i = 0; i <= degb; i++)
  {
    tmp2[i] = b[i];
  }
  int degtmp1 = dega;
  int degtmp2 = degb;

  unsigned long *swappol;
  int swapdeg;

  while(degtmp2 >= 0)
  {
    rem (tmp1, tmp2, p, degtmp1, degtmp2);
    swappol = tmp1;
    tmp1 = tmp2;
    tmp2 = swappol;

    swapdeg = degtmp1;
    degtmp1 = degtmp2;
    degtmp2 = swapdeg;
  }

  for(int i = 0; i <= degtmp1; i++)
  {
    g[i] = tmp1[i];
  }

  delete[]tmp1;
  delete[]tmp2;

  return degtmp1;
}


int lcm (unsigned long *l, unsigned long *a, unsigned long *b,
         unsigned long p, int dega, int degb)
{
  unsigned long *g = new unsigned long[dega + 1];
  // initialize entries of g to zero
  for(int i = 0; i <= dega; i++)
  {
    g[i] = 0;
  }

  int degg = gcd (g, a, b, p, dega, degb);

  if(degg > 0)
  {
    // non-trivial gcd, so compute a = (a/g)
    quo (a, g, p, dega, degg);
  }
  mult (l, a, b, p, dega, degb);

  // normalize
  if(l[dega + degb + 1] != 1)
  {
    unsigned long inv = modularInverse (l[dega + degb], p);
    for(int i = 0; i <= dega + degb; i++)
    {
      l[i] = multMod (l[i], inv, p);
    }
  }

  return dega + degb;
}


// computes the gcd and the cofactors of u and v
// gcd(u,v) = u3 = u*u1 + v*u2
unsigned long modularInverse (long long x, long long p)
{
  long long u1 = 1;
  long long u2 = 0;
  long long u3 = x;
  long long v1 = 0;
  long long v2 = 1;
  long long v3 = p;

  long long q, t1, t2, t3;
  while(v3 != 0)
  {
    q = u3 / v3;
    t1 = u1 - q * v1;
    t2 = u2 - q * v2;
    t3 = u3 - q * v3;
    u1 = v1;
    u2 = v2;
    u3 = v3;
    v1 = t1;
    v2 = t2;
    v3 = t3;
  }

  if(u1 < 0)
  {
    u1 += p;
  }

  return (unsigned long) u1;
}

