/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: matpol.cc,v 1.14 1998-07-15 14:40:29 pohl Exp $ */

/*
* ABSTRACT:
*/

#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "mod2.h"
#include "tok.h"
#include "lists.h"
#include "polys.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "ideals.h"
#include "ipid.h"
#include "subexpr.h"
#include "intvec.h"
#include "matpol.h"

/*0 implementation*/

typedef int perm[100];
static poly mpDivide(poly a, poly b);
static void mpReplace(int j, int n, int &sign, int *perm);
static float mpPolyWeight(poly p);
static int nextperm(perm * z, int max);
static poly mpLeibnitz(matrix a);
static poly minuscopy (poly p);
static poly pInsert(poly p1, poly p2);
static poly select (poly fro, poly what);
static poly exdiv ( poly m, poly d);

/*2
* create a r x c zero-matrix
*/
#ifdef MDEBUG
matrix mpDBNew(int r, int c, char *f, int l)
#else
matrix mpNew(int r, int c)
#endif
{
  if (r<=0) r=1;
  if ( (((int)(INT_MAX/sizeof(poly))) / r) <= c)
  {
    Werror("internal error: creating matrix[%d][%d]",r,c);
    return NULL;
  }
#ifdef MDEBUG
  matrix rc = (matrix)mmDBAllocBlock(sizeof(ip_smatrix),f,l);
#else
  matrix rc = (matrix)Alloc(sizeof(ip_smatrix));
#endif
  rc->nrows = r;
  rc->ncols = c;
  rc->rank = r;
  if (c != 0)
  {
    int s=r*c*sizeof(poly);
#ifdef MDEBUG
    rc->m = (polyset)mmDBAllocBlock0(s,f,l);
#else
    rc->m = (polyset)Alloc0(s);
#endif
    //if (rc->m==NULL)
    //{
    //  Werror("internal error: creating matrix[%d][%d]",r,c);
    //  return NULL;
    //}
  }
  return rc;
}

/*2
*copies matrix a to b
*/
matrix mpCopy (matrix a)
{
  poly t;
  int i, m=MATROWS(a), n=MATCOLS(a);
  matrix b = mpNew(m, n);

  for (i=m*n-1; i>=0; i--)
  {
    t = a->m[i];
    pNormalize(t);
#ifdef MDEBUG
//    b->m[i] = pDBCopy(t,f,l);
    b->m[i] = pCopy(t);
#else
    b->m[i] = pCopy(t);
#endif
  }
  b->rank=a->rank;
  return b;
}

/*2
* make it a p * unit matrix
*/
matrix mpInitP(int r, int c, poly p)
{
  matrix rc = mpNew(r,c);
  int i=min(r,c), n = c*(i-1)+i-1, inc = c+1;

  pNormalize(p);
  while (n>0)
  {
    rc->m[n] = pCopy(p);
    n -= inc;
  }
  rc->m[0]=p;
  return rc;
}

/*2
* make it a v * unit matrix
*/
matrix mpInitI(int r, int c, int v)
{
  return mpInitP(r,c,pISet(v));
}

/*2
* c = f*a
*/
matrix mpMultI(matrix a, int f)
{
  int k, n = a->nrows, m = a->ncols;
  poly p = pISet(f);
  matrix c = mpNew(n,m);

  for (k=m*n-1; k>0; k--)
    c->m[k] = pMult(pCopy(a->m[k]), pCopy(p));
  c->m[0] = pMult(pCopy(a->m[0]), p);
  return c;
}

/*2
* multiply a matrix 'a' by a poly 'p', destroy the args
*/
matrix mpMultP(matrix a, poly p)
{
  int k, n = a->nrows, m = a->ncols;

  pNormalize(p);
  for (k=m*n-1; k>0; k--)
    a->m[k] = pMult(a->m[k], pCopy(p));
  a->m[0] = pMult(a->m[0], p);
  return a;
}

matrix mpAdd(matrix a, matrix b)
{
  int k, n = a->nrows, m = a->ncols;
  if ((n != b->nrows) || (m != b->ncols))
  {
/*
*    Werror("cannot add %dx%d matrix and %dx%d matrix",
*      m,n,b->cols(),b->rows());
*/
    return NULL;
  }
  matrix c = mpNew(n,m);
  for (k=m*n-1; k>=0; k--)
    c->m[k] = pAdd(pCopy(a->m[k]), pCopy(b->m[k]));
  return c;
}

matrix mpSub(matrix a, matrix b)
{
  int k, n = a->nrows, m = a->ncols;
  if ((n != b->nrows) || (m != b->ncols))
  {
/*
*    Werror("cannot sub %dx%d matrix and %dx%d matrix",
*      m,n,b->cols(),b->rows());
*/
    return NULL;
  }
  matrix c = mpNew(n,m);
  for (k=m*n-1; k>=0; k--)
    c->m[k] = pSub(pCopy(a->m[k]), pCopy(b->m[k]));
  return c;
}

matrix mpMult(matrix a, matrix b)
{
  int i, j, k;
  poly s, t, aik, bkj;
  int m = MATROWS(a);
  int p = MATCOLS(a);
  int q = MATCOLS(b);

  if (p!=MATROWS(b))
  {
/*
*   Werror("cannot multiply %dx%d matrix and %dx%d matrix",
*     m,p,b->rows(),q);
*/
    return NULL;
  }
  matrix c = mpNew(m,q);

  for (i=1; i<=m; i++)
  {
    for (j=1; j<=q; j++)
    {
      t = NULL;
      for (k=1; k<=p; k++)
      {
        aik = pCopy(MATELEM(a,i,k));
        bkj = pCopy(MATELEM(b,k,j));
        s = pMult(aik,bkj);
        t = pAdd(t,s);
      }
      pNormalize(t);
      MATELEM(c,i,j) = t;
    }
  }
  return c;
}

