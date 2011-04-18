typedef int perm[100];
static void mpReplace(int j, int n, int &sign, int *perm);
static int mpNextperm(perm * z, int max);
static poly mp_Leibnitz(matrix a, const ring);
static poly minuscopy (poly p, const ring);
static poly p_Insert(poly p1, poly p2, const ring);

static void mp_PartClean(matrix, int, int, const ring);
static void mp_FinalClean(matrix, const ring);
static int mp_PrepareRow (matrix, int, int, const ring);
static int mp_PreparePiv (matrix, int, int, const ring);
static int mp_PivBar(matrix, int, int, const ring);
static int mp_PivRow(matrix, int, int, const ring);
static float mp_PolyWeight(poly, const ring);
static void mp_SwapRow(matrix, int, int, int, const ring);
static void mp_SwapCol(matrix, int, int, int, const ring);
static void mp_ElimBar(matrix, matrix, poly, int, int, const ring);

/*2
*  prepare one step of 'Bareiss' algorithm
*  for application in minor
*/
static int mp_PrepareRow (matrix a, int lr, int lc, const ring R)
{
  int r;

  r = mp_PivBar(a,lr,lc, R);
  if(r==0) return 0;
  if(r<lr) mp_SwapRow(a, r, lr, lc, R);
  return 1;
}

/*2
*  prepare one step of 'Bareiss' algorithm
*  for application in minor
*/
static int mp_PreparePiv (matrix a, int lr, int lc, const ring R)
{
  int c;

  c = mp_PivRow(a, lr, lc, R);
  if(c==0) return 0;
  if(c<lc) mp_SwapCol(a, c, lr, lc, R);
  return 1;
}

/*
* find best row
*/
static int mp_PivBar(matrix a, int lr, int lc, const ring R)
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
        f2 += mp_PolyWeight(q1[j], R);
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

/*
* find pivot in the last row
*/
static int mp_PivRow(matrix a, int lr, int lc, const ring R)
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
      f2 = mp_PolyWeight(q1[j], R);
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

