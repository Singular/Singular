/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#ifndef INTVEC_CC
#define INTVEC_CC

#include "misc/auxiliary.h"

#include "misc/intvec.h"
#include "misc/options.h"
#include "omalloc/omalloc.h"

#pragma GCC push_options
#pragma GCC optimize ("wrapv")

/*0 implementation*/

intvec::intvec(int s, int e)
{
  int inc;
  col = 1;
  if (s<e)
  {
    row =  e-s+1;
    inc =  1;
  }
  else
  {
    row = s-e+1;
    inc = -1;
  }
  v = (int *)omAlloc(sizeof(int)*row);
  for (int i=0; i<row; i++)
  {
    v[i] = s;
    s+=inc;
  }
}

intvec::intvec(int r, int c, int init)
{
  row = r;
  col = c;
  int l = r*c;
  if (l>0) /*(r>0) && (c>0) */
    v = (int *)omAlloc(sizeof(int)*l);
  else
    v = NULL;
  for (int i=0; i<l; i++)
  {
    v[i] = init;
  }
}

char * intvec::ivString(int not_mat,int spaces, int dim) const
{
  //Print("ivString:this=%x,v=%x,row=%d\n",this,v,row);
#ifndef OM_NDEBUG
  omCheckAddr((void *)this);
  if (v!=NULL) omCheckAddr((void *)v);
#endif
  StringSetS("");
  if ((col == 1)&&(not_mat))
  {
    int i=0;
    for (; i<row-1; i++)
    {
      StringAppend("%d,",v[i]);
    }
    if (i<row)
    {
      StringAppend("%d",v[i]);
    }
  }
  else
  {
    for (int j=0; j<row; j++)
    {
      if (j<row-1)
      {
        for (int i=0; i<col; i++)
        {
          StringAppend("%d%c",v[j*col+i],',');
        }
      }
      else
      {
        for (int i=0; i<col; i++)
        {
          StringAppend("%d%c",v[j*col+i],i<col-1 ? ',' : ' ');
        }
      }
      if (j+1<row)
      {
        if (dim > 1) StringAppendS("\n");
        if (spaces>0) StringAppend("%-*.*s",spaces,spaces," ");
      }
    }
  }
  return StringEndS();
}

void intvec::resize(int new_length)
{
  assume(new_length >= 0 && col == 1);
  if (new_length==0)
  {
    if (v!=NULL)
    {
      omFreeSize(v, row*sizeof(int));
      v=NULL;
    }
  }
  else
  {
    if (v!=NULL)
      v = (int*) omRealloc0Size(v, row*sizeof(int), new_length*sizeof(int));
    else
      v = (int*) omAlloc0(new_length*sizeof(int));
  }
  row = new_length;
}

char * intvec::String(int dim) const
{
  return ivString(1, 0, dim);
}

#ifndef SING_NDEBUG
// debug only
void intvec::view () const
{
  Print ("intvec: {rows: %d, cols: %d, length: %d, Values: \n", rows(), cols(), length());

  for (int i = 0; i < rows(); i++)
  {
    Print ("Row[%3d]:", i);
    for (int j = 0; j < cols(); j++)
      Print (" %5d", this->operator[]((i)*cols()+j) );
    PrintLn ();
  }
  PrintS ("}\n");
}
#endif

void intvec::show(int notmat,int spaces) const
{
  char *s=ivString(notmat,spaces);
  if (spaces>0)
  {
    PrintNSpaces(spaces);
    PrintS(s);
  }
  else
  {
    PrintS(s);
  }
  omFree(s);
}

void intvec::operator+=(int intop)
{
  for (int i=0; i<row*col; i++) { v[i] += intop; }
}

void intvec::operator-=(int intop)
{
  for (int i=0; i<row*col; i++) { v[i] -= intop; }
}

void intvec::operator*=(int intop)
{
  for (int i=0; i<row*col; i++) { v[i] *= intop; }
}

void intvec::operator/=(int intop)
{
  if (intop == 0) return;
  int bb=ABS(intop);
  for (int i=0; i<row*col; i++)
  {
    int r=v[i];
    int c=r%bb;
    if (c<0) c+=bb;
    r=(r-c)/intop;
    v[i]=r;
  }
}