matrix mpTransp(matrix a)
{
  int    i, j, r = MATROWS(a), c = MATCOLS(a);
  poly *p;
  matrix b =  mpNew(c,r);

  p = b->m;
  for (i=0; i<c; i++)
  {
    for (j=0; j<r; j++)
    {
      *p++ = pCopy(a->m[j*c+i]);
    }
  }
  return b;
}

/*2
*returns the trace of matrix a
*/
poly mpTrace ( matrix a)
{
  int i;
  int n = (MATCOLS(a)<MATROWS(a)) ? MATCOLS(a) : MATROWS(a);
  poly  t = NULL;

  for (i=1; i<=n; i++)
    t = pAdd(t, pCopy(MATELEM(a,i,i)));
  return t;
}

/*2
*returns the trace of the product of a and b
*/
poly TraceOfProd ( matrix a, matrix b, int n)
{
  int i, j;
  poly  p, t = NULL;

  for (i=1; i<=n; i++)
  {
    for (j=1; j<=n; j++)
    {
      p = pMult(pCopy(MATELEM(a,i,j)), pCopy(MATELEM(b,j,i)));
      t = pAdd(t, p);
    }
  }
  return t;
}

/*
* C++ classes for Bareiss algorithm
*/
class row_col_weight
{
  private:
  int ym, yn;
  public:
  float *wrow, *wcol;
  row_col_weight() : ym(0) {}
  row_col_weight(int, int);
  ~row_col_weight();
};

/*2
*  a submatrix M of a matrix X[m,n]:
*    0 <= i < s_m <= a_m
*    0 <= j < s_n <= a_n
*    M = ( Xarray[qrow[i],qcol[j]] )
*    if a_m = a_n and s_m = s_n
*      det(X) = sign*div^(s_m-1)*det(M)
*    resticted pivot for elimination
*      0 <= j < piv_s
*/
class mp_permmatrix
{
  private:
  int       a_m, a_n, s_m, s_n, sign, piv_s;
  int       *qrow, *qcol;
  poly      *Xarray;
  void mpInitMat();
  poly * mpRowAdr(int);
  poly * mpColAdr(int);
  void mpRowWeight(float *);
  void mpColWeight(float *);
  void mpRowSwap(int, int);
  void mpColSwap(int, int);
  public:
  mp_permmatrix() : a_m(0) {}
  mp_permmatrix(matrix);
  mp_permmatrix(mp_permmatrix *);
  ~mp_permmatrix();
  int mpGetRow();
  int mpGetCol();
  int mpGetRdim();
  int mpGetCdim();
  int mpGetSign();
  void mpSetSearch(int s);
  void mpSaveArray();
  poly mpGetElem(int, int);
  void mpSetElem(poly, int, int);
  void mpDelElem(int, int);
  void mpElimBareiss(poly);
  int mpPivotBareiss(row_col_weight *);
  int mpPivotRow(row_col_weight *, int);
  void mpToIntvec(intvec *);
  void mpRowReorder();
  void mpColReorder();
};

/*2
*  caller of 'Bareiss' algorithm,
*  return an list of a matrix and an intvec:
*    the matrix is lower triangular and the result,
*    the intvec is the performed permutation of columns.
*/
lists mpBareiss (matrix a, BOOLEAN sw)
{
  poly div;
  matrix c = mpCopy(a);
  mp_permmatrix *Bareiss = new mp_permmatrix(c);
  row_col_weight w(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());
  intvec *v = new intvec(Bareiss->mpGetCdim());
  lists res=(lists)Alloc(sizeof(slists));

  if (sw) WarnS(feNotImplemented);
  /* Bareiss */
  div = NULL;
  while(Bareiss->mpPivotBareiss(&w))
  {
    Bareiss->mpElimBareiss(div);
    div = Bareiss->mpGetElem(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());
  }
  Bareiss->mpToIntvec(v);
  Bareiss->mpRowReorder();
  Bareiss->mpColReorder();
  Bareiss->mpSaveArray();
  delete Bareiss;

  res->Init(2);
  res->m[0].rtyp=MATRIX_CMD;
  res->m[0].data=(void *)c;
  res->m[1].rtyp=INTVEC_CMD;
  res->m[1].data=(void *)v;
  return res;
}

/*2
*  one step of 'Bareiss' algorithm
*  for application in minor
*  assume to have a full matrix
*  if *H!=0, then divide by *H (the pivot from the step before)
*  returns the choosen pivot *H=m[*r,*c]
*  the result has the pivot at the right lower corner
*/
matrix mpOneStepBareiss (matrix a, poly *H, int *r, int *c)
{
  poly div=*H;
  matrix re = mpCopy(a);
  mp_permmatrix *Bareiss = new mp_permmatrix(re);
  row_col_weight w(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());
  int row = *r;

  /* step of Bareiss */
  if((row && Bareiss->mpPivotRow(&w,row-1)) || Bareiss->mpPivotBareiss(&w))
  {
    Bareiss->mpElimBareiss(div);
    div = Bareiss->mpGetElem(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());
    *H = pCopy(div);
    *c = Bareiss->mpGetCol()+1;
    *r = Bareiss->mpGetRow()+1;
    Bareiss->mpRowReorder();
    Bareiss->mpColReorder();
  }
  else
  {
    *H = NULL;
    *c = *r = 0;
  }
  Bareiss->mpSaveArray();
  delete Bareiss;
  return re;
}