/*
* weigth of a polynomial, for pivot strategy
*/
static float mp_PolyWeight(poly p, const ring R)
{
  int i;
  float res;

  if (pNext(p) == NULL)
  {
    res = (float)n_Size(p_GetCoeff(p, R), R);
    for (i=rVar(R);i>0;i--)
    {
      if(p_GetExp(p,i, R)!=0)
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
      res += (float)n_Size(p_GetCoeff(p, R), R) + 2.0;
      pIter(p);
    }
    while (p);
  }
  return res;
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


/*
/// entries of a are minors and go to result (only if not in R)
void mp_MinorToResult(ideal result, int &elems, matrix a, int r, int c,
                     ideal R, const ring R)
{
  poly *q1;
  int e=IDELEMS(result);
  int i,j;

  if (R != NULL)
  {
    for (i=r-1;i>=0;i--)
    {
      q1 = &(a->m)[i*a->ncols];
      for (j=c-1;j>=0;j--)
      {
        if (q1[j]!=NULL) q1[j] = kNF(R,currQuotient,q1[j]);
      }
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
          if(e<SIZE_OF_SYSTEM_PAGE)
          {
            pEnlargeSet(&(result->m),e,e);
            e += e;
          }
          else
          {
            pEnlargeSet(&(result->m),e,SIZE_OF_SYSTEM_PAGE);
            e += SIZE_OF_SYSTEM_PAGE;
          }
          IDELEMS(result) =e;
        }
        result->m[elems] = q1[j];
        q1[j] = NULL;
        elems++;
      }
    }
  }
}

/// produces recursively the ideal of all arxar-minors of a
void mp_RecMin(int ar,ideal result,int &elems,matrix a,int lr,int lc,
              poly barDiv, ideal R, const ring R)
{
  int k;
  int kr=lr-1,kc=lc-1;
  matrix nextLevel=mpNew(kr,kc);

  loop
  {
// --- look for an optimal row and bring it to last position ------------
    if(mpPrepareRow(a,lr,lc)==0) break;
// --- now take all pivots from the last row ------------
    k = lc;
    loop
    {
      if(mpPreparePiv(a,lr,k)==0) break;
      mpElimBar(a,nextLevel,barDiv,lr,k);
      k--;
      if (ar>1)
      {
        mpRecMin(ar-1,result,elems,nextLevel,kr,k,a->m[kr*a->ncols+k],R);
        mpPartClean(nextLevel,kr,k);
      }
      else mpMinorToResult(result,elems,nextLevel,kr,k,R);
      if (ar>k-1) break;
    }
    if (ar>=kr) break;
// --- now we have to take out the last row...------------
    lr = kr;
    kr--;
  }
  mpFinalClean(nextLevel);
}
*/



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
    ring      R;

    void mpInitMat();
    poly * mpRowAdr(int);
    poly * mpColAdr(int);
    void mpRowWeight(float *);
    void mpColWeight(float *);
    void mpRowSwap(int, int);
    void mpColSwap(int, int);
  public:
    mp_permmatrix() : a_m(0), R(NULL) {}
    mp_permmatrix(matrix, const ring);
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
*returns the determinant of the matrix m;
*uses Bareiss algorithm
*/
poly mp_DetBareiss (matrix a, const ring R)
{
  int s;
  poly div, res;
  if (MATROWS(a) != MATCOLS(a))
  {
    Werror("det of %d x %d matrix",MATROWS(a),MATCOLS(a));
    return NULL;
  }
  matrix c = mp_Copy(a, R);
  mp_permmatrix *Bareiss = new mp_permmatrix(c, R);
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
  id_Delete((ideal *)&c, R);
  if (s < 0)
    res = p_Neg(res, R);
  return res;
}



/*2
*returns the determinant of the matrix m;
*uses Newtons formulea for symmetric functions
*/
poly mp_Det (matrix m, const ring R)
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
  k=rChar(R);
  if ((k > 0) && (k <= n))
    return mp_Leibnitz(m, R);
  ONE = n_Init(1, R);
  ma[1]=mp_Copy(m, R);
  k = (n+1) / 2;
  s = mpNew(1, n);
  MATELEM(s,1,1) = mp_Trace(m, R);
  for (i=2; i<=k; i++)
  {
    //ma[i] = mpNew(n,n);
    ma[i]=mp_Mult(ma[i-1], ma[1], R);
    MATELEM(s,1,i) = mp_Trace(ma[i], R);
    p_Test(MATELEM(s,1,i), R);
  }
  for (i=k+1; i<=n; i++)
  {
    MATELEM(s,1,i) = TraceOfProd(ma[i / 2], ma[(i+1) / 2], n, R);
    p_Test(MATELEM(s,1,i), R);
  }
  for (i=1; i<=k; i++)
    id_Delete((ideal *)&(ma[i]), R);
/* the array s contains the traces of the powers of the matrix m,
*  these are the power sums of the eigenvalues of m */
  a = mpNew(1,n);
  MATELEM(a,1,1) = minuscopy(MATELEM(s,1,1), R);
  for (i=2; i<=n; i++)
  {
    p = p_Copy(MATELEM(s,1,i), R);
    for (j=i-1; j>=1; j--)
    {
      q = pp_Mult_qq(MATELEM(s,1,j), MATELEM(a,1,i-j), R);
      p_Test(q, R);
      p = p_Add_q(p,q, R);
    }
    // c= -1/i
    d = n_Init(-(int)i, R);
    c = n_Div(ONE, d, R);
    n_Delete(&d, R);

    p_Mult_nn(p, c, R);
    p_Test(p, R);
    MATELEM(a,1,i) = p;
    n_Delete(&c, R);
  }
/* the array a contains the elementary symmetric functions of the
*  eigenvalues of m */
  for (i=1; i<=n-1; i++)
  {
    //p_Delete(&(MATELEM(a,1,i)), R);
    p_Delete(&(MATELEM(s,1,i)), R);
  }
  p_Delete(&(MATELEM(s,1,n)), R);
/* up to a sign, the determinant is the n-th elementary symmetric function */
  if ((n/2)*2 < n)
  {
    d = n_Init(-1, R);
    p_Mult_nn(MATELEM(a,1,n), d, R);
    n_Delete(&d, R);
  }
  n_Delete(&ONE, R);
  id_Delete((ideal *)&s, R);
  poly result=MATELEM(a,1,n);
  MATELEM(a,1,n)=NULL;
  id_Delete((ideal *)&a, R);
  return result;
}

