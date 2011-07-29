/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT:
*/

#include <stdio.h>
#include <math.h>

#include "config.h"
#include <misc/auxiliary.h>

#include <omalloc/omalloc.h>
#include <misc/mylimits.h>


// #include <kernel/structs.h>
// #include <kernel/kstd1.h>
// #include <kernel/polys.h>

#include <misc/intvec.h>
#include <coeffs/numbers.h>

#include <reporter/reporter.h>


#include "monomials/ring.h"
#include "monomials/p_polys.h"

#include "coeffrings.h"
#include "simpleideals.h"
#include "matpol.h"
#include "prCopy.h"

#include "sparsmat.h"

//omBin sip_sideal_bin = omGetSpecBin(sizeof(ip_smatrix));
/*0 implementation*/

static poly mp_Exdiv ( poly m, poly d, poly vars, const ring);
static poly mp_Select (poly fro, poly what, const ring);

/// create a r x c zero-matrix
matrix mpNew(int r, int c)
{
  if (r<=0) r=1;
  if ( (((int)(INT_MAX/sizeof(poly))) / r) <= c)
  {
    Werror("internal error: creating matrix[%d][%d]",r,c);
    return NULL;
  }
  matrix rc = (matrix)omAllocBin(sip_sideal_bin);
  rc->nrows = r;
  rc->ncols = c;
  rc->rank = r;
  if (c != 0)
  {
    int s=r*c*sizeof(poly);
    rc->m = (poly*)omAlloc0(s);
    //if (rc->m==NULL)
    //{
    //  Werror("internal error: creating matrix[%d][%d]",r,c);
    //  return NULL;
    //}
  }
  return rc;
}

/// copies matrix a (from ring r to r)
matrix mp_Copy (matrix a, const ring r)
{
  id_Test((ideal)a, r);
  poly t;
  int i, m=MATROWS(a), n=MATCOLS(a);
  matrix b = mpNew(m, n);

  for (i=m*n-1; i>=0; i--)
  {
    t = a->m[i];
    if (t!=NULL)
    {
      p_Normalize(t, r);
      b->m[i] = p_Copy(t, r);
    }
  }
  b->rank=a->rank;
  return b;
}

/// copies matrix a from rSrc into rDst
matrix mp_Copy(const matrix a, const ring rSrc, const ring rDst)
{
  id_Test((ideal)a, rSrc);

  poly t;
  int i, m=MATROWS(a), n=MATCOLS(a);

  matrix b = mpNew(m, n);

  for (i=m*n-1; i>=0; i--)
  {
    t = a->m[i];
    if (t!=NULL)
    {
      b->m[i] = prCopyR_NoSort(t, rSrc, rDst);
      p_Normalize(b->m[i], rDst);
    }
  }
  b->rank=a->rank;

  id_Test((ideal)b, rDst);

  return b;
}



/// make it a p * unit matrix
matrix mp_InitP(int r, int c, poly p, const ring R)
{
  matrix rc = mpNew(r,c);
  int i=si_min(r,c), n = c*(i-1)+i-1, inc = c+1;

  p_Normalize(p, R);
  while (n>0)
  {
    rc->m[n] = p_Copy(p, R);
    n -= inc;
  }
  rc->m[0]=p;
  return rc;
}

/// make it a v * unit matrix
matrix mp_InitI(int r, int c, int v, const ring R)
{
  return mp_InitP(r, c, p_ISet(v, R), R);
}

/// c = f*a
matrix mp_MultI(matrix a, int f, const ring R)
{
  int k, n = a->nrows, m = a->ncols;
  poly p = p_ISet(f, R);
  matrix c = mpNew(n,m);

  for (k=m*n-1; k>0; k--)
    c->m[k] = pp_Mult_qq(a->m[k], p, R);
  c->m[0] = p_Mult_q(p_Copy(a->m[0], R), p, R);
  return c;
}

/// multiply a matrix 'a' by a poly 'p', destroy the args
matrix mp_MultP(matrix a, poly p, const ring R)
{
  int k, n = a->nrows, m = a->ncols;

  p_Normalize(p, R);
  for (k=m*n-1; k>0; k--)
  {
    if (a->m[k]!=NULL)
      a->m[k] = p_Mult_q(a->m[k], p_Copy(p, R), R);
  }
  a->m[0] = p_Mult_q(a->m[0], p, R);
  return a;
}