/*2
*returns the determinant of the matrix m;
*uses Bareiss algorithm
*/
poly mpDetBareiss (matrix a)
{
  int s;
  poly div, res;
  if (MATROWS(a) != MATCOLS(a))
  {
    Werror("det of %d x %d matrix",MATROWS(a),MATCOLS(a));
    return NULL;
  }
  matrix c = mpCopy(a);
  mp_permmatrix *Bareiss = new mp_permmatrix(c);
  row_col_weight w(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());

  /* Bareiss */
  div = NULL;
  while(Bareiss->mpPivotBareiss(&w))
  {
    Bareiss->mpElimBareiss(div);
    div = Bareiss->mpGetElem(Bareiss->mpGetRdim(), Bareiss->mpGetCdim());
  }
  Bareiss->mpRowReorder();
  Bareiss->mpColReorder();
  Bareiss->mpSaveArray();
  s = Bareiss->mpGetSign();
  delete Bareiss;

  /* result */
  res = MATELEM(c,1,1);
  MATELEM(c,1,1) = NULL;
  idDelete((ideal *)&c);
  if (s < 0)
    res = pNeg(res);
  return res;
}

/*2
*returns the determinant of the matrix m;
*uses Newtons formulea for symmetric functions
*/
poly mpDet (matrix m)
{
  int i,j,k,n;
  poly p,q;
  matrix a, s;
  matrix ma[100];
  number c=NULL, d=NULL, ONE=NULL;

  n = MATROWS(m);
  if (n != MATCOLS(m))
  {
    Werror("det of %d x %d matrix",n,MATCOLS(m));
    return NULL;
  }
  k=currRing->ch;
  if (k<0) k=-k;
  else if (k==1) k=0;
  if (((k > 0) && (k <= n))
#ifdef SRING
  || (pSRING)
#endif
  )
    return mpLeibnitz(m);
  ONE = nInit(1);
  ma[1]=mpCopy(m);
  k = (n+1) / 2;
  s = mpNew(1, n);
  MATELEM(s,1,1) = mpTrace(m);
  for (i=2; i<=k; i++)
  {
    //ma[i] = mpNew(n,n);
    ma[i]=mpMult(ma[i-1], ma[1]);
    MATELEM(s,1,i) = mpTrace(ma[i]);
    pTest(MATELEM(s,1,i));
  }
  for (i=k+1; i<=n; i++)
  {
    MATELEM(s,1,i) = TraceOfProd(ma[i / 2], ma[(i+1) / 2], n);
    pTest(MATELEM(s,1,i));
  }
  for (i=1; i<=k; i++)
    idDelete((ideal *)&(ma[i]));
/* the array s contains the traces of the powers of the matrix m,
*  these are the power sums of the eigenvalues of m */
  a = mpNew(1,n);
  MATELEM(a,1,1) = minuscopy(MATELEM(s,1,1));
  for (i=2; i<=n; i++)
  {
    p = pCopy(MATELEM(s,1,i));
    pTest(p);
    for (j=i-1; j>=1; j--)
    {
      q = pMult(pCopy(MATELEM(s,1,j)), pCopy(MATELEM(a,1,i-j)));
      pTest(q);
      p = pAdd(p,q);
    }
    // c= -1/i
    d = nInit(-(int)i);
    c = nDiv(ONE, d);
    nDelete(&d);

    pMultN(p, c);
    pTest(p);
    MATELEM(a,1,i) = p;
    nDelete(&c);
  }
/* the array a contains the elementary symmetric functions of the
*  eigenvalues of m */
  for (i=1; i<=n-1; i++)
  {
    //pDelete(&(MATELEM(a,1,i)));
    pDelete(&(MATELEM(s,1,i)));
  }
  pDelete(&(MATELEM(s,1,n)));
/* up to a sign, the determinant is the n-th elementary symmetric function */
  if ((n/2)*2 < n)
  {
    d = nInit(-1);
    pMultN(MATELEM(a,1,n), d);
    nDelete(&d);
  }
  nDelete(&ONE);
  idDelete((ideal *)&s);
  poly result=MATELEM(a,1,n);
  MATELEM(a,1,n)=NULL;
  idDelete((ideal *)&a);
  return result;
}

/*2
* compute all ar-minors of the matrix a
*/
matrix mpWedge(matrix a, int ar)
{
  int     i,j,k,l;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  matrix result;
  matrix tmp;
  poly p;

  i = binom(a->nrows,ar);
  j = binom(a->ncols,ar);

  rowchoise=(int *)Alloc(ar*sizeof(int));
  colchoise=(int *)Alloc(ar*sizeof(int));
  result =mpNew(i,j);
  tmp=mpNew(ar,ar);
  l = 1; /* k,l:the index in result*/
  idInitChoise(ar,1,a->nrows,&rowch,rowchoise);
  while (!rowch)
  {
    k=1;
    idInitChoise(ar,1,a->ncols,&colch,colchoise);
    while (!colch)
    {
      for (i=1; i<=ar; i++)
      {
        for (j=1; j<=ar; j++)
        {
          MATELEM(tmp,i,j) = MATELEM(a,rowchoise[i-1],colchoise[j-1]);
        }
      }
      p = mpDet(tmp);
      if ((k+l) & 1) p=pNeg(p);
      MATELEM(result,l,k) = p;
      k++;
      idGetNextChoise(ar,a->ncols,&colch,colchoise);
    }
    idGetNextChoise(ar,a->nrows,&rowch,rowchoise);
    l++;
  }
  /*delete the matrix tmp*/
  for (i=1; i<=ar; i++)
  {
    for (j=1; j<=ar; j++) MATELEM(tmp,i,j) = NULL;
  }
  idDelete((ideal *) &tmp);
  return (result);
}

