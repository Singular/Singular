/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Id: matpol.cc,v 1.2 1997-03-24 14:25:20 Singular Exp $";
/*
* ABSTRACT:
*/

#include <stdio.h>
#include <limits.h>

#include "mod2.h"
#include "tok.h"
#include "polys.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "ideals.h"
#include "ipid.h"
#include "subexpr.h"
#include "matpol.h"

/*0 implementation*/

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
    if (rc->m==NULL)
    {
      Werror("internal error: creating matrix[%d][%d]",r,c);
      return NULL;
    }
  }
  return rc;
}

/*2
*copies matrix a to b
*/
matrix mpCopy (matrix a)
{
  int i,j;
  matrix b;
  int m=MATROWS(a);
  int n=MATCOLS(a);

  b = mpNew(m, n);
  for (i=1; i<=m; i++)
  {
    for (j=1; j<=n; j++)
    {
      poly t=MATELEM(a,i,j);
      pNormalize(t);
#ifdef MDEBUG
//      MATELEM(b,i,j) = pDBCopy(t,f,l) ;
      MATELEM(b,i,j) = pCopy(t) ;
#else
      MATELEM(b,i,j) = pCopy(t) ;
#endif
    }
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
  int i=min(r,c);

  pNormalize(p);
  while (i>1)
  {
    MATELEM(rc,i,i)=pCopy(p);
    i--;
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
  int    i, j, k = 0, n = a->rows(), m = a->cols();
  poly p = pISet(f);
  matrix c = mpNew(n,m);

  for (i=0; i<n; i++)
  {
    for (j=0; j<m; j++)
    {
      c->m[k] = pMult(pCopy(a->m[k]), pCopy(p));
      k++;
    }
  }
  pDelete(&p);
  return c;
}

/*2
* multiply a matrix 'a' by a poly 'p', destroy the args
*/
matrix mpMultP(matrix a, poly p)
{
  int    i, j, k = 0, n = a->rows(), m = a->cols();

  pNormalize(p);
  for (i=0; i<n; i++)
  {
    for (j=0; j<m; j++)
    {
      a->m[k] = pMult(a->m[k], pCopy(p));
      k++;
    }
  }
  pDelete(&p);
  return a;
}

matrix mpAdd(matrix a, matrix b)
{
  int    i, j, k = 0, n = a->rows(), m = a->cols();
  if ((n != b->rows()) || (m != b->cols()))
  {
/*
*    Werror("cannot add %dx%d matrix and %dx%d matrix",
*      m,n,b->cols(),b->rows());
*/
    return NULL;
  }
  matrix c = mpNew(n,m);
  for (i=0; i<n; i++)
  {
    for (j=0; j<m; j++)
    {
      c->m[k] = pAdd(pCopy(a->m[k]), pCopy(b->m[k]));
      k++;
    }
  }
  return c;
}

matrix mpSub(matrix a, matrix b)
{
  int    i, j, k = 0, n = a->rows(), m = a->cols();
  if ((n != b->rows()) || (m != b->cols()))
  {
/*
*    Werror("cannot sub %dx%d matrix and %dx%d matrix",
*      m,n,b->cols(),b->rows());
*/
    return NULL;
  }
  matrix c = mpNew(n,m);
  for (i=0; i<n; i++)
  {
    for (j=0; j<m; j++)
    {
      c->m[k] = pSub(pCopy(a->m[k]), pCopy(b->m[k]));
      k++;
    }
  }
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

/*2
* res:= a / b;  a destroyed, b NOT destroyed
*/
static poly mpDivide(poly a, poly b)
{
  int i, k;
  number x;
  poly res, tail, t, h, h0;

  res = a;
  tail = pNext(b);
  if (tail == NULL)
  {
    k = pTotaldegree(b);
    do
    {
      if (k!=0)
      {
        for (i=pVariables; i>0; i--)
          pGetExp(a,i) -= pGetExp(b,i);
        pSetm(a);
      }
      x = nDiv(pGetCoeff(a), pGetCoeff(b));
      nNormalize(x);
      pSetCoeff(a,x);
      pIter(a);
    } while (a != NULL);
    return res;
  }
  do
  {
    for (i=pVariables; i>0; i--)
      pGetExp(a,i) -= pGetExp(b,i);
    pSetm(a);
    x = nDiv(pGetCoeff(a), pGetCoeff(b));
    nNormalize(x);
    pSetCoeff(a,x);
    t = tail;
    h = NULL;
    do
    {
      if (h == NULL)
        h = h0 = pNew();
      else
        h = pNext(h) = pNew();
      for (i=pVariables; i>0; i--)
        pSetExp(h,i, pGetExp(a,i)+pGetExp(t,i));
      pSetm(h);
      x = nMult(pGetCoeff(a), pGetCoeff(t));
      pSetCoeff0(h,nNeg(x));
      pIter(t);
    } while (t != NULL);
    pNext(h) = NULL;
    pNext(a) = pAdd(pNext(a),h0);
    pIter(a);
  } while (a != NULL);
  return res;
}

/*2
* pivot strategy for Bareiss algorithm
* search at all positions (qr[i],qc[j]) with
*   1 <= j <= m, 1 <= i <= n and ((z[j] == 0)&&(f[i] == 0))
* looks in the polynomials for
*   1. minimal length
*   2. minimal degree
* (qr[k],qc[k]) will be the optimal position of a nonzero element
* if all element are zero -> *r = 0
*/
static void mpPivot(matrix a, int k, int *r, int *qr, int *qc)
{
  int i, ii, j, iopt, jopt, l, lopt, d, dopt;
  poly p;
  int n = MATCOLS(a), m = MATROWS(a);

  lopt = INT_MAX;
  for(i=k; i<=m; i++)
  {
    ii = qr[i];
    for(j=k; j<=n; j++)
    {
      p = MATELEM(a,ii,qc[j]);
      if (p != NULL)
      {
        l = pLength(p);
        if (l<lopt)
        {
          iopt = i;
          jopt = j;
          lopt = l;
          dopt = pTotaldegree(p);
        }
        else if (l == lopt)
        {
          d = pTotaldegree(p);
          if (d < dopt)
          {
            iopt = i;
            jopt = j;
            dopt = d;
          }
        }
      }
    }
  }
  if (lopt < INT_MAX)
  {
    ii = qr[k];
    qr[k] = qr[iopt];
    qr[iopt] = ii;
    ii = qc[k];
    qc[k] = qc[jopt];
    qc[jopt] = ii;
    *r = 1;
  }
  else
    *r = 0;
}

/*2
* Bareiss' algorithm is applied for backward elimination in mpBar
*/
static matrix mpBarBack(matrix res, int *qr, int *qc, int mn)
{
  int i, ii, i1, j, jj, j1, k;
  poly p1, p2, q1, q2, t, c;
  int n = MATCOLS(res);

  /* backward elimination, clean upper triangular nonzeros */
  c = NULL;
  for(k=mn; k>1; k--)
  {
    ii = qr[k];
    jj = qc[k];
    p1 = MATELEM(res,ii,jj);
    for (i=1; i<k; i++)
    {
      i1 = qr[i];
      p2 = pNeg(MATELEM(res,i1,jj));
      for (j=n; j>mn; j--)
      {
        j1 = qc[j];
        q1 = pMult(pCopy(p1),MATELEM(res,i1,j1));
        q2 = pMult(pCopy(MATELEM(res,ii,j1)),pCopy(p2));
        t = pAdd(q1, q2);
        if ((c != NULL) &&(t != NULL))
          t = mpDivide(t, c);
        MATELEM(res,i1,j1) = t;
      }
      for (j=1; j<k; j++)
      {
        j1 = qc[j];
        q1 = pMult(pCopy(p1),MATELEM(res,i1,j1));
        q2 = pMult(pCopy(MATELEM(res,ii,j1)),pCopy(p2));
        t = pAdd(q1, q2);
        if ((c != NULL) &&(t != NULL))
          t = mpDivide(t, c);
        MATELEM(res,i1,j1) = t;
      }
      pDelete(&(MATELEM(res,i1,jj)));
    }
    c = p1;
  }
  return res;
}

/*2
*Bareiss' algorithm is applied to the mxn matrix a
*/
static matrix mpBar(matrix a, int *qr, int *qc, int *ra, BOOLEAN sw)
{
  int i, ii, i1, j, jj, j1;
  poly p1, p2, q1, q2, t, c;
  int n = MATCOLS(a),
      m = MATROWS(a),
      mn = min(m, n), k = 2;
  matrix res = mpCopy(a);

  if (mn < 2)
    return res;
  /* elimination, upper triangular structure */
  mpPivot(res,1,&ii,qr,qc);
  if (ii==0)
  {
    *ra = 0;
    return res;
  }
  ii = qr[1];
  jj = qc[1];
  p1 = MATELEM(res,ii,jj);
  for(i=m; i>1; i--)
  {
    i1 = qr[i];
    p2 = pNeg(MATELEM(res,i1,jj));
    for (j=n; j>1; j--)
    {
      j1 = qc[j];
      q1 = pMult(pCopy(p1),MATELEM(res,i1,j1));
      q2 = pMult(pCopy(MATELEM(res,ii,j1)),pCopy(p2));
      MATELEM(res,i1,j1) = pAdd(q1, q2);
    }
    pDelete(&(MATELEM(res,i1,jj)));
  }
  c = p1;
  loop
  {
    mpPivot(res,k,&ii,qr,qc);
    if (ii==0)
    {
      k--;
      break;
    }
    ii = qr[k];
    jj = qc[k];
    p1 = MATELEM(res,ii,jj);
    for(i=m; i>k; i--)
    {
      i1 = qr[i];
      p2 = pNeg(MATELEM(res,i1,jj));
      for (j=n; j>k; j--)
      {
        j1 = qc[j];
        q1 = pMult(pCopy(p1),MATELEM(res,i1,j1));
        q2 = pMult(pCopy(MATELEM(res,ii,j1)),pCopy(p2));
        t = pAdd(q1, q2);
        if (t != NULL)
          t = mpDivide(t, c);
        MATELEM(res,i1,j1) = t;
      }
      pDelete(&(MATELEM(res,i1,jj)));
    }
    if (k == mn)
      break;
    c = p1;
    k++;
  }
  *ra = k;
  /* backward elimination, clean upper triangular nonzeros */
  if (sw)
  {
    mpBarBack(res, qr, qc, k);
  }
  return res;
}

/*2
*  caller of 'Bareiss' algorithm,
*  the position of the 'pivots' is as follows:
*  (qc[i], qr[i]) for i=1,..,r (r the rank of a)
*  the switch sw:
*    TRUE -> full elimination
*    FALSE -> only upper 'tridiagonal'
*/
matrix mpBareiss (matrix a, BOOLEAN sw)
{
  matrix res;
  int *qc, *qr, r;
  int n = MATCOLS(a), m = MATROWS(a);

  qr = (int *)Alloc((m+1)*sizeof(int));
  qc = (int *)Alloc((n+1)*sizeof(int));
  for (r=m; r>0; r--)
    qr[r] = r;
  for (r=n; r>0; r--)
    qc[r] = r;
  res = mpBar(a, qr, qc, &r, sw);
  Free((ADDRESS)qc, (n+1)*sizeof(int));
  Free((ADDRESS)qr, (m+1)*sizeof(int));
  return res;
}

typedef int perm[100];

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

/*2
* compute sign for determinat in Bareiss method
*/
/*
*static int mpSign(int *qc, int n)
*{
*  int i, j, s=1;
*
*  for (i=1; i<n; i++)
*  {
*    if (i != qc[i])
*    {
*      s = -s;
*      j = i;
*      loop
*      {
*        j++;
*        if (i == qc[j])
*       {
*          qc[j] = qc[i];
*          qc[i] = i;
*        }
*      }
*    }
*  }
*  return s;
*}
*/

/*2
* returns the determinant of the n X n matrix m;
* uses Bareiss method:
* if m is regular, then for the matrix a (the result of mpBar)
* the determinant equals up to sign the last 'pivot'
*/
/*
*poly mpDet (matrix m)
*{
*  int *qc, *qr, r, s;
*  int n = MATCOLS(m);
*  matrix b;
*  number e;
*  poly res;
*
*  if (n != MATROWS(m))
*  {
*    Werror("det of %d x %d matrix",n,MATCOLS(m));
*    return NULL;
*  }
*#ifdef SRING
*  if(pSRING)
*    return mpLeibnitz(m);
*#endif
*  qc = (int *)Alloc0((n+1)*sizeof(int));
*  qr = (int *)Alloc0((n+1)*sizeof(int));
*  b = mpBar(m, qc, qr, &r, FALSE);
*  if (r<n)
*    res = NULL;
*  else
*  {
*    res = MATELEM(b,qr[n],qc[n]);
*    MATELEM(b,qr[n],qc[n]) = NULL;
*  }
*  s = mpSign(qc, n);
*  if (s < 0)
*  {
*    e = nInit(s);
*    pMultN(res, e);
*    nDelete(&e);
*  }
*  Free((ADDRESS)qc, (n+1)*sizeof(int));
*  Free((ADDRESS)qr, (n+1)*sizeof(int));
*  idDelete((ideal *)&b);
*  return res;
*}
*/

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
  }
  for (i=k+1; i<=n; i++)
    MATELEM(s,1,i) = TraceOfProd(ma[i / 2], ma[(i+1) / 2], n);
  for (i=1; i<=k; i++)
    idDelete((ideal *)&(ma[i]));
/* the array s contains the traces of the powers of the matrix m,
*  these are the power sums of the eigenvalues of m */
  a = mpNew(1,n);
  MATELEM(a,1,1) = minuscopy(MATELEM(s,1,1));
  for (i=2; i<=n; i++)
  {
    p = pCopy(MATELEM(s,1,i));
    for (j=i-1; j>=1; j--)
    {
      q = pMult(pCopy(MATELEM(s,1,j)), pCopy(MATELEM(a,1,i-j)));
      p = pAdd(p,q);
    }
    // c= -1/i
    d = nInit(-(int)i);
    c = nDiv(ONE, d);
    nDelete(&d);

    pMultN(p, c);
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

  i = binom(a->rows(),ar);
  j = binom(a->cols(),ar);

  rowchoise=(int *)Alloc(ar*sizeof(int));
  colchoise=(int *)Alloc(ar*sizeof(int));
  result =mpNew(i,j);
  tmp=mpNew(ar,ar);
  l = 1; /* k,l:the index in result*/
  idInitChoise(ar,1,a->rows(),&rowch,rowchoise);
  while (!rowch)
  {
    k=1;
    idInitChoise(ar,1,a->cols(),&colch,colchoise);
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
      idGetNextChoise(ar,a->cols(),&colch,colchoise);
    }
    idGetNextChoise(ar,a->rows(),&rowch,rowchoise);
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
  for(k=1;k<=r; k++)
  {
    MATELEM(m,k,k*(p+1))=pOne();
  }
  for(l=1;l<=p; l++)
  {
    pSetExp(h,var,l);
    pSetm(h);
    for(k=1;k<=r; k++)
    {
      MATELEM(m,k,k*(p+1)-l)=pCopy(h);
    }
  }
  pDelete(&h);
}

/*2
* insert a monomial into a list, avoid duplicates
* arguments are destroyed
*/
static poly pInsert(poly p1, poly p2)
{
  static poly a1, p, a2, a;
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
static poly select (poly fro, monomial what)
{
  int i;
  poly h, res;
  res = NULL;
  while (fro!=NULL)
  {
    h = pOne();
    for (i=1; i<=pVariables; i++)
      pSetExp(h,i, pGetExp(fro,i) * what[i]);
    pGetComp(h)=pGetComp(fro);
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
static poly exdiv ( poly m, monomial d)
{
  int i;
  poly h = pHead(m);
  for (i=1; i<=pVariables; i++)
  {
    if (d[i] > 0)
    {
      if (d[i] != pGetExp(h,i))
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
  sel = select(f, vars->exp);
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
      h = exdiv(f, MATELEM(co,1,i)->exp);
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
  poly sel=select(v,mon->exp);

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
      h = exdiv(v, MATELEM(*m,j,i)->exp);
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
