/*************************************************
 * Author: Sebastian Jambor, 2011                *
 * GPL (e-mail from June 6, 2012, 17:00:31 MESZ) *
 * sebastian@momo.math.rwth-aachen.de            *
 ************************************************/


#include <cmath>
#include <cstdlib>



#include "kernel/mod2.h"

#include "minpoly.h"

// TODO: avoid code copying, use subclassing instead!

LinearDependencyMatrix::LinearDependencyMatrix (unsigned n, unsigned long p)
{
  this->n = n;
  this->p = p;

  matrix = new unsigned long *[n];
  for(unsigned long i = 0; i < n; i++)
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

  for(unsigned long i = 0; i < n; i++)
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
  for(unsigned long i = 0; i < n; i++)
    if(row[i] != 0)
      return i;

  return -1;
}

void LinearDependencyMatrix::reduceTmpRow ()
{
  for(unsigned long i = 0; i < rows; i++)
  {
    unsigned piv = pivots[i];
    unsigned x = tmprow[piv];
    // if the corresponding entry in the row is zero, there is nothing to do
    if(x != 0)
    {
      // subtract tmprow[i] times the i-th row
      for(unsigned j = piv; j < n + rows + 1; j++)
      {
        if (matrix[i][j] != 0)
        {
          unsigned long tmp = multMod (matrix[i][j], x, p);
          tmp = p - tmp;
          tmprow[j] += tmp;
          if (tmprow[j] >= p)
          {
            tmprow[j] -= p;
          }
        }
      }
    }
  }
}


void LinearDependencyMatrix::normalizeTmp (unsigned i)
{
  unsigned long inv = modularInverse (tmprow[i], p);
  tmprow[i] = 1;
  for(unsigned long j = i + 1; j < 2 * n + 1; j++)
    tmprow[j] = multMod (tmprow[j], inv, p);
}

bool LinearDependencyMatrix::findLinearDependency (unsigned long *newRow,
                                                   unsigned long *dep)
{
  // Copy newRow to tmprow (we need to add RHS)
  for(unsigned long i = 0; i < n; i++)
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
    for(unsigned long i = 0; i <= n; i++)
    {
      dep[i] = tmprow[n + i];
    }

    return true;
  }
  else
  {
    normalizeTmp (newpivot);

    for(unsigned long i = 0; i < 2 * n + 1; i++)
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
  for(unsigned long i = 0; i < n; i++)
  {
    matrix[i] = new unsigned long[n];
  }

  pivots = new unsigned[n];

  nonPivots = new unsigned[n];

  for (unsigned long i = 0; i < n; i++)
  {
     nonPivots[i] = i;
  }

  rows = 0;
}

NewVectorMatrix::~NewVectorMatrix ()
{
  delete nonPivots;
  delete pivots;

  for(unsigned long i = 0; i < n; i++)
  {
    delete[]matrix[i];
  }
  delete matrix;
}

int NewVectorMatrix::firstNonzeroEntry (unsigned long *row)
{
  for(unsigned long i = 0; i < n; i++)
    if(row[i] != 0)
      return i;

  return -1;
}

void NewVectorMatrix::normalizeRow (unsigned long *row, unsigned i)
{
  unsigned long inv = modularInverse (row[i], p);
  row[i] = 1;

  for(unsigned long j = i + 1; j < n; j++)
  {
    row[j] = multMod (row[j], inv, p);
  }
}

void NewVectorMatrix::insertRow (unsigned long *row)
{
  for(unsigned long i = 0; i < rows; i++)
  {
    unsigned piv = pivots[i];
    unsigned x = row[piv];
    // if the corresponding entry in the row is zero, there is nothing to do
    if(x != 0)
    {
      // subtract row[i] times the i-th row
      // only the non-pivot entries have to be considered
      // (and also the first entry)
      row[piv] = 0;

      int smallestNonPivIndex = 0;
      while (nonPivots[smallestNonPivIndex] < piv)
      {
        smallestNonPivIndex++;
      }

      for (unsigned long j = smallestNonPivIndex; j < n-rows; j++)
      {
        unsigned ind = nonPivots[j];
        if (matrix[i][ind] != 0)
        {
          unsigned long tmp = multMod (matrix[i][ind], x, p);
          tmp = p - tmp;
          row[ind] += tmp;
          if (row[ind] >= p)
          {
            row[ind] -= p;
          }
        }
      }
    }
  }

  unsigned piv = firstNonzeroEntry (row);

  if(piv != -1)
  {
    // Normalize and insert row into the matrix.
    // Then reduce upwards.
    normalizeRow (row, piv);
    for(unsigned long i = 0; i < n; i++)
    {
      matrix[rows][i] = row[i];
    }


    for (unsigned long i = 0; i < rows; i++)
    {
      unsigned x = matrix[i][piv];
      // if the corresponding entry in the matrix is zero,
      // there is nothing to do
      if (x != 0)
      {
        for (unsigned long j = piv; j < n; j++)
        {
          if (row[j] != 0)
          {
            unsigned long tmp = multMod(row[j], x, p);
            tmp = p - tmp;
            matrix[i][j] += tmp;
            if (matrix[i][j] >= p)
            {
              matrix[i][j] -= p;
            }
          }
        }
      }
    }

    pivots[rows] = piv;

    // update nonPivots
    for (unsigned long i = 0; i < n-rows; i++)
    {
      if (nonPivots[i] == piv)
      {
        // shift everything one position to the left
        for (unsigned long j = i; j < n-rows-1; j++)
        {
          nonPivots[j] = nonPivots[j+1];
        }

        break;
      }
    }

    rows++;
  }
}


