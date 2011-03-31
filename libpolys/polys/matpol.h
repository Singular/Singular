#ifndef MATPOL_H
#define MATPOL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/
#include <kernel/structs.h>
#include <kernel/ring.h>

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

matrix mpNew(int r, int c);
matrix mpCopy(matrix a);
void   mpDelete(matrix* a, const ring r = currRing);
matrix mpCopy(const matrix a, const ring rSrc, const ring rDst = currRing);
matrix mpInitP(int r, int c, poly p);
matrix mpInitI(int r, int c, int v);
matrix mpMultI(matrix a, int f);
matrix mpMultP(matrix a, poly p);
matrix pMultMp(poly p, matrix a);
matrix mpAdd(matrix a, matrix b);
matrix mpSub(matrix a, matrix b);
matrix mpMult(matrix a, matrix b);
matrix mpTransp(matrix a);
BOOLEAN mpEqual(matrix a, matrix b);
poly mpTrace ( matrix a);
poly TraceOfProd ( matrix a, matrix b, int n);

poly mpDet (matrix m);
matrix mpWedge(matrix a, int ar);
BOOLEAN mpJacobi(leftv res,leftv a);
BOOLEAN mpKoszul(leftv res,leftv b/*in*/, leftv c/*ip*/, leftv id=NULL);
poly mpDetBareiss (matrix a);

//matrix mpHomogen(matrix a, int v);

void   mpMonomials(matrix c, int r, int var, matrix m);
matrix mpCoeffs(ideal I, int var);
/* corresponds to Maple's coeffs:
*var has to be the number of a variable
*/

matrix mpCoeffProc (poly f, poly vars);
/* corresponds to Macauley's coef:
the exponent vector of vars has to contain the variables, eg 'xy';
then the poly f is searched for monomials in x and y, these monimials
are written to the first row of the matrix co.
the second row of co contains the respective factors in f.
Thus f = sum co[1,i]*co[2,i], i = 1..cols, rows equals 2. */
void mpCoef2(poly v, poly vars, matrix *c, matrix *m);
/* for minors with Bareiss */
void mpRecMin(int, ideal, int &, matrix, int, int, poly, ideal);
void mpMinorToResult(ideal, int &, matrix, int, int, ideal);

BOOLEAN mpIsDiagUnit(matrix U);
void iiWriteMatrix(matrix im, const char *n, int dim,int spaces=0);
char * iiStringMatrix(matrix im, int dim,char ch=',');

extern omBin ip_smatrix_bin;
#endif