/*2
* multiply a poly 'p' by a matrix 'a', destroy the args
*/
matrix pMultMp(poly p, matrix a, const ring R)
{
  int k, n = a->nrows, m = a->ncols;

  p_Normalize(p, R);
  for (k=m*n-1; k>0; k--)
  {
    if (a->m[k]!=NULL)
      a->m[k] = p_Mult_q(p_Copy(p, R), a->m[k], R);
  }
  a->m[0] = p_Mult_q(p, a->m[0], R);
  return a;
}

matrix mp_Add(matrix a, matrix b, const ring R)
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
    c->m[k] = p_Add_q(p_Copy(a->m[k], R), p_Copy(b->m[k], R), R);
  return c;
}

matrix mp_Sub(matrix a, matrix b, const ring R)
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
    c->m[k] = p_Sub(p_Copy(a->m[k], R), p_Copy(b->m[k], R), R);
  return c;
}

matrix mp_Mult(matrix a, matrix b, const ring R)
{
  int i, j, k;
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
    for (k=1; k<=p; k++)
    {
      poly aik;
      if ((aik=MATELEM(a,i,k))!=NULL)
      {
        for (j=1; j<=q; j++)
        {
          poly bkj;
          if ((bkj=MATELEM(b,k,j))!=NULL)
          {
            poly *cij=&(MATELEM(c,i,j));
            poly s = pp_Mult_qq(aik /*MATELEM(a,i,k)*/, bkj/*MATELEM(b,k,j)*/, R);
            if (/*MATELEM(c,i,j)*/ (*cij)==NULL) (*cij)=s;
            else (*cij) = p_Add_q((*cij) /*MATELEM(c,i,j)*/ ,s, R);
          }
        }
      }
    //  pNormalize(t);
    //  MATELEM(c,i,j) = t;
    }
  }
  for(i=m*q-1;i>=0;i--) p_Normalize(c->m[i], R);
  return c;
}

matrix mp_Transp(matrix a, const ring R)
{
  int    i, j, r = MATROWS(a), c = MATCOLS(a);
  poly *p;
  matrix b =  mpNew(c,r);

  p = b->m;
  for (i=0; i<c; i++)
  {
    for (j=0; j<r; j++)
    {
      if (a->m[j*c+i]!=NULL) *p = p_Copy(a->m[j*c+i], R);
      p++;
    }
  }
  return b;
}

/*2
*returns the trace of matrix a
*/
poly mp_Trace ( matrix a, const ring R)
{
  int i;
  int n = (MATCOLS(a)<MATROWS(a)) ? MATCOLS(a) : MATROWS(a);
  poly  t = NULL;

  for (i=1; i<=n; i++)
    t = p_Add_q(t, p_Copy(MATELEM(a,i,i), R), R);
  return t;
}

/*2
*returns the trace of the product of a and b
*/
poly TraceOfProd ( matrix a, matrix b, int n, const ring R)
{
  int i, j;
  poly  p, t = NULL;

  for (i=1; i<=n; i++)
  {
    for (j=1; j<=n; j++)
    {
      p = pp_Mult_qq(MATELEM(a,i,j), MATELEM(b,j,i), R);
      t = p_Add_q(t, p, R);
    }
  }
  return t;
}

// #ifndef SIZE_OF_SYSTEM_PAGE
// #define SIZE_OF_SYSTEM_PAGE 4096
// #endif

/*2
* corresponds to Maple's coeffs:
* var has to be the number of a variable
*/
matrix mp_Coeffs (ideal I, int var, const ring R)
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
      l=p_GetExp(f,var, R);
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
    I->m[i]=NULL;
    while (f!=NULL)
    {
      l=p_GetExp(f,var, R);
      p_SetExp(f,var,0, R);
      c=si_max((int)p_GetComp(f, R),1);
      p_SetComp(f,0, R);
      p_Setm(f, R);
      /* now add the resulting monomial to co*/
      h=pNext(f);
      pNext(f)=NULL;
      //MATELEM(co,c*(m+1)-l,i+1)
      //  =p_Add_q(MATELEM(co,c*(m+1)-l,i+1),f, R);
      MATELEM(co,(c-1)*(m+1)+l+1,i+1)
        =p_Add_q(MATELEM(co,(c-1)*(m+1)+l+1,i+1),f, R);
      /* iterate f*/
      f=h;
    }
  }
  id_Delete(&I, R);
  return co;
}

