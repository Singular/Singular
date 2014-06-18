/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facSparseHensel.h
 *
 * This file provides functions for sparse heuristic Hensel lifting
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_SPARSE_HENSEL_H
#define FAC_SPARSE_HENSEL_H

#include "canonicalform.h"
#include "cf_map_ext.h"
#include "cf_iter.h"
#include "templates/ftmpl_functions.h"
#include "cf_algorithm.h"
#include "cf_map.h"

/// compare polynomials
inline
int comp (const CanonicalForm& A, const CanonicalForm& B)
{
  if (A.inCoeffDomain() && !B.inCoeffDomain())
    return -1;
  else if (!A.inCoeffDomain() && B.inCoeffDomain())
    return 1;
  else if (A.inCoeffDomain() && B.inCoeffDomain())
    return 0;
  else if (degree (A, 1) > degree (B, 1))
    return 1;
  else if (degree (A, 1) < degree (B, 1))
    return -1;
  // here A and B are not in CoeffDomain
  int n= tmax (A.level(), B.level());
  for (int i= 2; i <= n; i++)
  {
    if (degree (A,i) > degree (B,i))
      return 1;
    else if (degree (A,i) < degree (B,i))
      return -1;
  }
  return 0;
}

/// compare two polynomials up to level @a level
inline
int comp (const CanonicalForm& A, const CanonicalForm& B, int level)
{
  if (A.inCoeffDomain() && !B.inCoeffDomain() && B.level() <= level)
    return -1;
  else if (!A.inCoeffDomain() && A.level() <= level && B.inCoeffDomain())
    return 1;
  else if (A.inCoeffDomain() && B.inCoeffDomain())
    return 0;
  else if (degree (A, 1) > degree (B, 1))
    return 1;
  else if (degree (A, 1) < degree (B, 1))
    return -1;
  // here A and B are not in coeffdomain
  for (int i= 2; i <= level; i++)
  {
    if (degree (A,i) > degree (B,i))
      return 1;
    else if (degree (A,i) < degree (B,i))
      return -1;
  }
  return 0;
}

/// swap entry @a i and @a j in @a A
inline
void swap (CFArray& A, int i, int j)
{
  CanonicalForm tmp= A[i];
  A[i]= A[j];
  A[j]= tmp;
}

/// quick sort helper function
inline
void quickSort (int lo, int hi, CFArray& A, int l)
{
  int i= lo, j= hi;
  CanonicalForm tmp= A[(lo+hi)/2];
  while (i <= j)
  {
    if (l > 0)
    {
      while (comp (A [i], tmp, l) < 0 && i < hi) i++;
      while (comp (tmp, A[j], l) < 0 && j > lo) j--;
    }
    else
    {
      while (comp (A [i], tmp) < 0 && i < hi) i++;
      while (comp (tmp, A[j]) < 0 && j > lo) j--;
    }
    if (i <= j)
    {
      swap (A, i, j);
      i++;
      j--;
    }
  }
  if (lo < j) quickSort (lo, j, A, l);
  if (i < hi) quickSort (i, hi, A, l);
}

/// quick sort @a A
inline
void sort (CFArray& A, int l= 0)
{
  quickSort (0, A.size() - 1, A, l);
}


/// find normalizing factors for @a biFactors and build monic univariate factors
/// from @a biFactors
inline CFList
findNormalizingFactor1 (const CFList& biFactors, const CanonicalForm& evalPoint,
                        CFList& uniFactors)
{
  CFList result;
  CanonicalForm tmp;
  for (CFListIterator i= biFactors; i.hasItem(); i++)
  {
    tmp= i.getItem() (evalPoint);
    uniFactors.append (tmp /Lc (tmp));
    result.append (Lc (tmp));
  }
  return result;
}