void intvec::operator%=(int intop)
{
  if (intop == 0) return;
  int bb=ABS(intop);
  for (int i=0; i<row*col; i++)
  {
    int r=v[i];
    int c=r%bb;
    if (c<0) c+=bb;
    v[i]=c;
  }
}

int intvec::compare(const intvec* op) const
{
  if ((col!=1) ||(op->cols()!=1))
  {
    if((col!=op->cols())
    || (row!=op->rows()))
      return -2;
  }
  int i;
  for (i=0; i<si_min(length(),op->length()); i++)
  {
    if (v[i] > (*op)[i])
      return 1;
    if (v[i] < (*op)[i])
      return -1;
  }
  // this can only happen for intvec: (i.e. col==1)
  for (; i<row; i++)
  {
    if (v[i] > 0)
      return 1;
    if (v[i] < 0)
      return -1;
  }
  for (; i<op->rows(); i++)
  {
    if (0 > (*op)[i])
      return 1;
    if (0 < (*op)[i])
      return -1;
  }
  return 0;
}
int intvec::compare(int o) const
{
  for (int i=0; i<row*col; i++)
  {
    if (v[i] <o) return -1;
    if (v[i] >o) return 1;
  }
  return 0;
}

intvec * ivAdd(intvec * a, intvec * b)
{
  intvec * iv;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    iv = new intvec(ma);
    for (i=0; i<mn; i++) (*iv)[i] = (*a)[i] + (*b)[i];
    if (ma > mn)
    {
      if (ma == a->rows())
      {
        for(i=mn; i<ma; i++) (*iv)[i] = (*a)[i];
      }
      else
      {
        for(i=mn; i<ma; i++) (*iv)[i] = (*b)[i];
      }
    }
    return iv;
  }
  if (mn != ma) return NULL;
  iv = new intvec(a);
  for (i=0; i<mn*a->cols(); i++) { (*iv)[i] += (*b)[i]; }
  return iv;
}

intvec * ivAddShift(intvec * a, intvec * b, int s)
{
  if (a->cols() != b->cols()) return NULL;
  if (a->cols() == 1)
  {
    int i;
    intvec * iv;
    int ma = si_max(a->rows(),b->rows()+s);
    iv=new intvec(ma);
    for (i=a->rows()-1;i>=0;i--)
      (*iv)[i] = (*a)[i];
    for(i=b->rows()+s-1;i>=s;i--)
      (*iv)[i] += (*b)[i-s];
    return iv;
  }
  return NULL;
}

intvec * ivSub(intvec * a, intvec * b)
{
  intvec * iv;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    iv = new intvec(ma);
    for (i=0; i<mn; i++) (*iv)[i] = (*a)[i] - (*b)[i];
    if (ma > mn)
    {
      if (ma == a->rows())
      {
        for(i=mn; i<ma; i++) (*iv)[i] = (*a)[i];
      }
      else
      {
        for(i=mn; i<ma; i++) (*iv)[i] = -(*b)[i];
      }
    }
    return iv;
  }
  if (mn != ma) return NULL;
  iv = new intvec(a);
  for (i=0; i<mn*a->cols(); i++) { (*iv)[i] -= (*b)[i]; }
  return iv;
}

intvec * ivTranp(intvec * o)
{
  int i, j, r = o->rows(), c = o->cols();
  intvec * iv= new intvec(c, r, 0);
  for (i=0; i<r; i++)
  {
    for (j=0; j<c; j++)
      (*iv)[j*r+i] = (*o)[i*c+j];
  }
  return iv;
}

int ivTrace(intvec * o)
{
  int i, s = 0, m = si_min(o->rows(),o->cols()), c = o->cols();
  for (i=0; i<m; i++)
  {
    s += (*o)[i*c+i];
  }
  return s;
}

intvec * ivMult(intvec * a, intvec * b)
{
  int i, j, k, sum,
      ra = a->rows(), ca = a->cols(),
      rb = b->rows(), cb = b->cols();
  intvec * iv;
  if (ca != rb) return NULL;
  iv = new intvec(ra, cb, 0);
  for (i=0; i<ra; i++)
  {
    for (j=0; j<cb; j++)
    {
      sum = 0;
      for (k=0; k<ca; k++)
        sum += (*a)[i*ca+k]*(*b)[k*cb+j];
      (*iv)[i*cb+j] = sum;
    }
  }
  return iv;
}