/*2
* given the result c of mpCoeffs(ideal/module i, var)
* i of rank r
* build the matrix of the corresponding monomials in m
*/
void   mp_Monomials(matrix c, int r, int var, matrix m, const ring R)
{
  /* clear contents of m*/
  int k,l;
  for (k=MATROWS(m);k>0;k--)
  {
    for(l=MATCOLS(m);l>0;l--)
    {
      p_Delete(&MATELEM(m,k,l), R);
    }
  }
  omfreeSize((ADDRESS)m->m,MATROWS(m)*MATCOLS(m)*sizeof(poly));
  /* allocate monoms in the right size r x MATROWS(c)*/
  m->m=(poly*)omAlloc0(r*MATROWS(c)*sizeof(poly));
  MATROWS(m)=r;
  MATCOLS(m)=MATROWS(c);
  m->rank=r;
  /* the maximal power p of x_var: MATCOLS(m)=r*(p+1) */
  int p=MATCOLS(m)/r-1;
  /* fill in the powers of x_var=h*/
  poly h=p_One(R);
  for(k=r;k>0; k--)
  {
    MATELEM(m,k,k*(p+1))=p_One(R);
  }
  for(l=p;l>=0; l--)
  {
    p_SetExp(h,var,p-l, R);
    p_Setm(h, R);
    for(k=r;k>0; k--)
    {
      MATELEM(m,k,k*(p+1)-l)=p_Copy(h, R);
    }
  }
  p_Delete(&h, R);
}

matrix mp_CoeffProc (poly f, poly vars, const ring R)
{
  assume(vars!=NULL);
  poly sel, h;
  int l, i;
  int pos_of_1 = -1;
  matrix co;

  if (f==NULL)
  {
    co = mpNew(2, 1);
    MATELEM(co,1,1) = p_One(R);
    MATELEM(co,2,1) = NULL;
    return co;
  }
  sel = mp_Select(f, vars, R);
  l = pLength(sel);
  co = mpNew(2, l);

  if (rHasLocalOrMixedOrdering(R))
  {
    for (i=l; i>=1; i--)
    {
      h = sel;
      pIter(sel);
      pNext(h)=NULL;
      MATELEM(co,1,i) = h;
      MATELEM(co,2,i) = NULL;
      if (p_IsConstant(h, R)) pos_of_1 = i;
    }
  }
  else
  {
    for (i=1; i<=l; i++)
    {
      h = sel;
      pIter(sel);
      pNext(h)=NULL;
      MATELEM(co,1,i) = h;
      MATELEM(co,2,i) = NULL;
      if (p_IsConstant(h, R)) pos_of_1 = i;
    }
  }
  while (f!=NULL)
  {
    i = 1;
    loop
    {
      if (i!=pos_of_1)
      {
        h = mp_Exdiv(f, MATELEM(co,1,i),vars, R);
        if (h!=NULL)
        {
          MATELEM(co,2,i) = p_Add_q(MATELEM(co,2,i), h, R);
          break;
        }
      }
      if (i == l)
      {
        // check monom 1 last:
        if (pos_of_1 != -1)
        {
          h = mp_Exdiv(f, MATELEM(co,1,pos_of_1),vars, R);
          if (h!=NULL)
          {
            MATELEM(co,2,pos_of_1) = p_Add_q(MATELEM(co,2,pos_of_1), h, R);
          }
        }
        break;
      }
      i ++;
    }
    pIter(f);
  }
  return co;
}

/*2
*exact divisor: let d  == x^i*y^j, m is thought to have only one term;
*    return m/d iff d divides m, and no x^k*y^l (k>i or l>j) divides m
* consider all variables in vars
*/
static poly mp_Exdiv ( poly m, poly d, poly vars, const ring R)
{
  int i;
  poly h = p_Head(m, R);
  for (i=1; i<=rVar(R); i++)
  {
    if (p_GetExp(vars,i, R) > 0)
    {
      if (p_GetExp(d,i, R) != p_GetExp(h,i, R))
      {
        p_Delete(&h, R);
        return NULL;
      }
      p_SetExp(h,i,0, R);
    }
  }
  p_Setm(h, R);
  return h;
}