/// find normalizing factors for @a biFactors and sort @a biFactors s.t.
/// the returned @a biFactors evaluated at evalPoint coincide with @a uniFactors
inline CFList
findNormalizingFactor2 (CFList& biFactors, const CanonicalForm& evalPoint,
                        const CFList& uniFactors)
{
  CFList result;
  CFList uniBiFactors= biFactors;
  CFList newBiFactors;
  CFList l;
  int pos;
  CFListIterator iter;
  for (iter= uniBiFactors; iter.hasItem(); iter++)
  {
    iter.getItem()= iter.getItem() (evalPoint);
    l.append (Lc (iter.getItem()));
    iter.getItem() /= Lc (iter.getItem());
  }
  for (CFListIterator i= uniFactors; i.hasItem(); i++)
  {
    pos= findItem (uniBiFactors, i.getItem());
    newBiFactors.append (getItem (biFactors, pos));
    result.append (getItem (l, pos));
  }
  biFactors= newBiFactors;
  return result;
}

/// get terms of @a F
inline CFArray
getTerms (const CanonicalForm& F)
{
  if (F.inCoeffDomain())
  {
    CFArray result= CFArray (1);
    result [0]= F;
    return result;
  }
  if (F.isUnivariate())
  {
    CFArray result= CFArray (size(F));
    int j= 0;
    for (CFIterator i= F; i.hasTerms(); i++, j++)
      result[j]= i.coeff()*power (F.mvar(), i.exp());
    return result;
  }
  int numMon= size (F);
  CFArray result= CFArray (numMon);
  int j= 0;
  CFArray recResult;
  Variable x= F.mvar();
  CanonicalForm powX;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    powX= power (x, i.exp());
    recResult= getTerms (i.coeff());
    for (int k= 0; k < recResult.size(); k++)
      result[j+k]= powX*recResult[k];
    j += recResult.size();
  }
  return result;
}

/// helper function for getBiTerms
inline CFArray
getBiTerms_helper (const CanonicalForm& F, const CFMap& M, int threshold)
{
  CFArray buf= CFArray (size (F));
  int k= 0, level= F.level() - 1;
  Variable x= F.mvar();
  Variable y= Variable (F.level() - 1);
  Variable one= Variable (1);
  Variable two= Variable (2);
  CFIterator j;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().level() < level)
    {
      buf[k]= M (i.coeff())*power (one,i.exp());
      k++;
      if (k > threshold)
        break;
      continue;
    }
    j= i.coeff();
    for (;j.hasTerms() && k <= threshold; j++, k++)
      buf[k]= power (one,i.exp())*power (two,j.exp())*M (j.coeff());
    if (k > threshold)
      break;
  }
  CFArray result= CFArray (k);
  for (int i= 0; i < k && k <= threshold; i++)
    result[i]= buf[i];
  return result;
}

/// get terms of @a F where F is considered a bivariate poly in Variable(1),
/// Variable (2)
inline CFArray
getBiTerms (const CanonicalForm& F, int threshold)
{
  if (F.inCoeffDomain())
  {
    CFArray result= CFArray (1);
    result [0]= F;
    return result;
  }
  if (F.isUnivariate())
  {
    CFArray result= CFArray (size(F));
    int j= 0;
    for (CFIterator i= F; i.hasTerms(); i++, j++)
      result[j]= i.coeff()*power (F.mvar(), i.exp());
    return result;
  }

  CanonicalForm G= F;

  CFMap M;
  M.newpair (Variable (1), F.mvar());
  M.newpair (Variable (2), Variable (F.level() - 1));
  G= swapvar (F, Variable (1), F.mvar());
  G= swapvar (G, Variable (2), Variable (F.level() - 1));

  CFArray result= getBiTerms_helper (G, M, threshold);
  return result;
}

/// build a poly from entries in @a A
inline CanonicalForm
buildPolyFromArray (const CFArray& A)
{
  CanonicalForm result= 0;
  for (int i= A.size() - 1; i > -1; i--)
    result += A[i];
  return result;
}

/// group together elements in @a A, where entries in @a A are put together
/// if they coincide up to level @a level
inline void
groupTogether (CFArray& A, int level)
{
  int n= A.size() - 1;
  int k= A.size();
  for (int i= 0; i < n; i++)
  {
    if (comp (A[i],A[i+1], level) == 0)
    {
      A[i+1] += A[i];
      A[i]= 0;
      k--;
    }
  }
  if (A[n].isZero())
    k--;
  CFArray B= CFArray (k);
  n++;
  k= 0;
  for (int i= 0; i < n; i++)
  {
    if (!A[i].isZero())
    {
      B[k]= A[i];
      k++;
    }
  }
  A= B;
}