/*2
* compute the jacobi matrix of an ideal
*/
BOOLEAN mpJacobi(leftv res,leftv a)
{
  int     i,j;
  matrix result;
  ideal id=(ideal)a->Data();

  result =mpNew(IDELEMS(id),pVariables);
  for (i=1; i<=IDELEMS(id); i++)
  {
    for (j=1; j<=pVariables; j++)
    {
      MATELEM(result,i,j) = pDiff(id->m[i-1],j);
    }
  }
  res->data=(char *)result;
  return FALSE;
}

/*2
* returns the Koszul-matrix of degree d of a vectorspace with dimension n
* uses the first n entrees of id, if id <> NULL
*/
BOOLEAN mpKoszul(leftv res,leftv b/*in*/, leftv c/*ip*/,leftv id)
{
  int n=(int)b->Data();
  int d=(int)c->Data();
  int     k,l,sign,row,col;
  matrix  result;
  ideal temp;
  BOOLEAN bo;
  poly    p;

  if ((d>n) || (d<1) || (n<1))
  {
    res->data=(char *)mpNew(1,1);
    return FALSE;
  }
  int *choise = (int*)Alloc(d*sizeof(int));
  if (id==NULL)
    temp=idMaxIdeal(1);
  else
    temp=(ideal)id->Data();

  k = binom(n,d);
  l = k*d;
  l /= n-d+1;
  result =mpNew(l,k);
  col = 1;
  idInitChoise(d,1,n,&bo,choise);
  while (!bo)
  {
    sign = 1;
    for (l=1;l<=d;l++)
    {
      if (choise[l-1]<=IDELEMS(temp))
      {
        p = pCopy(temp->m[choise[l-1]-1]);
        if (sign == -1) p = pNeg(p);
        sign *= -1;
        row = idGetNumberOfChoise(l-1,d,1,n,choise);
        MATELEM(result,row,col) = p;
      }
    }
    col++;
    idGetNextChoise(d,n,&bo,choise);
  }
  if (id==NULL) idDelete(&temp);

  res->data=(char *)result;
  return FALSE;
}

///*2
//*homogenize all elements of matrix (not the matrix itself)
//*/
//matrix mpHomogen(matrix a, int v)
//{
//  int i,j;
//  poly p;
//
//  for (i=1;i<=MATROWS(a);i++)
//  {
//    for (j=1;j<=MATCOLS(a);j++)
//    {
//      p=pHomogen(MATELEM(a,i,j),v);
//      pDelete(&(MATELEM(a,i,j)));
//      MATELEM(a,i,j)=p;
//    }
//  }
//  return a;
//}

/*2
* corresponds to Maple's coeffs:
* var has to be the number of a variable
*/
matrix mpCoeffs (ideal I, int var)
{
  poly h,f;
  int l, i, c, m=0;
  matrix co;
  /* look for maximal power m of x_var in I */
  for (i=IDELEMS(I)-1; i>=0; i--)
  {
    f=I->m[i];
    while (f!=NULL)
    {
      l=pGetExp(f,var);
      if (l>m) m=l;
      pIter(f);
    }
  }
  co=mpNew((m+1)*I->rank,IDELEMS(I));
  /* divide each monomial by a power of x_var,
  * remember the power in l and the component in c*/
  for (i=IDELEMS(I)-1; i>=0; i--)
  {
    f=I->m[i];
    while (f!=NULL)
    {
      l=pGetExp(f,var);
      pSetExp(f,var,0);
      c=max(pGetComp(f),1);
      pSetComp(f,0);
      pSetm(f);
      /* now add the resulting monomial to co*/
      h=pNext(f);
      pNext(f)=NULL;
      //MATELEM(co,c*(m+1)-l,i+1)
      //  =pAdd(MATELEM(co,c*(m+1)-l,i+1),f);
      MATELEM(co,(c-1)*(m+1)+l+1,i+1)
        =pAdd(MATELEM(co,(c-1)*(m+1)+l+1,i+1),f);
      /* iterate f*/
      f=h;
    }
  }
  return co;
}

/*2
* given the result c of mpCoeffs(ideal/module i, var)
* i of rank r
* build the matrix of the corresponding monomials in m
*/
void   mpMonomials(matrix c, int r, int var, matrix m)
{
  /* clear contents of m*/
  int k,l;
  for (k=MATROWS(m);k>0;k--)
  {
    for(l=MATCOLS(m);l>0;l--)
    {
      pDelete(&MATELEM(m,k,l));
    }
  }
  Free((ADDRESS)m->m,MATROWS(m)*MATCOLS(m)*sizeof(poly));
  /* allocate monoms in the right size r x MATROWS(c)*/
  m->m=(polyset)Alloc0(r*MATROWS(c)*sizeof(poly));
  MATROWS(m)=r;
  MATCOLS(m)=MATROWS(c);
  m->rank=r;
  /* the maximal power p of x_var: MATCOLS(m)=r*(p+1) */
  int p=MATCOLS(m)/r-1;
  /* fill in the powers of x_var=h*/
  poly h=pOne();
  for(k=r;k>0; k--)
  {
    MATELEM(m,k,k*(p+1))=pOne();
  }
  for(l=p;l>0; l--)
  {
    pSetExp(h,var,l);
    pSetm(h);
    for(k=r;k>0; k--)
    {
      MATELEM(m,k,k*(p+1)-l)=pCopy(h);
    }
  }
  pDelete(&h);
}