void mp_Coef2(poly v, poly mon, matrix *c, matrix *m, const ring R)
{
  poly* s;
  poly p;
  int sl,i,j;
  int l=0;
  poly sel=mp_Select(v,mon, R);

  p_Vec2Polys(sel,&s,&sl, R);
  for (i=0; i<sl; i++)
    l=si_max(l,pLength(s[i]));
  *c=mpNew(sl,l);
  *m=mpNew(sl,l);
  poly h;
  int isConst;
  for (j=1; j<=sl;j++)
  {
    p=s[j-1];
    if (p_IsConstant(p, R)) /*p != NULL */
    {
      isConst=-1;
      i=l;
    }
    else
    {
      isConst=1;
      i=1;
    }
    while(p!=NULL)
    {
      h = p_Head(p, R);
      MATELEM(*m,j,i) = h;
      i+=isConst;
      p = p->next;
    }
  }
  while (v!=NULL)
  {
    i = 1;
    j = p_GetComp(v, R);
    loop
    {
      poly mp=MATELEM(*m,j,i);
      if (mp!=NULL)
      {
        h = mp_Exdiv(v, mp /*MATELEM(*m,j,i)*/, mp, R);
        if (h!=NULL)
        {
          p_SetComp(h,0, R);
          MATELEM(*c,j,i) = p_Add_q(MATELEM(*c,j,i), h, R);
          break;
        }
      }
      if (i < l)
        i++;
      else
        break;
    }
    v = v->next;
  }
}


BOOLEAN mp_Equal(matrix a, matrix b, const ring R)
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
      if (b->m[i]==NULL) return FALSE;
      else if (p_Cmp(a->m[i],b->m[i], R)!=0) return FALSE;
    i--;
  }
  i=MATCOLS(a)*MATROWS(b)-1;
  while (i>=0)
  {
#if 0
    poly tt=p_Sub(p_Copy(a->m[i], R),p_Copy(b->m[i], R), R);
    if (tt!=NULL)
    {
      p_Delete(&tt, R);
      return FALSE;
    }
#else
    if(!p_EqualPolys(a->m[i],b->m[i], R)) return FALSE;
#endif
    i--;
  }
  return TRUE;
}

static poly minuscopy (poly p, const ring R)
{
  poly w;
  number  e;
  e = n_Init(-1, R);
  w = p_Copy(p, R);
  p_Mult_nn(w, e, R);
  n_Delete(&e, R);
  return w;
}