/*2
* compute all ar-minors of the matrix a
*/
matrix mp_Wedge(matrix a, int ar, const ring R)
{
  int     i,j,k,l;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  matrix result;
  matrix tmp;
  poly p;

  i = binom(a->nrows,ar);
  j = binom(a->ncols,ar);

  rowchoise=(int *)omAlloc(ar*sizeof(int));
  colchoise=(int *)omAlloc(ar*sizeof(int));
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
      p = mp_DetBareiss(tmp, R);
      if ((k+l) & 1) p=p_Neg(p, R);
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
  id_Delete((ideal *) &tmp, R);
  return (result);
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
//      p_Delete(&(MATELEM(a,i,j)), ?);
//      MATELEM(a,i,j)=p;
//    }
//  }
//  return a;
//}



/* --------------- internal stuff ------------------- */

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

mp_permmatrix::mp_permmatrix(matrix A, const ring r) : sign(1),  R(r)
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
  R = M->R;

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
        athis[j] = p_Copy(p, R);
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
        p_Delete(&Xarray[k], R);
      omFreeSize((ADDRESS)Xarray,a_m*a_n*sizeof(poly));
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
  p_Delete(&Xarray[a_n*qrow[r]+qcol[c]], R);
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
      elim = p_Neg(elim, R);
      for (j=s_n-1; j>=0; j--)
      {
        q2 = NULL;
        jj = qcol[j];
        if (ap[jj] != NULL)
        {
          q2 = SM_MULT(ap[jj], elim, div, R);
          if (a[jj] != NULL)
          {
            q1 = SM_MULT(a[jj], piv, div, R);
            p_Delete(&a[jj], R);
            q2 = p_Add_q(q2, q1, R);
          }
        }
        else if (a[jj] != NULL)
        {
          q2 = SM_MULT(a[jj], piv, div, R);
        }
        if ((q2!=NULL) && div)
          SM_DIV(q2, div, R);
        a[jj] = q2;
      }
      p_Delete(&a[qcol[s_n]], R);
    }
    else
    {
      for (j=s_n-1; j>=0; j--)
      {
        jj = qcol[j];
        if (a[jj] != NULL)
        {
          q2 = SM_MULT(a[jj], piv, div, R);
          p_Delete(&a[jj], R);
          if (div)
            SM_DIV(q2, div, R);
          a[jj] = q2;
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
        f1 = mp_PolyWeight(p, R);
        if (f1 < fo)
        {
          fo = f1;
          if (iopt >= 0)
            p_Delete(&(this->mpRowAdr(iopt)[qcol[0]]), R);
          iopt = i;
        }
        else
          p_Delete(&(this->mpRowAdr(i)[qcol[0]]), R);
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
        lp = mp_PolyWeight(p, R);
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
      f1 = mp_PolyWeight(p, R);
      if (f1 < fo)
      {
        fo = f1;
        if (iopt >= 0)
          p_Delete(&(this->mpRowAdr(iopt)[qcol[0]]), R);
        iopt = row;
      }
      else
        p_Delete(&(this->mpRowAdr(row)[qcol[0]]), R);
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
      lp = mp_PolyWeight(p, R);
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
  qrow = (int *)omAlloc(a_m*sizeof(int));
  qcol = (int *)omAlloc(a_n*sizeof(int));
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
        count += mp_PolyWeight(p, R);
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
        count += mp_PolyWeight(p, R);
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

/// perform replacement for pivot strategy in Bareiss algorithm
/// change sign of determinant
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

static int mpNextperm(perm * z, int max)
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

static poly mp_Leibnitz(matrix a, const ring R)
{
  int i, e, n;
  poly p, d;
  perm z;

  n = MATROWS(a);
  memset(&z,0,(n+2)*sizeof(int));
  p = p_One(R);
  for (i=1; i <= n; i++)
    p = p_Mult_q(p, p_Copy(MATELEM(a, i, i), R), R);
  d = p;
  for (i=1; i<= n; i++)
    z[i] = i;
  z[0]=1;
  e = 1;
  if (n!=1)
  {
    while (e)
    {
      e = mpNextperm((perm *)&z, n);
      p = p_One(R);
      for (i = 1; i <= n; i++)
        p = p_Mult_q(p, p_Copy(MATELEM(a, i, z[i]), R), R);
      if (z[0] > 0)
        d = p_Add_q(d, p, R);
      else
        d = p_Sub(d, p, R);
    }
  }
  return d;
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
    if (ap[j] != NULL) ap[j] = p_Neg(ap[j], R);
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
          q1 = SM_MULT(a[j], piv, div, R);
          if (ap[j] != NULL)
          {
            q2 = SM_MULT(ap[j], elim, div, R);
            q1 = p_Add_q(q1,q2, R);
          }
        }
        else if (ap[j] != NULL)
          q1 = SM_MULT(ap[j], elim, div, R);
        if (q1 != NULL)
        {
          if (div)
            SM_DIV(q1, div, R);
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
          q1 = SM_MULT(a[j], piv, div, R);
          if (div)
            SM_DIV(q1, div, R);
          q[j] = q1;
        }
      }
    }
  }
}

