#ifndef MATPOL_H
#define MATPOL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: matpol.h,v 1.5 1997-11-13 09:09:28 siebert Exp $ */
/*
* ABSTRACT
*/
#include "structs.h"

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

#ifdef MDEBUG
matrix mpDBNew(int r, int c, char *f, int l);
#define mpNew(A,B) mpDBNew(A,B,__FILE__,__LINE__)
#else
matrix mpNew(int r, int c);
#endif
matrix mpCopy (matrix a);
matrix mpInitP(int r, int c, poly p);
matrix mpInitI(int r, int c, int v);
matrix mpMultI(matrix a, int f);
matrix mpMultP(matrix a, poly p);
matrix mpAdd(matrix a, matrix b);
matrix mpSub(matrix a, matrix b);
matrix mpMult(matrix a, matrix b);
matrix mpTransp(matrix a);
BOOLEAN mpEqual(matrix a, matrix b);
poly mpTrace ( matrix a);
poly TraceOfProd ( matrix a, matrix b, int n);

matrix mpBareiss (matrix a, BOOLEAN diagonal);
matrix mpOneStepBareiss (matrix a, poly *H, int *r, int *c);
poly mpDet (matrix m);
matrix mpWedge(matrix a, int ar);
BOOLEAN mpJacobi(leftv res,leftv a);
BOOLEAN mpKoszul(leftv res,leftv b/*in*/, leftv c/*ip*/, leftv id=NULL);

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

#endif