/*2
* insert a monomial into a list, avoid duplicates
* arguments are destroyed
*/
static poly p_Insert(poly p1, poly p2, const ring R)
{
  poly a1, p, a2, a;
  int c;

  if (p1==NULL) return p2;
  if (p2==NULL) return p1;
  a1 = p1;
  a2 = p2;
  a = p  = p_One(R);
  loop
  {
    c = p_Cmp(a1, a2, R);
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
      p_LmDelete(&a2, R);
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
  p_LmDelete(&p, R);
  return p;
}

/*2
*if what == xy the result is the list of all different power products
*    x^i*y^j (i, j >= 0) that appear in fro
*/
static poly mp_Select (poly fro, poly what, const ring R)
{
  int i;
  poly h, res;
  res = NULL;
  while (fro!=NULL)
  {
    h = p_One(R);
    for (i=1; i<=rVar(R); i++)
      p_SetExp(h,i, p_GetExp(fro,i, R) * p_GetExp(what, i, R), R);
    p_SetComp(h, p_GetComp(fro, R), R);
    p_Setm(h, R);
    res = p_Insert(h, res, R);
    fro = fro->next;
  }
  return res;
}

/*
*static void ppp(matrix a)
*{
*  int j,i,r=a->nrows,c=a->ncols;
*  for(j=1;j<=r;j++)
*  {
*    for(i=1;i<=c;i++)
*    {
*      if(MATELEM(a,j,i)!=NULL) Print("X");
*      else Print("0");
*    }
*    Print("\n");
*  }
*}
*/

static void mp_PartClean(matrix a, int lr, int lc, const ring R)
{
  poly *q1;
  int i,j;

  for (i=lr-1;i>=0;i--)
  {
    q1 = &(a->m)[i*a->ncols];
    for (j=lc-1;j>=0;j--) if(q1[j]) p_Delete(&q1[j], R);
  }
}

static void mp_FinalClean(matrix a, const ring R)
{
  omFreeSize((ADDRESS)a->m,a->nrows*a->ncols*sizeof(poly));
  omFreeBin((ADDRESS)a, sip_sideal_bin);
}

BOOLEAN mp_IsDiagUnit(matrix U, const ring R)
{
  if(MATROWS(U)!=MATCOLS(U))
    return FALSE;
  for(int i=MATCOLS(U);i>=1;i--)
  {
    for(int j=MATCOLS(U); j>=1; j--)
    {
      if (i==j)
      {
        if (!p_IsUnit(MATELEM(U,i,i), R)) return FALSE;
      }
      else if (MATELEM(U,i,j)!=NULL) return FALSE;
    }
  }
  return TRUE;
}

void iiWriteMatrix(matrix im, const char *n, int dim, const ring r, int spaces)
{
  int i,ii = MATROWS(im)-1;
  int j,jj = MATCOLS(im)-1;
  poly *pp = im->m;

  for (i=0; i<=ii; i++)
  {
    for (j=0; j<=jj; j++)
    {
      if (spaces>0)
        Print("%-*.*s",spaces,spaces," ");
      if (dim == 2) Print("%s[%u,%u]=",n,i+1,j+1);
      else if (dim == 1) Print("%s[%u]=",n,j+1);
      else if (dim == 0) Print("%s=",n);
      if ((i<ii)||(j<jj)) p_Write(*pp++, r);
      else                p_Write0(*pp, r);
    }
  }
}

char * iiStringMatrix(matrix im, int dim, const ring r, char ch)
{
  int i,ii = MATROWS(im);
  int j,jj = MATCOLS(im);
  poly *pp = im->m;
  char *s=StringSetS("");

  for (i=0; i<ii; i++)
  {
    for (j=0; j<jj; j++)
    {
      p_String0(*pp++, r);
      s=StringAppend("%c",ch);
      if (dim > 1) s = StringAppendS("\n");
    }
  }
  s[strlen(s)- (dim > 1 ? 2 : 1)]='\0';
  return s;
}

void   mp_Delete(matrix* a, const ring r)
{
  id_Delete((ideal *) a, r);
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

row_col_weight::row_col_weight(int i, int j)
{
  ym = i;
  yn = j;
  wrow = (float *)omAlloc(i*sizeof(float));
  wcol = (float *)omAlloc(j*sizeof(float));
}
row_col_weight::~row_col_weight()
{
  if (ym!=0)
  {
    omFreeSize((ADDRESS)wcol, yn*sizeof(float));
    omFreeSize((ADDRESS)wrow, ym*sizeof(float));
  }
}

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
  ring _R;
  void mpInitMat();
  poly * mpRowAdr(int r)
  { return &(Xarray[a_n*qrow[r]]); }
  poly * mpColAdr(int c)
  { return &(Xarray[qcol[c]]); }
  void mpRowWeight(float *);
  void mpColWeight(float *);
  void mpRowSwap(int, int);
  void mpColSwap(int, int);
  public:
  mp_permmatrix() : a_m(0) {}
  mp_permmatrix(matrix, ring);
  mp_permmatrix(mp_permmatrix *);
  ~mp_permmatrix();
  int mpGetRow();
  int mpGetCol();
  int mpGetRdim() { return s_m; }
  int mpGetCdim() { return s_n; }
  int mpGetSign() { return sign; }
  void mpSetSearch(int s);
  void mpSaveArray() { Xarray = NULL; }
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
mp_permmatrix::mp_permmatrix(matrix A, ring R) : sign(1)
{
  a_m = A->nrows;
  a_n = A->ncols;
  this->mpInitMat();
  Xarray = A->m;
  _R=R;
}

mp_permmatrix::mp_permmatrix(mp_permmatrix *M)
{
  poly p, *athis, *aM;
  int i, j;

  _R=M->_R;
  a_m = M->s_m;
  a_n = M->s_n;
  sign = M->sign;
  this->mpInitMat();
  Xarray = (poly *)omAlloc0(a_m*a_n*sizeof(poly));
  for (i=a_m-1; i>=0; i--)
  {
    athis = this->mpRowAdr(i);
    aM = M->mpRowAdr(i);
    for (j=a_n-1; j>=0; j--)
    {
      p = aM[M->qcol[j]];
      if (p)
      {
        athis[j] = p_Copy(p,_R);
      }
    }
  }
}

mp_permmatrix::~mp_permmatrix()
{
  int k;

  if (a_m != 0)
  {
    omFreeSize((ADDRESS)qrow,a_m*sizeof(int));
    omFreeSize((ADDRESS)qcol,a_n*sizeof(int));
    if (Xarray != NULL)
    {
      for (k=a_m*a_n-1; k>=0; k--)
        p_Delete(&Xarray[k],_R);
      omFreeSize((ADDRESS)Xarray,a_m*a_n*sizeof(poly));
    }
  }
}


static float mp_PolyWeight(poly p, const ring r);
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
        count += mp_PolyWeight(p,_R);
    }
    wcol[j] = count;
  }
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
        count += mp_PolyWeight(p,_R);
    }
    wrow[i] = count;
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
void mp_permmatrix::mpInitMat()
{
  int k;

  s_m = a_m;
  s_n = a_n;
  piv_s = 0;
  qrow = (int *)omAlloc(a_m*sizeof(int));
  qcol = (int *)omAlloc(a_n*sizeof(int));
  for (k=a_m-1; k>=0; k--) qrow[k] = k;
  for (k=a_n-1; k>=0; k--) qcol[k] = k;
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
        f1 = mp_PolyWeight(p,_R);
        if (f1 < fo)
        {
          fo = f1;
          if (iopt >= 0)
            p_Delete(&(this->mpRowAdr(iopt)[qcol[0]]),_R);
          iopt = i;
        }
        else
          p_Delete(&(this->mpRowAdr(i)[qcol[0]]),_R);
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
        lp = mp_PolyWeight(p,_R);
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
poly mp_permmatrix::mpGetElem(int r, int c)
{
  return Xarray[a_n*qrow[r]+qcol[c]];
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
      elim = p_Neg(elim,_R);
      for (j=s_n-1; j>=0; j--)
      {
        q2 = NULL;
        jj = qcol[j];
        if (ap[jj] != NULL)
        {
          q2 = SM_MULT(ap[jj], elim, div,_R);
          if (a[jj] != NULL)
          {
            q1 = SM_MULT(a[jj], piv, div,_R);
            p_Delete(&a[jj],_R);
            q2 = p_Add_q(q2, q1, _R);
          }
        }
        else if (a[jj] != NULL)
        {
          q2 = SM_MULT(a[jj], piv, div, _R);
        }
        if ((q2!=NULL) && div)
          SM_DIV(q2, div, _R);
        a[jj] = q2;
      }
      p_Delete(&a[qcol[s_n]], _R);
    }
    else
    {
      for (j=s_n-1; j>=0; j--)
      {
        jj = qcol[j];
        if (a[jj] != NULL)
        {
          q2 = SM_MULT(a[jj], piv, div, _R);
          p_Delete(&a[jj], _R);
          if (div)
            SM_DIV(q2, div, _R);
          a[jj] = q2;
        }
      }
    }
  }
}
/*
* weigth of a polynomial, for pivot strategy
*/
static float mp_PolyWeight(poly p, const ring r)
{
  int i;
  float res;

  if (pNext(p) == NULL)
  {
    res = (float)n_Size(pGetCoeff(p),r->cf);
    for (i=rVar(r);i>0;i--)
    {
      if(p_GetExp(p,i,r)!=0)
      {
        res += 2.0;
        break;
      }
    }
  }
  else
  {
    res = 0.0;
    do
    {
      res += (float)n_Size(pGetCoeff(p),r->cf)+2.0;
      pIter(p);
    }
    while (p);
  }
  return res;
}
/*
* find best row
*/
static int mp_PivBar(matrix a, int lr, int lc, const ring r)
{
  float f1, f2;
  poly *q1;
  int i,j,io;

  io = -1;
  f1 = 1.0e30;
  for (i=lr-1;i>=0;i--)
  {
    q1 = &(a->m)[i*a->ncols];
    f2 = 0.0;
    for (j=lc-1;j>=0;j--)
    {
      if (q1[j]!=NULL)
        f2 += mp_PolyWeight(q1[j],r);
    }
    if ((f2!=0.0) && (f2<f1))
    {
      f1 = f2;
      io = i;
    }
  }
  if (io<0) return 0;
  else return io+1;
}

