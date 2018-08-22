/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: class int64vec: lists/vectors of int64
*/


#include "misc/auxiliary.h"



#include "misc/int64vec.h"
#include "misc/intvec.h"

/*0 implementation*/


int64vec::int64vec(int64vec* iv)
{
  row = iv->rows();
  col = iv->cols();
  v   = (int64 *)omAlloc(sizeof(int64)*row*col);
  for (int i=0; i<row*col; i++)
  {
    v[i] = (*iv)[i];
  }
}

int64vec::int64vec(intvec* iv)
{
  row = iv->rows();
  col = iv->cols();
  v   = (int64 *)omAlloc(sizeof(int64)*row*col);
  for (int i=0; i<row*col; i++)
  {
    v[i] = (int64)((*iv)[i]);
  }
}

int64vec::int64vec(int r, int c, int64 init)
{
  row = r;
  col = c;
  int l = r*c;
  if ((r>0) && (c>0))
    v = (int64 *)omAlloc(sizeof(int64)*l);
  else
    v = NULL;
  for (int i=0; i<l; i++)
  {
    v[i] = init;
  }
}

char * int64vec::iv64String(int not_mat, int /*mat*/, int spaces, int dim)
{
  StringSetS("");
  if ((col == 1)&&(not_mat))
  {
    int i=0;
    for (; i<row-1; i++)
    {
      StringAppend("%lld,",v[i]);
    }
    if (i<row)
    {
      StringAppend("%lld",v[i]);
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
          StringAppend("%lld%c",v[j*col+i],',');
        }
      }
      else
      {
        for (int i=0; i<col; i++)
        {
          StringAppend("%lld%c",v[j*col+i],i<col-1 ? ',' : ' ');
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

char * int64vec::String(int dim)
{
  return iv64String(0, 0, dim);
}

void int64vec::show(int notmat,int spaces)
{
  char *s=iv64String(notmat,spaces);
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

void int64vec::operator*=(int64 intop)
{
  for (int i=row*col-1; i>=0; i--) { v[i] *= intop; }
}

void int64vec::operator/=(int64 intop)
{
  if (intop == 0) return;
  int64 bb=ABS(intop);
  for (int i=row*col-1; i>=0; i--)
  {
    int64 r=v[i];
    int64 c=r%bb;
    if (c<0) c+=bb;
    r=(r-c)/intop;
    v[i]=r;
  }
}

int int64vec::compare(const int64vec* op) const
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
  // this can only happen for int64vec: (i.e. col==1)
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

int64vec * iv64Add(int64vec * a, int64vec * b)
{
  int64vec * iv;
  int64 mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    iv = new int64vec(ma);
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
  iv = new int64vec(a);
  for (i=0; i<mn*a->cols(); i++) { (*iv)[i] += (*b)[i]; }
  return iv;
}

int64vec * iv64Sub(int64vec * a, int64vec * b)
{
  int64vec * iv;
  int mn, ma,i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    iv = new int64vec(ma);
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
  iv = new int64vec(a);
  for (i=0; i<mn*a->cols(); i++) { (*iv)[i] -= (*b)[i]; }
  return iv;
}


/*
 * The following two functions are never used.
 * Remove?

// def. internals
static int64 iv64Gcd(int, int);
static int64 iv64L1Norm(intvec *);


// The following two functions seem to be never used. Remove?
static int64 iv64Gcd(int64 a,int64 b)
{
  int64 x;

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

static int64 iv64L1Norm(int64vec *w)
{
  int i;
  int64 j, s = 0;

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
*/