matrix mpCoeffProc (poly f, poly vars)
{
  poly sel, h;
  int l, i;
  matrix co;
  if (f==NULL)
  {
    co = mpNew(2, 1);
    MATELEM(co,1,1) = pOne();
    MATELEM(co,2,1) = NULL;
    return co;
  }
  sel = select(f, vars);
  l = pLength(sel);
  co = mpNew(2, l);
  if (pOrdSgn==-1)
  {
    for (i=l; i>=1; i--)
    {
      h = pHead(sel);
      MATELEM(co,1,i) = h;
      MATELEM(co,2,i) = NULL;
      sel = sel->next;
    }
  }
  else
  {
    for (i=1; i<=l; i++)
    {
      h = pHead(sel);
      MATELEM(co,1,i) = h;
      MATELEM(co,2,i) = NULL;
      sel = sel->next;
    }
  }
  while (f!=NULL)
  {
    i = 1;
    loop
    {
      h = exdiv(f, MATELEM(co,1,i));
      if (h!=NULL)
      {
        MATELEM(co,2,i) = pAdd(MATELEM(co,2,i), h);
        break;
      }
      if (i < l)
        i++;
      else
        break;
    }
    pIter(f);
  }
  return co;
}

void mpCoef2(poly v, poly mon, matrix *c, matrix *m)
{
  polyset s;
  poly p;
  int sl,i,j;
  int l=0;
  poly sel=select(v,mon);

  pVec2Polys(sel,&s,&sl);
  for (i=0; i<sl; i++)
    l=max(l,pLength(s[i]));
  *c=mpNew(sl,l);
  *m=mpNew(sl,l);
  poly h;
  int isConst;
  for (j=1; j<=sl;j++)
  {
    p=s[j-1];
    if (pIsConstant(p)) /*p != NULL */
    {
      isConst=-1;
      i=l;
    }
    else
    {
      i=1;
      isConst=1;
    }
    while(p!=NULL)
    {
      h = pHead(p);
      MATELEM(*m,j,i) = h;
      i+=isConst;
      p = p->next;
    }
  }
  while (v!=NULL)
  {
    i = 1;
    j = pGetComp(v);
    loop
    {
      h = exdiv(v, MATELEM(*m,j,i));
      if (h!=NULL)
      {
        pSetComp(h,0);
        MATELEM(*c,j,i) = pAdd(MATELEM(*c,j,i), h);
        break;
      }
      if (i < l)
        i++;
      else
        break;
    }
    v = v->next;
  }
}


BOOLEAN mpEqual(matrix a, matrix b)
{
  if ((MATCOLS(a)!=MATCOLS(b)) || (MATROWS(a)!=MATROWS(b)))
    return FALSE;
  int i=MATCOLS(a)*MATROWS(b)-1;
  while (i>=0)
  {
    if (a->m[i]==NULL)
    {
      if (b->m[i]!=NULL) return FALSE;
    }
    else
      if (pComp(a->m[i],b->m[i])!=0) return FALSE;
    i--;
  }
  i=MATCOLS(a)*MATROWS(b)-1;
  while (i>=0)
  {
    if (pSub(pCopy(a->m[i]),pCopy(b->m[i]))!=NULL) return FALSE;
    i--;
  }
  return TRUE;
}

/* --------------- internal stuff ------------------- */

row_col_weight::row_col_weight(int i, int j)
{
  ym = i;
  yn = j;
  wrow = (float *)Alloc(i*sizeof(float));
  wcol = (float *)Alloc(j*sizeof(float));
}

row_col_weight::~row_col_weight()
{
  if (ym!=0)
  {
    Free((ADDRESS)wcol, yn*sizeof(float));
    Free((ADDRESS)wrow, ym*sizeof(float));
  }
}

mp_permmatrix::mp_permmatrix(matrix A) : sign(1)
{
  a_m = A->nrows;
  a_n = A->ncols;
  this->mpInitMat();
  Xarray = A->m;
}

mp_permmatrix::mp_permmatrix(mp_permmatrix *M)
{
  poly p, *athis, *aM;
  int i, j;

  a_m = M->s_m;
  a_n = M->s_n;
  sign = M->sign;
  this->mpInitMat();
  Xarray = (poly *)Alloc0(a_m*a_n*sizeof(poly));
  for (i=a_m-1; i>=0; i--)
  {
    athis = this->mpRowAdr(i);
    aM = M->mpRowAdr(i);
    for (j=a_n-1; j>=0; j--)
    {
      p = aM[M->qcol[j]];
      if (p)
      {
        athis[j] = pCopy(p);
      }
    }
  }
}

mp_permmatrix::~mp_permmatrix()
{
  int k;

  if (a_m != 0)
  {
    Free((ADDRESS)qrow,a_m*sizeof(int));
    Free((ADDRESS)qcol,a_n*sizeof(int));
    if (Xarray != NULL)
    {
      for (k=a_m*a_n-1; k>=0; k--)
        pDelete(&Xarray[k]);
      Free((ADDRESS)Xarray,a_m*a_n*sizeof(poly));
    }
  }
}

int mp_permmatrix::mpGetRdim() { return s_m; }

