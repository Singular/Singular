#ifndef MATPOL_H
#define MATPOL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

// #include <kernel/structs.h>
#include <polys/monomials/ring.h>

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
  #define MATELEM(mat,i,j) ((mat)->m)[MATCOLS((mat)) * ((i)-1) + (j)-1]
};

typedef ip_smatrix *       matrix;

matrix mpNew(int r, int c);
static inline matrix mp_New(int r, int c){ return mpNew(r,c); }

// matrix mpCopy(matrix a);
void   mp_Delete(matrix* a, const ring r);
matrix mp_Copy(const matrix a, const ring rSrc, const ring rDst);

matrix mp_Copy(matrix a, const ring r);
// static inline matrix mp_Copy(matrix a, const ring r){ return mp_Copy(a, r, r); }

matrix mp_InitP(int r, int c, poly p, const ring R);
matrix mp_InitI(int r, int c, int v, const ring R);
matrix mp_MultI(matrix a, int f, const ring r);
matrix mp_MultP(matrix a, poly p, const ring r);
matrix pMultMp(poly p, matrix a, const ring r);
matrix mp_Add(matrix a, matrix b, const ring r);
matrix mp_Sub(matrix a, matrix b, const ring r);
matrix mp_Mult(matrix a, matrix b, const ring r);
matrix mp_Transp(matrix a, const ring r);
BOOLEAN mp_Equal(matrix a, matrix b, const ring r);
poly mp_Trace ( matrix a, const ring r);
poly TraceOfProd ( matrix a, matrix b, int n, const ring r);

poly mp_Det (matrix m, const ring r);
matrix mp_Wedge(matrix a, int ar, const ring r);

// BOOLEAN mpJacobi(leftv res,leftv a);
// BOOLEAN mpKoszul(leftv res,leftv b/*in*/, leftv c/*ip*/, leftv id=NULL);
poly mp_DetBareiss (matrix a, const ring r);

//matrix mp_Homogen(matrix a, int v, const ring r);

void   mp_Monomials(matrix c, int r, int var, matrix m, const ring R);

/// corresponds to Maple's coeffs:
/// var has to be the number of a variable
matrix mp_Coeffs(ideal I, int var, const ring r);

matrix mp_CoeffProc (poly f, poly vars, const ring r);
/// corresponds to Macauley's coef:
/// the exponent vector of vars has to contain the variables, eg 'xy';
/// then the poly f is searched for monomials in x and y, these monimials
/// are written to the first row of the matrix co.
/// the second row of co contains the respective factors in f.
/// Thus f = sum co[1,i]*co[2,i], i = 1..cols, rows equals 2.
void mp_Coef2(poly v, poly vars, matrix *c, matrix *m, const ring r);

/// for minors with Bareiss
void mp_RecMin(int, ideal, int &, matrix, int, int, poly, ideal, const ring r);
// void mp_MinorToResult(ideal, int &, matrix, int, int, ideal, const ring r);

BOOLEAN mp_IsDiagUnit(matrix U, const ring r);

void iiWriteMatrix(matrix im, const char *n, int dim, const ring r, int spaces=0);
char * iiStringMatrix(matrix im, int dim, const ring r, char ch=',');

extern omBin ip_smatrix_bin;

#endif/* MATPOL_H */