static void mpSwapRow(matrix a, int pos, int lr, int lc)
{
  poly sw;
  int j;
  poly* a2 = a->m;
  poly* a1 = &a2[a->ncols*(pos-1)];

  a2 = &a2[a->ncols*(lr-1)];
  for (j=lc-1; j>=0; j--)
  {
    sw = a1[j];
    a1[j] = a2[j];
    a2[j] = sw;
  }
}

/*2
*  prepare one step of 'Bareiss' algorithm
*  for application in minor
*/
static int mp_PrepareRow (matrix a, int lr, int lc, const ring R)
{
  int r;

  r = mp_PivBar(a,lr,lc,R);
  if(r==0) return 0;
  if(r<lr) mpSwapRow(a, r, lr, lc);
  return 1;
}

/*
* find pivot in the last row
*/
static int mp_PivRow(matrix a, int lr, int lc, const ring r)
{
  float f1, f2;
  poly *q1;
  int j,jo;

  jo = -1;
  f1 = 1.0e30;
  q1 = &(a->m)[(lr-1)*a->ncols];
  for (j=lc-1;j>=0;j--)
  {
    if (q1[j]!=NULL)
    {
      f2 = mp_PolyWeight(q1[j],r);
      if (f2<f1)
      {
        f1 = f2;
        jo = j;
      }
    }
  }
  if (jo<0) return 0;
  else return jo+1;
}