int mp_permmatrix::mpGetCdim() { return s_n; }

int mp_permmatrix::mpGetSign() { return sign; }

void mp_permmatrix::mpSetSearch(int s) { piv_s = s; }

void mp_permmatrix::mpSaveArray() { Xarray = NULL; }

poly mp_permmatrix::mpGetElem(int r, int c)
{
  return Xarray[a_n*qrow[r]+qcol[c]];
}

void mp_permmatrix::mpSetElem(poly p, int r, int c)
{
  Xarray[a_n*qrow[r]+qcol[c]] = p;
}

void mp_permmatrix::mpDelElem(int r, int c)
{
  pDelete(&Xarray[a_n*qrow[r]+qcol[c]]);
}

/*
* the Bareiss-type elimination with division by div (div != NULL)
*/
void mp_permmatrix::mpElimBareiss(poly div)
{
  poly piv, elim, q1, q2, *ap, *a;
  int i, j, jj;

  ap = this->mpRowAdr(s_m);
  piv = ap[qcol[s_n]];
  for(i=s_m-1; i>=0; i--)
  {
    a = this->mpRowAdr(i);
    elim = a[qcol[s_n]];
    if (elim != NULL)
    {
      for (j=s_n-1; j>=0; j--)
      {
        q2 = NULL;
        jj = qcol[j];
        q1 = a[jj];
        if (ap[jj] != NULL)
        {
          q2 = pNeg(pCopy(ap[jj]));
          q2 = pMult(q2, pCopy(elim));
          if (q1 != NULL)
          {
            q1 = pMult(q1,pCopy(piv));
            q2 = pAdd(q2, q1);
          }
        }
        else if (q1 != NULL)
        {
          q2 = pMult(q1, pCopy(piv));
        }
        if (q2 && div)
          q2 = mpDivide(q2, div);
        a[jj] = q2;
      }
      pDelete(&a[qcol[s_n]]);
    }
    else
    {
      for (j=s_n-1; j>=0; j--)
      {
        jj = qcol[j];
        q1 = a[jj];
        if (q1 != NULL)
        {
          q1 = pMult(q1, pCopy(piv));
          if (div)
            q1 = mpDivide(q1, div);
          a[jj] = q1;
        }
      }
    }
  }
}

/*2
* pivot strategy for Bareiss algorithm
*/
int mp_permmatrix::mpPivotBareiss(row_col_weight *C)
{
  poly p, *a;
  int i, j, iopt, jopt;
  float sum, f1, f2, fo, r, ro, lp;
  float *dr = C->wrow, *dc = C->wcol;

  fo = 1.0e20;
  ro = 0.0;
  iopt = jopt = -1;
  
  s_n--;
  s_m--;
  if (s_m == 0)
    return 0;
  if (s_n == 0)
  {
    for(i=s_m; i>=0; i--)
    {
      p = this->mpRowAdr(i)[qcol[0]];
      if (p)
      {
        f1 = mpPolyWeight(p);
        if (f1 < fo)
        {
          fo = f1;
          if (iopt >= 0)
            pDelete(&(this->mpRowAdr(iopt)[qcol[0]]));
          iopt = i;
        }
        else
          pDelete(&(this->mpRowAdr(i)[qcol[0]]));
      }
    }
    if (iopt >= 0)
      mpReplace(iopt, s_m, sign, qrow);
    return 0;
  }
  this->mpRowWeight(dr);
  this->mpColWeight(dc);
  sum = 0.0;
  for(i=s_m; i>=0; i--)
    sum += dr[i];
  for(i=s_m; i>=0; i--)
  {
    r = dr[i];
    a = this->mpRowAdr(i);
    for(j=s_n; j>=0; j--)
    {
      p = a[qcol[j]];
      if (p)
      {
        lp = mpPolyWeight(p);
        ro = r - lp;
        f1 = ro * (dc[j]-lp);
        if (f1 != 0.0)
        {
          f2 = lp * (sum - ro - dc[j]);
          f2 += f1;
        }
        else
          f2 = lp-r-dc[j];
        if (f2 < fo)
        {
          fo = f2;
          iopt = i;
          jopt = j;
        }
      }
    }
  }
  if (iopt < 0)
    return 0;
  mpReplace(iopt, s_m, sign, qrow);
  mpReplace(jopt, s_n, sign, qcol);
  return 1;
}

/*2
* pivot strategy for Bareiss algorithm with defined row
*/
int mp_permmatrix::mpPivotRow(row_col_weight *C, int row)
{
  poly p, *a;
  int j, iopt, jopt;
  float sum, f1, f2, fo, r, ro, lp;
  float *dr = C->wrow, *dc = C->wcol;

  fo = 1.0e20;
  ro = 0.0;
  iopt = jopt = -1;
  
  s_n--;
  s_m--;
  if (s_m == 0)
    return 0;
  if (s_n == 0)
  {
    p = this->mpRowAdr(row)[qcol[0]];
    if (p)
    {
      f1 = mpPolyWeight(p);
      if (f1 < fo)
      {
        fo = f1;
        if (iopt >= 0)
        pDelete(&(this->mpRowAdr(iopt)[qcol[0]]));
        iopt = row;
      }
      else
        pDelete(&(this->mpRowAdr(row)[qcol[0]]));
    }
    if (iopt >= 0)
      mpReplace(iopt, s_m, sign, qrow);
    return 0;
  }
  this->mpRowWeight(dr);
  this->mpColWeight(dc);
  sum = 0.0;
  for(j=s_m; j>=0; j--)
    sum += dr[j];
  r = dr[row];
  a = this->mpRowAdr(row);
  for(j=s_n; j>=0; j--)
  {
    p = a[qcol[j]];
    if (p)
    {
      lp = mpPolyWeight(p);
      ro = r - lp;
      f1 = ro * (dc[j]-lp);
      if (f1 != 0.0)
      {
        f2 = lp * (sum - ro - dc[j]);
        f2 += f1;
      }
      else
        f2 = lp-r-dc[j];
      if (f2 < fo)
      {
        fo = f2;
        iopt = row;
        jopt = j;
      }
    }
  }
  if (iopt < 0)
    return 0;
  mpReplace(iopt, s_m, sign, qrow);
  mpReplace(jopt, s_n, sign, qcol);
  return 1;
}

