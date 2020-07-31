/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facSparseHensel.cc
 *
 * This file implements functions for sparse heuristic Hensel lifting
 *
 * ABSTRACT: "A fast implementation of polynomial factorization" by M. Lucks and
 * "Effective polynomial computation" by R. Zippel
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"

#include "cf_assert.h"
#include "facSparseHensel.h"
#include "cf_algorithm.h"
#include "cfModGcd.h"
#include "facFqFactorize.h"

int
LucksWangSparseHeuristic (const CanonicalForm& F, const CFList& factors,
                          int level, const CFList& leadingCoeffs, CFList& result)
{
  int threshold= 450;
  CFArray termsF= getBiTerms (F, threshold);
  int si=termsF.size();
  int fl=factors.length();
  int ch=getCharacteristic();
  //printf("size:%d, length=%d, char=%d\n",si,fl,getCharacteristic());
  if ((si > threshold)
  || (si <= fl)
  )
    return 0;
  sort (termsF, level);

  CFArray* monoms= new CFArray [factors.length()];
  int i= 0;
  int num= 0;
  for (CFListIterator iter= factors; iter.hasItem(); iter++, i++)
  {
    monoms[i]= getTerms (iter.getItem());
    num += monoms [i].size();
    sort (monoms [i]);
  }

  i= 0;
  CFArray* monomsLead= new CFArray [leadingCoeffs.length()];
  for (CFListIterator iter= leadingCoeffs; iter.hasItem(); iter++, i++)
  {
    monomsLead[i]= getTerms (iter.getItem());
    sort (monomsLead [i]);
    groupTogether (monomsLead [i], level);
    strip (monomsLead [i], level);
  }

  CFArray solution= CFArray (num);
  int k, d, count, j= F.level() + 1;
  num= 0;
  i= 0;
  for (CFListIterator iter= factors; iter.hasItem(); i++, iter++)
  {
    d= degree (iter.getItem(), 1);
    count= 0;
    for (k= 0; k < monoms[i].size(); k++, j++, num++)
    {
      monoms [i][k] *= Variable (j);
      if (degree (monoms[i][k], 1) == d)
      {
        solution[num]= monomsLead [i] [count];
        count++;
      }
    }
  }

  delete [] monomsLead;

  CFList tmp;
  CFArray* stripped2= new CFArray [factors.length()];
  for (i= factors.length() - 1; i > -1; i--)
  {
    tmp.insert (buildPolyFromArray (monoms [i]));
    strip (monoms[i], stripped2 [i], level);
  }
  delete [] monoms;

  CanonicalForm H= prod (tmp);
  CFArray monomsH= getMonoms (H);
  sort (monomsH,F.level());

  groupTogether (monomsH, F.level());

  if (monomsH.size() != termsF.size())
  {
    delete [] stripped2;
    return 0;
  }

  CFArray strippedH;
  strip (monomsH, strippedH, level);
  CFArray strippedF;
  strip (termsF, strippedF, level);

  if (!isEqual (strippedH, strippedF))
  {
    delete [] stripped2;
    return 0;
  }

  CFArray A= getEquations (monomsH, termsF);
  CFArray startingSolution= solution;
  CFArray newSolution= CFArray (solution.size());
  result= CFList();
  do
  {
    evaluate (A, solution, F.level() + 1);
    if (isZero (A))
      break;
    if (!simplify (A, newSolution, F.level() + 1))
    {
      delete [] stripped2;
      return 0;
    }
    if (isZero (newSolution))
      break;
    if (!merge (solution, newSolution))
      break;
  } while (1);

  if (isEqual (startingSolution, solution))
  {
    delete [] stripped2;
    return 0;
  }
  CanonicalForm factor;
  num= 0;
  for (i= 0; i < factors.length(); i++)
  {
    k= stripped2[i].size();
    factor= 0;
    for (j= 0; j < k; j++, num++)
    {
      if (solution [num].isZero())
        continue;
      factor += solution [num]*stripped2[i][j];
    }
    result.append (factor);
  }

  delete [] stripped2;
  if (result.length() > 0)
    return 1;
  return 0;
}

