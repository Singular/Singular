#ifndef MATPOL_H
#define MATPOL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "polys/monomials/ring.h"

// THIS IS REALLY DIRTY: ip_smatrix HAS TO BE IDENTICAL TO ip_sideal
// SO, DON'T CHANGE THE DECLARATION OF ip_smatrix
class ip_smatrix
{
  public:

  poly *m;
  long rank;
  int nrows;
  int ncols;

  inline int& rows() { return nrows; }
  inline int& cols() { return ncols; }

  #define MATROWS(i) ((i)->nrows)
  #define MATCOLS(i) ((i)->ncols)
  /// 1-based access to matrix
  #define MATELEM(mat,i,j) ((mat)->m)[(long)MATCOLS((mat)) * (long)((i)-1) + (long)(j)-1]
  /// 0-based access to matrix
  #define MATELEM0(mat,i,j) ((mat)->m)[(long)MATCOLS((mat)) * (long)(i) + (long)(j)]
};

enum DetVariant
{
  DetDefault=0,
  DetBareiss,
  DetSBareiss,
  DetMu,
  DetFactory
};

typedef ip_smatrix *       matrix;

matrix mpNew(int r, int c);
#define mp_New(r,c,R) mpNew(r,c)

void   mp_Delete(matrix* a, const ring r);
matrix mp_Copy(const matrix a, const ring rSrc, const ring rDst);

matrix mp_Copy(matrix a, const ring r);

matrix mp_InitP(int r, int c, poly p, const ring R);
matrix mp_InitI(int r, int c, int v, const ring R);
matrix mp_MultI(matrix a, long f, const ring r);
matrix mp_MultP(matrix a, poly p, const ring r);
matrix pMultMp(poly p, matrix a, const ring r);
matrix mp_Add(matrix a, matrix b, const ring r);
matrix mp_Sub(matrix a, matrix b, const ring r);
matrix mp_Mult(matrix a, matrix b, const ring r);
matrix mp_Transp(matrix a, const ring r);
BOOLEAN mp_Equal(matrix a, matrix b, const ring r);
poly mp_Trace ( matrix a, const ring r);
poly TraceOfProd ( matrix a, matrix b, int n, const ring r);

matrix mp_Wedge(matrix a, int ar, const ring r);

// BOOLEAN mpJacobi(leftv res,leftv a);
// BOOLEAN mpKoszul(leftv res,leftv b/*in*/, leftv c/*ip*/, leftv id=NULL);

poly mp_Det(matrix a, const ring r, DetVariant d=DetDefault);
poly mp_DetBareiss (matrix a, const ring r);
poly mp_DetMu(matrix A, const ring R);


//matrix mp_Homogen(matrix a, int v, const ring r);

void   mp_Monomials(matrix c, int r, int var, matrix m, const ring R);

/// corresponds to Maple's coeffs:
/// var has to be the number of a variable
matrix mp_Coeffs(ideal I, int var, const ring r);

matrix mp_CoeffProc (poly f, poly vars, const ring r);
matrix mp_CoeffProcId (ideal I, poly vars, const ring R);
/// corresponds to Macauley's coef:
/// the exponent vector of vars has to contain the variables, eg 'xy';
/// then the poly f is searched for monomials in x and y, these monimials
/// are written to the first row of the matrix co.
/// the second row of co contains the respective factors in f.
/// Thus f = sum co[1,i]*co[2,i], i = 1..cols, rows equals 2.
void mp_Coef2(poly v, poly vars, matrix *c, matrix *m, const ring r);

/// for minors with Bareiss
void mp_RecMin(int, ideal, int &, matrix, int, int, poly, ideal, const ring);
void mp_MinorToResult(ideal, int &, matrix, int, int, ideal, const ring);

BOOLEAN mp_IsDiagUnit(matrix U, const ring r);

/// set spaces to zero by default
void iiWriteMatrix(matrix im, const char *n, int dim, const ring r, int spaces);

char * iiStringMatrix(matrix im, int dim, const ring r, char ch=',');

EXTERN_VAR omBin ip_smatrix_bin;

int mp_Compare(matrix a, matrix b, const ring r);

// -------------------------------------------------------
ideal sm_Add(ideal a, ideal b, const ring R);
ideal sm_Sub(ideal a, ideal b, const ring R);
ideal sm_Mult(ideal a, ideal b, const ring R);
ideal sm_Flatten(ideal a, const ring R);
ideal sm_UnFlatten(ideal a, int col, const ring R);
poly sm_Trace ( ideal a, const ring R);
int sm_Compare(ideal a, ideal b, const ring R);
BOOLEAN sm_Equal(ideal a, ideal b, const ring R);
ideal sm_Tensor(ideal A, ideal B, const ring r);
poly sm_Det(ideal I, const ring, DetVariant d=DetDefault);
DetVariant mp_GetAlgorithmDet(matrix m, const ring r);
DetVariant mp_GetAlgorithmDet(const char *s);

#define SMATELEM(A,i,j,R) p_Vec2Poly(A->m[j],i+1,R)

#endif/* MATPOL_H */