void mp_permmatrix::mpToIntvec(intvec *v)
{
  int i;

  for (i=v->rows()-1; i>=0; i--)
    (*v)[i] = qcol[i]+1; 
}

void mp_permmatrix::mpRowReorder()
{
  int k, i, i1, i2;

  if (a_m > a_n)
    k = a_m - a_n;
  else
    k = 0;
  for (i=a_m-1; i>=k; i--)
  {
    i1 = qrow[i];
    if (i1 != i)
    {
      this->mpRowSwap(i1, i);
      i2 = 0;
      while (qrow[i2] != i) i2++;
      qrow[i2] = i1;
    }
  }
}

void mp_permmatrix::mpColReorder()
{
  int k, j, j1, j2;

  if (a_n > a_m)
    k = a_n - a_m;
  else
    k = 0;
  for (j=a_n-1; j>=k; j--)
  {
    j1 = qcol[j];
    if (j1 != j)
    {
      this->mpColSwap(j1, j);
      j2 = 0;
      while (qcol[j2] != j) j2++;
      qcol[j2] = j1;
    }
  }
}

// private
void mp_permmatrix::mpInitMat()
{
  int k;

  s_m = a_m;
  s_n = a_n;
  piv_s = 0;
  qrow = (int *)Alloc(a_m*sizeof(int));
  qcol = (int *)Alloc(a_n*sizeof(int));
  for (k=a_m-1; k>=0; k--) qrow[k] = k;
  for (k=a_n-1; k>=0; k--) qcol[k] = k;
}

poly * mp_permmatrix::mpRowAdr(int r)
{
  return &(Xarray[a_n*qrow[r]]);
}

poly * mp_permmatrix::mpColAdr(int c)
{
  return &(Xarray[qcol[c]]);
}

void mp_permmatrix::mpRowWeight(float *wrow)
{
  poly p, *a;
  int i, j;
  float count;

  for (i=s_m; i>=0; i--)
  {
    a = this->mpRowAdr(i);
    count = 0.0;
    for(j=s_n; j>=0; j--)
    {
      p = a[qcol[j]];
      if (p)
        count += mpPolyWeight(p);
    }
    wrow[i] = count;
  }
}

void mp_permmatrix::mpColWeight(float *wcol)
{
  poly p, *a;
  int i, j;
  float count;

  for (j=s_n; j>=0; j--)
  {
    a = this->mpColAdr(j);
    count = 0.0;
    for(i=s_m; i>=0; i--)
    {
      p = a[a_n*qrow[i]];
      if (p)
        count += mpPolyWeight(p);
    }
    wcol[j] = count;
  }
}

void mp_permmatrix::mpRowSwap(int i1, int i2)
{
   poly p, *a1, *a2;
   int j;

   a1 = &(Xarray[a_n*i1]);
   a2 = &(Xarray[a_n*i2]);
   for (j=a_n-1; j>= 0; j--)
   {
     p = a1[j];
     a1[j] = a2[j];
     a2[j] = p;
   }
}

void mp_permmatrix::mpColSwap(int j1, int j2)
{
   poly p, *a1, *a2;
   int i, k = a_n*a_m;

   a1 = &(Xarray[j1]);
   a2 = &(Xarray[j2]);
   for (i=0; i< k; i+=a_n)
   {
     p = a1[i];
     a1[i] = a2[i];
     a2[i] = p;
   }
}

int mp_permmatrix::mpGetRow()
{
  return qrow[s_m];
}

int mp_permmatrix::mpGetCol()
{
  return qcol[s_n];
}

/*2
* exact division a/b, used in Bareiss algorithm
* a destroyed, b NOT destroyed
*/
static poly mpDivide(poly a, poly b)
{
  number x, y, z;
  poly r, tail, t, h, h0;
  int i, deg;

  r = a;
  x = pGetCoeff(b);
  deg = pTotaldegree(b);
  tail = pNext(b);
  if (tail == NULL)
  {
    do
    {
      if (deg != 0)
      {
        for (i=pVariables; i; i--)
          pSubExp(r,i,  pGetExp(b,i));
        pSetm(r);
      }
      y = nIntDiv(pGetCoeff(r),x);
      pSetCoeff(r,y);
      pIter(r);
    } while (r != NULL);
    return a;
  }
  h0 = pInit();
  do
  {
    if (deg != 0)
    {
      for (i=pVariables; i>0; i--)
        pSubExp(r,i,pGetExp(b,i));
      pSetm(r);
    }
    y = nDiv(pGetCoeff(r), x);
    nNormalize(y);
    pSetCoeff(r,y);
    t = tail;
    h = h0;
    do
    {
      h = pNext(h) = pInit();
      for (i=pVariables; i>0; i--)
        pSetExp(h,i, pGetExp(r,i)+pGetExp(t,i));
      pSetm(h);
      z = nMult(y, pGetCoeff(t));
      pSetCoeff0(h,nNeg(z));
      pIter(t);
    } while (t != NULL);
    pNext(h) = NULL;
    r = pNext(r) = pAdd(pNext(r),pNext(h0));
  } while (r!=NULL);
  pFree1(h0);
  return a;
}