/// strip off those parts of entries in @a F whose level is less than or equal
/// than @a level and store the stripped off parts in @a G
inline void
strip (CFArray& F, CFArray& G, int level)
{
  int n, m, i, j;
  CanonicalForm g;
  m= F.size();
  G= CFArray (m);
  for (j= 0; j < m; j++)
  {
    g= 1;
    for (i= 1; i <= level; i++)
    {
      if ((n= degree (F[j],i)) > 0)
        g *= power (Variable (i), n);
    }
    F[j] /= g;
    G[j]= g;
  }
}

/// s.a. stripped off parts are not returned
inline void
strip (CFArray& F, int level)
{
  int n, m, i;
  CanonicalForm g;
  m= F.size();
  for (int j= 0; j < m; j++)
  {
    g= 1;
    for (i= 1; i <= level; i++)
    {
      if ((n= degree (F[j],i)) > 0)
        g *= power (Variable (i), n);
    }
    F[j] /= g;
  }
}

/// get equations for LucksWangSparseHeuristic
inline
CFArray getEquations (const CFArray& A, const CFArray& B)
{
  ASSERT (A.size() == B.size(), "size of A and B has to coincide");
  CFArray result= CFArray (A.size());
  int n= A.size();
  for (int i= 0; i < n; i++)
    result[i]= A[i]-B[i];
  return result;
}

/// evaluate every entry of @a A at @a B and level @a level
inline void
evaluate (CFArray& A, const CanonicalForm& B, int level)
{
  int n= A.size();
  for (int i= 0; i < n; i++)
  {
    if (A[i].level() < level)
      continue;
    else
      A[i]= A[i] (B, level);
  }
}

/// evaluate every entry of @a A at every entry of @a B starting at level @a
/// level
inline void
evaluate (CFArray& A, const CFArray& B, int level)
{
  int n= B.size();
  for (int i= 0; i < n; i++)
  {
    if (!B[i].isZero())
      evaluate (A, B[i], level + i);
  }
}

/// simplify @a A if possible, i.e. @a A consists of 2 terms and contains only
/// one variable of level greater or equal than @a level
inline CanonicalForm
simplify (const CanonicalForm& A, int level)
{
  CanonicalForm F= 0;
  if (size (A, A.level()) == 2)
  {
    CanonicalForm C= getVars (A);
    if ((C/C.mvar()).level() < level)
    {
      CanonicalForm B= LC (A);
      if (B.level() < level)
      {
        CanonicalForm quot;
        if (fdivides (B, A, quot))
          F= -tailcoeff (quot);
      }
    }
  }
  return F;
}

///  if possible simplify @a A as described above and store result in @a B
inline bool
simplify (CFArray& A, CFArray& B, int level)
{
  int n= A.size();
  CanonicalForm f;
  int index;
  for (int i= 0; i < n; i++)
  {
    if (!A[i].isZero())
    {
      f= simplify (A[i], level);
      if (!f.isZero())
      {
        index= A[i].level() - level;
        if (index < 0 || index >= B.size())
          return false;
        if (!B[index].isZero() && B[index] != f)
          return false;
        else if (B[index].isZero())
        {
          B[index]= f;
          A[i]= 0;
        }
      }
    }
  }
  return true;
}

/// merge @a B into @a A if possible, i.e. every non-zero entry in @a A should
/// be zero in @a B
inline bool
merge (CFArray& A, CFArray& B)
{
  if (A.size() != B.size())
    return false;
  int n= A.size();
  for (int i= 0; i < n; i++)
  {
    if (!B[i].isZero())
    {
      if (A[i].isZero())
      {
        A[i]= B[i];
        B[i]= 0;
      }
      else if (A[i] == B[i])
        B[i]= 0;
      else
        return false;
    }
  }
  return true;
}

/// checks if entries of @a A are zero
inline bool
isZero (const CFArray& A)
{
  int n= A.size();
  for (int i= 0; i < n; i++)
    if (!A[i].isZero())
      return false;
  return true;
}