/*2
*computes a triangular matrix
*/
//void ivTriangMat(intvec * imat)
//{
//  int i=0,j=imat->rows(),k=j*imat->cols()-1;
//
//  ivTriangIntern(imat,i,j);
//  i *= imat->cols();
//  for(j=k;j>=i;j--)
//    (*imat)[j] = 0;
//}

/* def. internals */
static int ivColPivot(intvec *, int, int, int, int);
static void ivNegRow(intvec *, int);
static void ivSaveRow(intvec *, int);
static void ivSetRow(intvec *, int, int);
static void ivFreeRow(intvec *, int, int);
static void ivReduce(intvec *, int, int, int, int);
static void ivZeroElim(intvec *,int, int, int &);
static void ivRowContent(intvec *, int, int);
static void ivKernFromRow(intvec *, intvec *, intvec *,
                          int, int, int);
static intvec * ivOptimizeKern(intvec *);
static int ivGcd(int, int);
static void ivOptRecursive(intvec *, intvec *, intvec *,
                          int &, int &, int);
static void ivOptSolve(intvec *, intvec *, int &, int &);
static void ivContent(intvec *);
static int ivL1Norm(intvec *);
static int ivCondNumber(intvec *, int);

/* Triangulierung in intmat.cc */
void ivTriangIntern(intvec *imat, int &ready, int &all)
{
  int rpiv, colpos=0, rowpos=0;
  int ia=ready, ie=all;

  do
  {
    rowpos++;
    do
    {
      colpos++;
      rpiv = ivColPivot(imat, colpos, rowpos, ia, ie);
    } while (rpiv==0);
    if (rpiv>ia)
    {
      if (rowpos!=rpiv)
      {
        ivSaveRow(imat, rpiv);
        ivFreeRow(imat, rowpos, rpiv);
        ivSetRow(imat, rowpos, colpos);
        rpiv = rowpos;
      }
      ia++;
      if (ia==imat->cols())
      {
        ready = ia;
        all = ie;
        return;
      }
    }
    ivReduce(imat, rpiv, colpos, ia, ie);
    ivZeroElim(imat, colpos, ia, ie);
  } while (ie>ia);
  ready = ia;
  all = ie;
}

/* Kernberechnung in intmat.cc */
intvec * ivSolveKern(intvec *imat, int dimtr)
{
  int d=imat->cols();
  int kdim=d-dimtr;
  intvec *perm = new intvec(dimtr+1);
  intvec *kern = new intvec(kdim,d,0);
  intvec *res;
  int c, cp, r, t;

  t = kdim;
  c = 1;
  for (r=1;r<=dimtr;r++)
  {
    while (IMATELEM(*imat,r,c)==0) c++;
    (*perm)[r] = c;
    c++;
  }
  c = d;
  for (r=dimtr;r>0;r--)
  {
    cp = (*perm)[r];
    if (cp!=c)
    {
      ivKernFromRow(kern, imat, perm, t, r, c);
      t -= (c-cp);
      if (t==0)
        break;
      c = cp-1;
    }
    else
      c--;
  }
  if (kdim>1)
    res = ivOptimizeKern(kern);
  else
    res = ivTranp(kern);
  delete kern;
  delete perm;
  return res;
}

/* internals */
static int ivColPivot(intvec *imat, int colpos, int rowpos, int ready, int all)
{
  int rpiv;

  if (IMATELEM(*imat,rowpos,colpos)!=0)
    return rowpos;
  for (rpiv=ready+1;rpiv<=all;rpiv++)
  {
    if (IMATELEM(*imat,rpiv,colpos)!=0)
      return rpiv;
  }
  return 0;
}

static void ivNegRow(intvec *imat, int rpiv)
{
  int i;
  for (i=imat->cols();i!=0;i--)
    IMATELEM(*imat,rpiv,i) = -(IMATELEM(*imat,rpiv,i));
}

static void ivSaveRow(intvec *imat, int rpiv)
{
  int i, j=imat->rows();

  for (i=imat->cols();i!=0;i--)
    IMATELEM(*imat,j,i) = IMATELEM(*imat,rpiv,i);
}

static void ivSetRow(intvec *imat, int rowpos, int colpos)
{
  int i, j=imat->rows();

  for (i=imat->cols();i!=0;i--)
    IMATELEM(*imat,rowpos,i) = IMATELEM(*imat,j,i);
  ivRowContent(imat, rowpos, colpos);
}