/*
* perform replacement for pivot strategy in Bareiss algorithm
* change sign of determinant
*/
static void mpReplace(int j, int n, int &sign, int *perm)
{
  int k;

  if (j != n)
  {
    k = perm[n];
    perm[n] = perm[j];
    perm[j] = k;
    sign = -sign;
  }
}

/*
* weigth of a polynomial, for pivot strategy
* modify this for characteristic 0 !!!
*/
static float mpPolyWeight(poly p)
{
  int i;
  float res;

  if (pNext(p) == NULL)
  {
    res = (float)nSize(pGetCoeff(p));
    if (pTotaldegree(p) != 0) res += 1.0;
  }
  else
  {
    i = 0;
    res = 0.0;
    do
    {
      i++;
      res += (float)nSize(pGetCoeff(p));
      pIter(p);
    }
    while (p);
    res += (float)i;
  }
  return res;
}

static int nextperm(perm * z, int max)
{
  int s, i, k, t;
  s = max;
  do
  {
    s--;
  }
  while ((s > 0) && ((*z)[s] >= (*z)[s+1]));
  if (s==0)
    return 0;
  do
  {
    (*z)[s]++;
    k = 0;
    do
    {
      k++;
    }
    while (((*z)[k] != (*z)[s]) && (k!=s));
  }
  while (k < s);
  for (i=s+1; i <= max; i++)
  {
    (*z)[i]=0;
    do
    {
      (*z)[i]++;
      k=0;
      do
      {
        k++;
      }
      while (((*z)[k] != (*z)[i]) && (k != i));
    }
    while (k < i);
  }
  s = max+1;
  do
  {
    s--;
  }
  while ((s > 0) && ((*z)[s] > (*z)[s+1]));
  t = 1;
  for (i=1; i<max; i++)
    for (k=i+1; k<=max; k++)
      if ((*z)[k] < (*z)[i])
        t = -t;
  (*z)[0] = t;
  return s;
}

static poly mpLeibnitz(matrix a)
{
  int i, e, n;
  poly p, d;
  perm z;

  n = MATROWS(a);
  memset(&z,0,(n+2)*sizeof(int));
  p = pOne();
  for (i=1; i <= n; i++)
    p = pMult(p, pCopy(MATELEM(a, i, i)));
  d = p;
  for (i=1; i<= n; i++)
    z[i] = i;
  z[0]=1;
  e = 1;
  if (n!=1)
  {
    while (e)
    {
      e = nextperm((perm *)&z, n);
      p = pOne();
      for (i = 1; i <= n; i++)
        p = pMult(p, pCopy(MATELEM(a, i, z[i])));
      if (z[0] > 0)
        d = pAdd(d, p);
      else
        d = pSub(d, p);
    }
  }
  return d;
}

static poly minuscopy (poly p)
{
  poly w;
  number  e;
  e = nInit(-1);
  w = pCopy(p);
  pMultN(w, e);
  nDelete(&e);
  return w;
}

/*2
* insert a monomial into a list, avoid duplicates
* arguments are destroyed
*/
static poly pInsert(poly p1, poly p2)
{
  poly a1, p, a2, a;
  int c;

  if (p1==NULL) return p2;
  if (p2==NULL) return p1;
  a1 = p1;
  a2 = p2;
  a = p  = pOne();
  loop
  {
    c = pComp(a1, a2);
    if (c == 1)
    {
      a = pNext(a) = a1;
      pIter(a1);
      if (a1==NULL)
      {
        pNext(a) = a2;
        break;
      }
    }
    else if (c == -1)
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pNext(a) = a1;
        break;
      }
    }
    else
    {
      pDelete1(&a2);
      a = pNext(a) = a1;
      pIter(a1);
      if (a1==NULL)
      {
        pNext(a) = a2;
        break;
      }
      else if (a2==NULL)
      {
        pNext(a) = a1;
        break;
      }
    }
  }
  pDelete1(&p);
  return p;
}

/*2
*if what == xy the result is the list of all different power products
*    x^i*y^j (i, j >= 0) that appear in fro
*/
static poly select (poly fro, poly what)
{
  int i;
  poly h, res;
  res = NULL;
  while (fro!=NULL)
  {
    h = pOne();
    for (i=1; i<=pVariables; i++)
      pSetExp(h,i, pGetExp(fro,i) * pGetExp(what, i));
    pSetComp(h, pGetComp(fro));
    pSetm(h);
    res = pInsert(h, res);
    fro = fro->next;
  }
  return res;
}

/*2
*exact divisor: let d  == x^i*y^j, m is thought to have only one term;
*    return m/d iff d divides m, and no x^k*y^l (k>i or l>j) divides m
*/
static poly exdiv ( poly m, poly d)
{
  int i;
  poly h = pHead(m);
  for (i=1; i<=pVariables; i++)
  {
    if (pGetExp(d,i) > 0)
    {
      if (pGetExp(d,i) != pGetExp(h,i))
      {
        pDelete(&h);
        return NULL;
      }
      pSetExp(h,i,0);
    }
  }
  pSetm(h);
  return h;
}

