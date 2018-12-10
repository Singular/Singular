


#include "kernel/mod2.h"
#include "Singular/lists.h"
#include "kernel/linear_algebra/linearAlgebra.h"

/*!
!
*
 * Computes all eigenvalues of a given real quadratic matrix with
 * multiplicites.
 *
 * The method assumes that the current ground field is the complex numbers.
 * Computations are based on the QR double shift algorithm involving
 * Hessenberg form and householder transformations.
 * If the algorithm works, then it returns a list with two entries which
 * are again lists of the same size:
 * _[1][i] is the i-th mutually distinct eigenvalue that was found,
 * _[2][i] is the (int) multiplicity of _[1][i].
 * If the algorithm does not work (due to an ill-posed matrix), a list with
 * the single entry (int)0 is returned.
 * 'tol1' is used for detection of deflation in the actual qr double shift
 * algorithm.
 * 'tol2' is used for ending Heron's iteration whenever square roots
 * are being computed.
 * 'tol3' is used to distinguish between distinct eigenvalues: When
 * the Euclidean distance between two computed eigenvalues is less then
 * tol3, then they will be regarded equal, resulting in a higher
 * multiplicity of the corresponding eigenvalue.
 *
 * @return a list with one entry (int)0, or two entries which are again lists
 **/
lists qrDoubleShift(
       const matrix A,     /*!
!
*< [in]  the quadratic matrix         

*/
       const number tol1,  /*!
!
*< [in]  tolerance for deflation      

*/
       const number tol2,  /*!
!
*< [in]  tolerance for square roots   

*/
       const number tol3,   /*!
!
*< [in]  tolerance for distinguishing
                                      eigenvalues                  

*/
       const ring r= currRing
                   );

lists qrDoubleShift(const matrix A, const number tol1, const number tol2,
                    const number tol3, const ring R)
{
  int n = MATROWS(A);
  matrix* queue = new matrix[n];
  queue[0] = mp_Copy(A,R); int queueL = 1;
  number* eigenVs = new number[n]; int eigenL = 0;
  /*!
!
 here comes the main call: 

*/
  bool worked = qrDS(n, queue, queueL, eigenVs, eigenL, tol1, tol2,R);
  lists result = (lists)omAlloc(sizeof(slists));
  if (!worked)
  {
    for (int i = 0; i < eigenL; i++)
      nDelete(&eigenVs[i]);
    delete [] eigenVs;
    for (int i = 0; i < queueL; i++)
      idDelete((ideal*)&queue[i]);
    delete [] queue;
    result->Init(1);
    result->m[0].rtyp = INT_CMD;
    result->m[0].data = (void*)0;   /*!
!
 a list with a single entry
                                             which is the int zero 

*/
  }
  else
  {
    /*!
!
 now eigenVs[0..eigenL-1] contain all eigenvalues; among them, there
       may be equal entries 

*/
    number* distinctEVs = new number[n]; int distinctC = 0;
    int* mults = new int[n];
    for (int i = 0; i < eigenL; i++)
    {
      int index = similar(distinctEVs, distinctC, eigenVs[i], tol3);
      if (index == -1) /*!
!
 a new eigenvalue 

*/
      {
        distinctEVs[distinctC] = nCopy(eigenVs[i]);
        mults[distinctC++] = 1;
      }
      else mults[index]++;
      nDelete(&eigenVs[i]);
    }
    delete [] eigenVs;

    lists eigenvalues = (lists)omAlloc(sizeof(slists));
    eigenvalues->Init(distinctC);
    lists multiplicities = (lists)omAlloc(sizeof(slists));
    multiplicities->Init(distinctC);
    for (int i = 0; i < distinctC; i++)
    {
      eigenvalues->m[i].rtyp = NUMBER_CMD;
      eigenvalues->m[i].data = (void*)nCopy(distinctEVs[i]);
      multiplicities->m[i].rtyp = INT_CMD;
      multiplicities->m[i].data = (void*)(long)mults[i];
      nDelete(&distinctEVs[i]);
    }
    delete [] distinctEVs; delete [] mults;

    result->Init(2);
    result->m[0].rtyp = LIST_CMD;
    result->m[0].data = (char*)eigenvalues;
    result->m[1].rtyp = LIST_CMD;
    result->m[1].data = (char*)multiplicities;
  }
  return result;
}