CFList
sparseHeuristic (const CanonicalForm& A, const CFList& biFactors,
                 CFList*& moreBiFactors, const CFList& evaluation,
                 int minFactorsLength)
{
  int j= A.level() - 1;
  int i;

  //initialize storage
  CFArray *** storeFactors= new CFArray** [j];
  for (i= 0; i < j; i++)
    storeFactors [i]= new CFArray* [2];

  CFArray eval= CFArray (j);
  i= j - 1;
  for (CFListIterator iter= evaluation; iter.hasItem(); iter++,i--)
    eval[i]= iter.getItem();
  storeFactors [0] [0]= new CFArray [minFactorsLength];
  storeFactors [0] [1]= new CFArray [minFactorsLength];
  for (i= 1; i < j; i++)
  {
    storeFactors[i] [0]= new CFArray [minFactorsLength];
    storeFactors[i] [1]= new CFArray [minFactorsLength];
  }
  //

  CFList * normalizingFactors= new CFList [j];
  CFList uniFactors;
  normalizingFactors [0]= findNormalizingFactor1 (biFactors,
                                              evaluation.getLast(), uniFactors);
  for (i= j - 1; i > 0; i--)
  {
    if (moreBiFactors[i-1].length() != minFactorsLength)
    {
      moreBiFactors[i-1]=
        recombination (moreBiFactors [i-1], uniFactors, 1,
                       moreBiFactors[i-1].length()-uniFactors.length()+1,
                       eval[i], Variable (i + 2)
                      );
    }
    normalizingFactors [i]= findNormalizingFactor2 (moreBiFactors [i - 1],
                                                    eval[i], uniFactors);
  }

  CFList tmp;
  tmp= normalize (biFactors, normalizingFactors[0]);
  getTerms2 (tmp, storeFactors [0] [0]);
  storeFactors [0] [1]= evaluate (storeFactors [0] [0], minFactorsLength,
                                  evaluation.getLast(), Variable (2));
  for (i= j - 1; i > 0; i--)
  {
    tmp= normalize (moreBiFactors [i-1], normalizingFactors [i]);
    getTerms2 (tmp, storeFactors [i] [0]);
    storeFactors [i] [1]= evaluate (storeFactors [i] [0], minFactorsLength,
                                    eval[i], Variable (i + 2));
  }


  int k, l, m, mm, count, sizeOfUniFactors= 0;
  int*** seperator= new int** [j];
  Variable x= Variable (1);

  for (i= 0; i < j; i++)
    seperator [i]= new int* [minFactorsLength];
  for (k= 0; k < minFactorsLength; k++)
  {
    for (i= 0; i < j; i++)
    {
      count= 0;
      for (l= 0; l < storeFactors [i][0][k].size() - 1; l++)
      {
        if (degree (storeFactors[i][0][k][l], x) <
            degree (storeFactors[i][0][k][l+1], x))
          count++;
      }
      if (i == 0)
        sizeOfUniFactors= count;
      else if (sizeOfUniFactors != count)
      {
        for (m= 0; m < j; m++)
        {
          delete [] storeFactors [m] [0];
          delete [] storeFactors [m] [1];
          delete [] storeFactors [m];
          for (mm= 0; mm < k; mm++)
            delete [] seperator [m][mm];
          delete [] seperator [m];
        }
        delete [] storeFactors;
        delete [] seperator;
        delete [] normalizingFactors;
        return CFList();
      }
      seperator [i][k]= new int [count + 3];
      seperator [i][k][0]= count + 1;
      seperator [i][k][1]= 0;
      count= 2;
      for (l= 0; l < storeFactors [i][0][k].size() - 1; l++)
      {
        if (degree (storeFactors[i][0][k][l], x) <
            degree (storeFactors[i][0][k][l+1], x))
        {
          seperator[i][k][count]=l + 1;
          count++;
        }
      }
      seperator [i][k][count]= storeFactors[i][0][k].size();
    }
  }

  CanonicalForm tmp1, factor, quot;
  CanonicalForm B= A;
  CFList result;
  int maxTerms, n, index1, index2, mmm, found, columns, oneCount;
  int ** mat;

  for (k= 0; k < minFactorsLength; k++)
  {
    factor= 0;
    sizeOfUniFactors= seperator [0][k][0];
    for (n= 1; n <= sizeOfUniFactors; n++)
    {
      columns= 0;
      maxTerms= 1;
      index1= j - 1;
      for (i= j - 1; i >= 0; i--)
      {
        if (maxTerms < seperator[i][k][n+1]-seperator[i][k][n])
        {
          maxTerms= seperator[i][k][n + 1]-seperator[i][k][n];
          index1= i;
        }
      }
      for (i= j - 1; i >= 0; i--)
      {
        if (i == index1)
          continue;
        columns += seperator [i][k][n+1]-seperator[i][k][n];
      }
      mat= new int *[maxTerms];
      mm= 0;
      for (m= seperator[index1][k][n]; m < seperator[index1][k][n+1]; m++, mm++)
      {
        tmp1= storeFactors [index1][1][k][m];
        mat[mm]= new int [columns];
        for (i= 0; i < columns; i++)
          mat[mm][i]= 0;
        index2= 0;
        for (i= j - 1; i >= 0; i--)
        {
          if (i == index1)
            continue;
          found= -1;
          if ((found= search (storeFactors[i][1][k], tmp1,
                              seperator[i][k][n], seperator[i][k][n+1])) >= 0)
            mat[mm][index2 + found - seperator [i][k][n]]= 1;
          index2 += seperator [i][k][n+1]-seperator[i][k][n];
        }
      }

      index2= 0;
      for (i= j - 1; i >= 0; i--)
      {
        if (i == index1)
          continue;
        oneCount= 0;
        for (mm= 0; mm < seperator [i][k][n + 1] - seperator [i][k][n]; mm++)
        {
          for (m= 0; m < maxTerms; m++)
          {
            if (mat[m][mm+index2] == 1)
              oneCount++;
          }
        }
        if (oneCount == seperator [i][k][n+1]-seperator[i][k][n] - 1)
        {
          for (mm= 0; mm < seperator [i][k][n+1]-seperator[i][k][n]; mm++)
          {
            oneCount= 0;
            for (m= 0; m < maxTerms; m++)
              if (mat[m][mm+index2] == 1)
                oneCount++;
            if (oneCount > 0)
              continue;
            for (m= 0; m < maxTerms; m++)
            {
              oneCount= 0;
              for (mmm= 0; mmm < seperator[i][k][n+1]-seperator[i][k][n]; mmm++)
              {
                if (mat[m][mmm+index2] == 1)
                  oneCount++;
              }
              if (oneCount > 0)
                continue;
              mat[m][mm+index2]= 1;
            }
          }
        }
        index2 += seperator [i][k][n+1] - seperator [i][k][n];
      }

      //read off solution
      mm= 0;
      for (m= seperator[index1][k][n]; m < seperator[index1][k][n+1]; m++, mm++)
      {
        tmp1= storeFactors [index1][0][k][m];
        index2= 0;
        for (i= j - 1; i > -1; i--)
        {
          if (i == index1)
            continue;
          for (mmm= 0; mmm < seperator [i][k][n+1]-seperator[i][k][n]; mmm++)
            if (mat[mm][mmm+index2] == 1)
              tmp1= patch (tmp1, storeFactors[i][0][k][seperator[i][k][n]+mmm],
                           eval[i]);
          index2 += seperator [i][k][n+1]-seperator[i][k][n];
        }
        factor += tmp1;
      }

      for (m= 0; m < maxTerms; m++)
        delete [] mat [m];
      delete [] mat;
    }

    if (fdivides (factor, B, quot))
    {
      result.append (factor);
      B= quot;
      if (result.length() == biFactors.length() - 1)
      {
        result.append (quot);
        break;
      }
    }
  }

  //delete
  for (i= 0; i < j; i++)
  {
    delete [] storeFactors [i] [0];
    delete [] storeFactors [i] [1];
    delete [] storeFactors [i];
    for (k= 0; k < minFactorsLength; k++)
      delete [] seperator [i][k];
    delete [] seperator [i];
  }
  delete [] seperator;
  delete [] storeFactors;
  delete [] normalizingFactors;
  //

  return result;
}
