/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: intvec.cc,v 1.18 2000-09-18 09:19:02 obachman Exp $ */
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#ifndef INTVEC_CC
#define INTVEC_CC
#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "intvec.h"
#include "omalloc.h"

/*0 implementation*/


intvec::intvec(intvec* iv)
{
  row = iv->rows();
  col = iv->cols();
  v   = (int *)omAlloc(sizeof(int)*row*col);
  for (int i=0; i<row*col; i++)
  {
    v[i] = (*iv)[i];
  }
}

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
  if ((r>0) && (c>0))
    v = (int *)omAlloc(sizeof(int)*l);
  else
    v = NULL;
  for (int i=0; i<l; i++)
  {
    v[i] = init;
  }
}

char * intvec::ivString(int mat,int spaces, int dim)
{
  StringSetS("");
  if ((col == 1)&&(mat!=INTMAT_CMD))
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
  return StringAppendS("");
}

void intvec::resize(int new_length)
{
  assume(new_length > 0 && col == 1);
  v = (int*) omRealloc0Size(v, row*sizeof(int), new_length*sizeof(int));
  row = new_length;
}

char * intvec::String(int dim)
{
  return omStrDup(ivString(0, 0, dim));
}

void intvec::show(int mat,int spaces)
{
  if (spaces>0)
    Print("%-*.*s%s",spaces,spaces," ",ivString(mat,spaces));
  else
    PrintS(ivString(mat,0));
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
  for (int i=0; i<row*col; i++) { v[i] /= intop; }
}

void intvec::operator%=(int intop)
{
  if (intop == 0) return;
  if (intop<0) intop*=(-1);
  for (int i=0; i<row*col; i++)
  { v[i] %= intop; if (v[i]<0) v[i] += intop; }
}