/// checks if @a A equals @a B
inline bool
isEqual (const CFArray& A, const CFArray& B)
{
  if (A.size() != B.size())
    return false;
  int i, n= B.size();
  for (i= 0; i < n; i++)
    if (A[i] != B[i])
      return false;
  return true;
}

/// get terms of @a F wrt. Variable (1)
inline CFArray
getTerms2 (const CanonicalForm& F)
{
  if (F.inCoeffDomain())
  {
    CFArray result= CFArray (1);
    result[0]= F;
    return result;
  }
  CFArray result= CFArray (size (F));
  int j= 0;
  Variable x= F.mvar();
  Variable y= Variable (1);
  CFIterator k;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      result[j]= i.coeff()*power (x,i.exp());
      j++;
    }
    else
    {
      for (k= i.coeff(); k.hasTerms(); k++, j++)
        result[j]= k.coeff()*power (x,i.exp())*power (y,k.exp());
    }
  }
  sort (result);
  return result;
}

/// get terms of entries in @a F and put them in @a result
inline
void getTerms2 (const CFList& F, CFArray* result)
{
  int j= 0;
  for (CFListIterator i= F; i.hasItem(); i++, j++)
    result[j]= getTerms2 (i.getItem());
}

/// evaluate entries in @a A at @a eval and @a y
inline CFArray
evaluate (const CFArray& A, const CanonicalForm& eval, const Variable& y)
{
  int j= A.size();
  CFArray result= CFArray (j);
  for (int i= 0; i < j; i++)
    result [i]= A[i] (eval, y);
  return result;
}

/// s.a.
inline CFArray*
evaluate (CFArray* const& A, int sizeA, const CanonicalForm& eval,
          const Variable& y)
{
  CFArray* result= new CFArray [sizeA];
  for (int i= 0; i < sizeA; i++)
    result[i]= evaluate (A[i], eval, y);
  return result;
}

/// normalize entries in @a L with @a normalizingFactor
inline
CFList normalize (const CFList& L, const CFList& normalizingFactor)
{
  CFList result;
  CFListIterator j= normalizingFactor;
  for (CFListIterator i= L; i.hasItem(); i++, j++)
    result.append (i.getItem() / j.getItem());
  return result;
}

/// search for @a F in @a A between index @a i and @a j
inline
int search (const CFArray& A, const CanonicalForm& F, int i, int j)
{
  for (; i < j; i++)
    if (A[i] == F)
      return i;
  return -1;
}

/// patch together @a F1 and @a F2 and normalize by a power of @a eval
/// @a F1 and @a F2 are assumed to be bivariate with one variable having level 1
inline
CanonicalForm patch (const CanonicalForm& F1, const CanonicalForm& F2,
                     const CanonicalForm& eval)
{
  CanonicalForm result= F1;
  if (F2.level() != 1 && !F2.inCoeffDomain())
  {
    int d= degree (F2);
    result *= power (F2.mvar(), d);
    result /= power (eval, d);
  }
  return result;
}

/// sparse heuristic lifting by Wang and Lucks
///
/// @return @a LucksWangSparseHeuristic returns true if it was successful
int
LucksWangSparseHeuristic (const CanonicalForm& F,     ///<[in] polynomial to be
                                                      ///< factored
                          const CFList& factors,      ///<[in] factors of F
                                                      ///< lifted to level
                          int level,                  ///<[in] level of lifted
                                                      ///< factors
                          const CFList& leadingCoeffs,///<[in] leading
                                                      ///< coefficients of
                                                      ///< factors
                          CFList& result              ///<[in,out] result
                         );

/// sparse heuristic which patches together bivariate factors of @a A wrt.
/// different second variables by their univariate images
///
/// @return @a sparseHeuristic returns a list of found factors of A
CFList
sparseHeuristic (const CanonicalForm& A,  ///<[in] polynomial to be factored
                 const CFList& biFactors, ///<[in] bivariate factors of A where
                                          ///< the second variable has level 2
                 CFList*& moreBiFactors,  ///<[in] more bivariate factorizations
                                          ///< wrt. different second variables
                 const CFList& evaluation,///<[in] evaluation point
                 int minFactorsLength     ///<[in] minimal length of bivariate
                                          ///< factorizations
                );

#endif