static void ivFreeRow(intvec *imat, int rowpos, int rpiv)
{
  int i, j;

  for (j=rpiv-1;j>=rowpos;j--)
  {
    for (i=imat->cols();i!=0;i--)
      IMATELEM(*imat,j+1,i) = IMATELEM(*imat,j,i);
  }
}

static void ivReduce(intvec *imat, int rpiv, int colpos,
                     int ready, int all)
{
  int tgcd, ce, m1, m2, j, i;
  int piv = IMATELEM(*imat,rpiv,colpos);

  for (j=all;j>ready;j--)
  {
    ivRowContent(imat, j, 1);
    ce = IMATELEM(*imat,j,colpos);
    if (ce!=0)
    {
      IMATELEM(*imat,j,colpos) = 0;
      m1 = piv;
      m2 = ce;
      tgcd = ivGcd(m1, m2);
      if (tgcd != 1)
      {
        m1 /= tgcd;
        m2 /= tgcd;
      }
      for (i=imat->cols();i>colpos;i--)
      {
        IMATELEM(*imat,j,i) = IMATELEM(*imat,j,i)*m1-
                              IMATELEM(*imat,rpiv,i)*m2;
      }
      ivRowContent(imat, j, colpos+1);
    }
  }
}

static void ivZeroElim(intvec *imat, int colpos,
                     int ready, int &all)
{
  int j, i, k, l;

  k = ready;
  for (j=ready+1;j<=all;j++)
  {
    for (i=imat->cols();i>colpos;i--)
    {
      if (IMATELEM(*imat,j,i)!=0)
      {
        k++;
        if (k<j)
        {
          for (l=imat->cols();l>colpos;l--)
            IMATELEM(*imat,k,l) = IMATELEM(*imat,j,l);
        }
        break;
      }
    }
  }
  all = k;
}

static void ivRowContent(intvec *imat, int rowpos, int colpos)
{
  int tgcd, m;
  int i=imat->cols();

  loop
  {
    tgcd = IMATELEM(*imat,rowpos,i--);
    if (tgcd!=0) break;
    if (i<colpos) return;
  }
  if (tgcd<0) tgcd = -tgcd;
  if (tgcd==1) return;
  loop
  {
    m = IMATELEM(*imat,rowpos,i--);
    if (m!=0) tgcd= ivGcd(tgcd, m);
    if (tgcd==1) return;
    if (i<colpos) break;
  }
  for (i=imat->cols();i>=colpos;i--)
    IMATELEM(*imat,rowpos,i) /= tgcd;
}

static void ivKernFromRow(intvec *kern, intvec *imat,
                          intvec *perm, int pos, int r, int c)
{
  int piv, cp, g, i, j, k, s;

  for (i=c;i>(*perm)[r];i--)
  {
    IMATELEM(*kern,pos,i) = 1;
    for (j=r;j!=0;j--)
    {
      cp = (*perm)[j];
      s=0;
      for(k=c;k>cp;k--)
        s += IMATELEM(*imat,j,k)*IMATELEM(*kern,pos,k);
      if (s!=0)
      {
        piv = IMATELEM(*imat,j,cp);
        g = ivGcd(piv,s);
        if (g!=1)
        {
          s /= g;
          piv /= g;
        }
        for(k=c;k>cp;k--)
          IMATELEM(*kern,pos,k) *= piv;
        IMATELEM(*kern,pos,cp) = -s;
        ivRowContent(kern,pos,cp);
      }
    }
    if (IMATELEM(*kern,pos,i)<0)
      ivNegRow(kern,pos);
    pos--;
  }
}

static int ivGcd(int a,int b)
{
  int x;

  if (a<0) a=-a;
  if (b<0) b=-b;
  if (b>a)
  {
    x=b;
    b=a;
    a=x;
  }
  while (b!=0)
  {
    x = a % b;
    a = b;
    b = x;
  }
  return a;
}

static intvec * ivOptimizeKern(intvec *kern)
{
  int i,l,j,c=kern->cols(),r=kern->rows();
  intvec *res=new intvec(c);

  if (TEST_OPT_PROT)
    Warn(" %d linear independent solutions\n",r);
  for (i=r;i>1;i--)
  {
    for (j=c;j>0;j--)
    {
      (*res)[j-1] += IMATELEM(*kern,i,j);
    }
  }
  ivContent(res);
  if (r<11)
  {
    l = ivCondNumber(res,-c);
    j = ivL1Norm(res);
    ivOptRecursive(res, NULL, kern, l, j, r);
  }
  return res;
}