int intvec::compare(intvec* op)
{
  if ((col!=1) ||(op->cols()!=1))
  {
    if((col!=op->cols())
    || (row!=op->rows()))
      return -2;
  }
  int i;
  for (i=0; i<min(length(),op->length()); i++)
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
int intvec::compare(int o)
{
  for (int i=0; i<row*col; i++)
  {
    if (v[i] <o) return -1;
    if (v[i] >o) return 1;
  }
  return 0;
}

intvec * ivCopy(intvec * o)
{
  intvec * iv=new intvec(o);
  return iv;
}

intvec * ivAdd(intvec * a, intvec * b)
{
  intvec * iv;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = min(a->rows(),b->rows());
  ma = max(a->rows(),b->rows());
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

intvec * ivSub(intvec * a, intvec * b)
{
  intvec * iv;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = min(a->rows(),b->rows());
  ma = max(a->rows(),b->rows());
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
  int i, s = 0, m = min(o->rows(),o->cols()), c = o->cols();
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

/*3
* gcd of a and b
*/
static int ivIntGcd(int a,int b)
{
  int x;

  while (b!=0)
  {
    x = b;
    b = a % x;
    a = x;
  }
  return a;
}

/*2
*computes a triangular matrix of the lower right submatrix
*beginning with the field (srw,cl) by Gaussian elimination
*/
void ivTriangMat(intvec * imat, int srw, int cl)
{
  int i=srw,t,j,k,tgcd,mult1,mult2;

  if ((cl>imat->cols()) || (srw>imat->rows())) return;
  ivCancelContent(imat,srw);
  while ((i<=imat->rows()) && (IMATELEM(*imat,i,cl)==0)) i++;
  if ((i>srw ) && (i<=imat->rows()))
  {
    for (j=cl;j<=imat->cols();j++)
    {
      t = IMATELEM(*imat,srw,j);
      IMATELEM(*imat,srw,j) = IMATELEM(*imat,i,j);
      IMATELEM(*imat,i,j) = t;
    }
  }
  if (i<=imat->rows())
  {
    for (k=srw+1;k<=imat->rows();k++)
    {
      if (IMATELEM(*imat,k,cl)!=0)
      {
        tgcd = ivIntGcd(IMATELEM(*imat,srw,cl),IMATELEM(*imat,k,cl));
        mult1 = IMATELEM(*imat,k,cl) / tgcd;
        mult2 = IMATELEM(*imat,srw,cl) / tgcd;
        IMATELEM(*imat,k,cl) = 0;
        for (j=cl+1;j<=imat->cols();j++)
        {
          IMATELEM(*imat,k,j) =
              mult2*IMATELEM(*imat,k,j)-mult1*IMATELEM(*imat,srw,j);
        }
      }
    }
    srw++;
  }
  ivTriangMat(imat,srw,cl+1);
  return;
}

/*2
*returns the number of the first zero row,
*-1 for no zero row
*/
int ivFirstEmptyRow(intvec * imat)
{
  int i,j;

  for (i=1;i<=imat->rows();i++)
  {
    j = 1;
    while ((j<=imat->cols()) && (IMATELEM(*imat,i,j)==0)) j++;
    if (j>imat->cols()) return i;
  }
  return -1;
}

/*2
*divide out the content for every row >= from
*/
void ivCancelContent(intvec * imat,int from)
{
  if (imat->cols()<2) return;
  int i,j,tgcd;

  for (i=from;i<=imat->rows();i++)
  {
    tgcd = ivIntGcd(IMATELEM(*imat,i,1),IMATELEM(*imat,i,2));
    j = 3;
    while ((j<=imat->cols()) && ((ABS(tgcd)>1) || (tgcd==0)))
    {
      tgcd = ivIntGcd(tgcd,IMATELEM(*imat,i,j));
      j++;
    }
    if ((j>imat->cols()) && (ABS(tgcd)>1))
     for (j=1;j<=imat->cols();j++) IMATELEM(*imat,i,j) /= tgcd;
  }
  return;
}

/*3
*searches the independent columns of the system given by imat
*imat is assumed to be in triangular shape
*/
static intvec * ivIndepCols(intvec * imat)
{
  intvec * result=new intvec(1,imat->cols(),0);
  int i,j;

  for (i=1;i<=imat->rows();i++)
  {
    j=1;
    while ((j<=imat->cols()) && (IMATELEM(*imat,i,j)==0)) j++;
    if (j<=imat->cols()) IMATELEM(*result,1,j)=i;
  }
  return result;
}

/*3
* computes basic solutions of the system given by imat
*/
static intvec * ivBasicSol(intvec * imat)
{
  intvec * result = new intvec(2*imat->cols(),imat->cols(),0);
  intvec * indep=ivIndepCols(imat),*actsol=new intvec(imat->cols());
  int actPosSol=1,actNegSol=imat->cols()+1,i=imat->cols(),j,k;
  int tgcd,tlcm;
  BOOLEAN isNeg;

  while (i>0)
  {
    if ((*indep)[i-1]==0)
    {
      for (j=0;j<actsol->length();j++) (*actsol)[j] = 0;
      (*actsol)[i-1] = 1;
      isNeg = FALSE;
      j = i-1;
      while (j>0)
      {
        if ((*indep)[j-1]!=0)
        {
          tlcm = 0;
          for (k=0;k<i;k++)
            tlcm += IMATELEM(*imat,(*indep)[j-1],k+1)*(*actsol)[k];
          tgcd = ivIntGcd(tlcm,IMATELEM(*imat,(*indep)[j-1],j));
          tlcm /= tgcd;
          tgcd = IMATELEM(*imat,(*indep)[j-1],j) / tgcd;
          if (tgcd<0)
          {
            tgcd = -tgcd;
            tlcm = -tlcm;
          }
          for (k=0;k<i;k++) (*actsol)[k] *= tgcd;
          (*actsol)[j-1] = -tlcm;
          isNeg = isNeg || (tlcm>0);
        }
        j--;
      }
/*writes the solution into result depending on the sign*/
      if (isNeg)
      {
        for (j=1;j<=imat->cols();j++)
          IMATELEM(*result,actNegSol,j) = (*actsol)[j-1];
        actNegSol++;
      }
      else
      {
        for (j=1;j<=imat->cols();j++)
          IMATELEM(*result,actPosSol,j) = (*actsol)[j-1];
        actPosSol++;
      }
    }
    i--;
  }
  delete actsol;
  delete indep;
  return result;
}

/*2
*returns an integer solution of imat which is assumed to be
*in triangular form
*the solution contains as many positive entrees as possible
*/
intvec * ivSolveIntMat(intvec * imat)
{
  intvec * result=new intvec(imat->cols());
  intvec * basesol=ivBasicSol(imat);
  int i=imat->cols(),j,k,l,t;

/*adds all base solutions*/
  for (j=1;j<=2*i;j++)
  {
    for (k=1;k<=i;k++)
      (*result)[k-1] += IMATELEM(*basesol,j,k);
  }
/*tries to cancel negative entrees by adding positive solutions*/
  j = 1;
  while (j<=i)
  {
    if ((*result)[j-1]<0)
    {
      t = -1;
      l = 0;
      for (k=1;k<=i;k++)
      {
        if (IMATELEM(*basesol,k,j)>t)
        {
          t = IMATELEM(*basesol,k,j);
          l = k;
        }
      }
      if (t>0)
      {
        t = ((*result)[j-1] / t) +1;
        for (k=1;k<=i;k++)
        {
          (*result)[k-1] += t*IMATELEM(*basesol,l,k);
        }
      }
    }
    j++;
  }
  delete basesol;
  return result;
}

#endif