void NewVectorMatrix::insertMatrix (LinearDependencyMatrix & mat)
{
  for(unsigned i = 0; i < mat.rows; i++)
  {
    insertRow (mat.matrix[i]);
  }
}

int NewVectorMatrix::findSmallestNonpivot ()
{
  // This method isn't very efficient, but it is called at most a few times,
  // so efficiency is not important.
  if(rows == n)
    return -1;

  for(unsigned long i = 0; i < n; i++)
  {
    bool isPivot = false;
    for(unsigned j = 0; j < rows; j++)
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
  abort();
}

int NewVectorMatrix::findLargestNonpivot ()
{
  // This method isn't very efficient, but it is called at most a few times, so efficiency is not important.
  if(rows == n)
    return -1;

  for(unsigned long i = n-1; i >= 0; i--)
  {
    bool isPivot = false;
    for(unsigned j = 0; j < rows; j++)
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
  abort();
}


void vectorMatrixMult (unsigned long *vec, unsigned long **mat,
                       unsigned **nonzeroIndices, unsigned *nonzeroCounts,
                       unsigned long *result, unsigned n, unsigned long p)
{
  unsigned long tmp;

  for(unsigned long i = 0; i < n; i++)
  {
    result[i] = 0;
    for(unsigned j = 0; j < nonzeroCounts[i]; j++)
    {
      tmp = multMod (vec[nonzeroIndices[i][j]], mat[nonzeroIndices[i][j]][i], p);
      result[i] += tmp;
      if (result[i] >= p)
      {
        result[i] -= p;
      }
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
  for(unsigned long i = 0; i <= n; i++)
  {
    result[i] = 0;
  }
  result[0] = 1;

  int degresult = 0;


  // Store the indices where the matrix has non-zero entries.
  // This has a huge impact on spares matrices.
  unsigned* nonzeroCounts = new unsigned[n];
  unsigned** nonzeroIndices = new unsigned*[n];
  for (unsigned long i = 0; i < n; i++)
  {
    nonzeroIndices[i] = new unsigned[n];
    nonzeroCounts[i] = 0;
    for (unsigned long j = 0; j < n; j++)
    {
      if (matrix[j][i] != 0)
      {
        nonzeroIndices[i][nonzeroCounts[i]] = j;
        nonzeroCounts[i]++;
      }
    }
  }

  int i = n-1;

  unsigned long *vec = new unsigned long[n];
  unsigned long *vecnew = new unsigned long[n];

  unsigned loopsEven = true;
  while(i != -1)
  {
    for(unsigned long j = 0; j < n; j++)
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

      vectorMatrixMult (vec, matrix, nonzeroIndices, nonzeroCounts, vecnew, n, p);
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
      for(unsigned long j = 0; j <= n; j++)
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

        // choose new unit vector from the front or the end, alternating
        // for each round. If the matrix is the companion matrix for x^n,
        // then taking vectors from the end is best. If it is the transpose,
        // taking vectors from the front is best.
        // This tries to take the middle way
        if (loopsEven)
        {
          i = newvectormat.findSmallestNonpivot ();
        }
        else
        {
          i = newvectormat.findLargestNonpivot ();
        }
      }
    }

    loopsEven = !loopsEven;
  }

  for (unsigned long i = 0; i < n; i++)
  {
    delete[] nonzeroIndices[i];
  }
  delete[] nonzeroIndices;
  delete[] nonzeroCounts;


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
      if (a[d + i] >= p)
      {
        a[d + i] -= p;
      }
    }

    while(dega >= 0 && a[dega] == 0)
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

  // initialize to zero
  for (unsigned i = 0; i <= degres; i++)
  {
    result[i] = 0;
  }

  while(degq <= dega)
  {
    unsigned d = dega - degq;
    unsigned long inv = modularInverse (q[degq], p);
    result[d] = multMod (a[dega], inv, p);
    for(int i = degq; i >= 0; i--)
    {
      unsigned long tmp = p - multMod (result[d], q[i], p);
      a[d + i] += tmp;
      if (a[d + i] >= p)
      {
        a[d + i] -= p;
      }
    }

    while(dega >= 0 && a[dega] == 0)
    {
      dega--;
    }
  }

  // copy result into a
  for(unsigned i = 0; i <= degres; i++)
  {
    a[i] = result[i];
  }
  // set all following entries of a to zero
  for(unsigned i = degres + 1; i <= degq + degres; i++)
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
      if (result[i + j] >= p)
      {
        result[i + j] -= p;
      }
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