static void ivOptRecursive(intvec *res, intvec *w, intvec *kern,
                          int &l, int &j, int pos)
{
  int m, k, d;
  intvec *h;

  d=kern->rows();
  d=96/(d*d);
  if (d<3) d=3;
  if (w!=0)
    h = new intvec(w);
  else
    h = new intvec(res->rows());
  for (m=d;m>0;m--)
  {
    for(k=h->rows()-1;k>=0;k--)
      (*h)[k] += IMATELEM(*kern,pos,k+1);
    if(pos>1)
      ivOptRecursive(res, h, kern, l, j, pos-1);
    else
      ivOptSolve(res, h, l, j);
  }
  delete h;
  if (pos>1)
    ivOptRecursive(res, w, kern, l, j, pos-1);
  else if (w!=NULL)
    ivOptSolve(res, w, l, j);
}

static void ivOptSolve(intvec *res, intvec *w, int &l, int &j)
{
  int l0, j0, k;

  l0 = ivCondNumber(w, l);
  if (l0==l)
  {
    ivContent(w);
    j0 = ivL1Norm(w);
    if(j0<j)
    {
      j = j0;
      for(k=w->rows()-1;k>=0;k--)
        (*res)[k] = (*w)[k];
    }
    return;
  }
  if(l0>l)
  {
    l = l0;
    ivContent(w);
    j = ivL1Norm(w);
    for(k=w->rows()-1;k>=0;k--)
      (*res)[k] = (*w)[k];
  }
}

static int ivL1Norm(intvec *w)
{
  int i, j, s = 0;

  for (i=w->rows()-1;i>=0;i--)
  {
    j = (*w)[i];
    if (j>0)
      s += j;
    else
      s -= j;
  }
  return s;
}

static int ivCondNumber(intvec *w, int l)
{
  int l0=0, i;

  if (l<0)
  {
    for (i=w->rows()-1;i>=0;i--)
    {
      if ((*w)[i]<0) l0--;
    }
    if (l0==0)
    {
      for (i=w->rows()-1;i>=0;i--)
      {
        if ((*w)[i]>0) l0++;
      }
    }
    return l0;
  }
  else
  {
    for (i=w->rows()-1;i>=0;i--)
    {
      if ((*w)[i]<0) return -1;
    }
    for (i=w->rows()-1;i>=0;i--)
    {
      if ((*w)[i]>0) l0++;
    }
    return l0;
  }
}

static void ivContent(intvec *w)
{
  int tgcd, m;
  int i=w->rows()-1;

  loop
  {
    tgcd = (*w)[i--];
    if (tgcd!=0) break;
    if (i<0) return;
  }
  if (tgcd<0) tgcd = -tgcd;
  if (tgcd==1) return;
  loop
  {
    m = (*w)[i--];
    if (m!=0) tgcd= ivGcd(tgcd, m);
    if (tgcd==1) return;
    if (i<0) break;
  }
  for (i=w->rows()-1;i>=0;i--)
    (*w)[i] /= tgcd;
}

// columnwise concatination of two intvecs
intvec * ivConcat(intvec * a, intvec * b)
{
  int ac=a->cols();
  int c = ac + b->cols(); int r = si_max(a->rows(),b->rows());
  intvec * ab = new intvec(r,c,0);

  int i,j;
  for (i=1; i<=a->rows(); i++)
  {
    for(j=1; j<=ac; j++)
      IMATELEM(*ab,i,j) = IMATELEM(*a,i,j);
  }
  for (i=1; i<=b->rows(); i++)
  {
    for(j=1; j<=b->cols(); j++)
      IMATELEM(*ab,i,j+ac) = IMATELEM(*b,i,j);
  }
  return ab;
}

intvec* intvec::delete_pos(int p)
{
  if (!range(p)) return NULL;
  intvec *iv=new intvec(rows()-1);
  for(int i=0;i<p;i++) (*iv)[i]=v[i];
  for(int i=p+1;i<rows();i++) (*iv)[i-1]=v[i];
  return iv;
}

#pragma GCC pop_options

#endif