static void mpSwapCol(matrix a, int pos, int lr, int lc)
{
  poly sw;
  int j;
  poly* a2 = a->m;
  poly* a1 = &a2[pos-1];

  a2 = &a2[lc-1];
  for (j=a->ncols*(lr-1); j>=0; j-=a->ncols)
  {
    sw = a1[j];
    a1[j] = a2[j];
    a2[j] = sw;
  }
}

/*2
*  prepare one step of 'Bareiss' algorithm
*  for application in minor
*/
static int mp_PreparePiv (matrix a, int lr, int lc,const ring r)
{
  int c;

  c = mp_PivRow(a, lr, lc,r);
  if(c==0) return 0;
  if(c<lc) mpSwapCol(a, c, lr, lc);
  return 1;
}

static inline BOOLEAN smSmaller(poly a, poly b)
{
  loop
  {
    pIter(b);
    if (b == NULL) return TRUE;
    pIter(a);
    if (a == NULL) return FALSE;
  }
}

static BOOLEAN sm_IsNegQuot(poly a, const poly b, const poly c, const ring R)
{
  if (p_LmDivisibleByNoComp(c, b, R))
  {
    p_ExpVectorDiff(a, b, c, R);
    // Hmm: here used to be a pSetm(a): but it is unnecessary,
    // if b and c are correct
    return FALSE;
  }
  else
  {
    int i;
    for (i=rVar(R); i>0; i--)
    {
      if(p_GetExp(c,i,R) > p_GetExp(b,i,R))
        p_SetExp(a,i,p_GetExp(c,i,R)-p_GetExp(b,i,R),R);
      else
        p_SetExp(a,i,0,R);
    }
    // here we actually might need a pSetm, if a is to be used in
    // comparisons
    return TRUE;
  }
}

static void mp_ElimBar(matrix a0, matrix re, poly div, int lr, int lc, const ring R)
{
  int r=lr-1, c=lc-1;
  poly *b = a0->m, *x = re->m;
  poly piv, elim, q1, q2, *ap, *a, *q;
  int i, j;

  ap = &b[r*a0->ncols];
  piv = ap[c];
  for(j=c-1; j>=0; j--)
    if (ap[j] != NULL) ap[j] = p_Neg(ap[j],R);
  for(i=r-1; i>=0; i--)
  {
    a = &b[i*a0->ncols];
    q = &x[i*re->ncols];
    if (a[c] != NULL)
    {
      elim = a[c];
      for (j=c-1; j>=0; j--)
      {
        q1 = NULL;
        if (a[j] != NULL)
        {
          q1 = sm_MultDiv(a[j], piv, div,R);
          if (ap[j] != NULL)
          {
            q2 = sm_MultDiv(ap[j], elim, div, R);
            q1 = p_Add_q(q1,q2,R);
          }
        }
        else if (ap[j] != NULL)
          q1 = sm_MultDiv(ap[j], elim, div, R);
        if (q1 != NULL)
        {
          if (div)
            sm_SpecialPolyDiv(q1, div,R);
          q[j] = q1;
        }
      }
    }
    else
    {
      for (j=c-1; j>=0; j--)
      {
        if (a[j] != NULL)
        {
          q1 = sm_MultDiv(a[j], piv, div, R);
          if (div)
            sm_SpecialPolyDiv(q1, div, R);
          q[j] = q1;
        }
      }
    }
  }
}

/*2
* entries of a are minors and go to result (only if not in R)
*/
void mp_MinorToResult(ideal result, int &elems, matrix a, int r, int c,
                     ideal R, const ring ri)
{
  poly *q1;
  int e=IDELEMS(result);
  int i,j;

  if (R != NULL)
  {
    for (i=r-1;i>=0;i--)
    {
      q1 = &(a->m)[i*a->ncols];
      //for (j=c-1;j>=0;j--)
      //{
      //  if (q1[j]!=NULL) q1[j] = kNF(R,currQuotient,q1[j]);
      //}
    }
  }
  for (i=r-1;i>=0;i--)
  {
    q1 = &(a->m)[i*a->ncols];
    for (j=c-1;j>=0;j--)
    {
      if (q1[j]!=NULL)
      {
        if (elems>=e)
        {
          pEnlargeSet(&(result->m),e,e);
          e += e;
          IDELEMS(result) =e;
        }
        result->m[elems] = q1[j];
        q1[j] = NULL;
        elems++;
      }
    }
  }
}

static void mpFinalClean(matrix a)
{
  omFreeSize((ADDRESS)a->m,a->nrows*a->ncols*sizeof(poly));
  omFreeBin((ADDRESS)a, sip_sideal_bin);
}

/*2
* produces recursively the ideal of all arxar-minors of a
*/
void mp_RecMin(int ar,ideal result,int &elems,matrix a,int lr,int lc,
              poly barDiv, ideal R, const ring r)
{
  int k;
  int kr=lr-1,kc=lc-1;
  matrix nextLevel=mpNew(kr,kc);

  loop
  {
/*--- look for an optimal row and bring it to last position ------------*/
    if(mp_PrepareRow(a,lr,lc,r)==0) break;
/*--- now take all pivots from the last row ------------*/
    k = lc;
    loop
    {
      if(mp_PreparePiv(a,lr,k,r)==0) break;
      mp_ElimBar(a,nextLevel,barDiv,lr,k,r);
      k--;
      if (ar>1)
      {
        mp_RecMin(ar-1,result,elems,nextLevel,kr,k,a->m[kr*a->ncols+k],R,r);
        mp_PartClean(nextLevel,kr,k, r);
      }
      else mp_MinorToResult(result,elems,nextLevel,kr,k,R,r);
      if (ar>k-1) break;
    }
    if (ar>=kr) break;
/*--- now we have to take out the last row...------------*/
    lr = kr;
    kr--;
  }
  mpFinalClean(nextLevel);
}

/*2
*returns the determinant of the matrix m;
*uses Bareiss algorithm
*/
poly mp_DetBareiss (matrix a, const ring r)
{
  int s;
  poly div, res;
  if (MATROWS(a) != MATCOLS(a))
  {
    Werror("det of %d x %d matrix",MATROWS(a),MATCOLS(a));
    return NULL;
  }
  matrix c = mp_Copy(a,r);
  mp_permmatrix *Bareiss = new mp_permmatrix(c,r);
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
  id_Delete((ideal *)&c,r);
  if (s < 0)
    res = p_Neg(res,r);
  return res;
}
