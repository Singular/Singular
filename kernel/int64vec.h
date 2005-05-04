#ifndef INT64VEC_H
#define INT64VEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: int64vec.h,v 1.1 2005-05-04 14:08:54 Singular Exp $ */
/*
* ABSTRACT: class intvec: lists/vectors of int64
*/
#include <string.h>
#include "structs.h"
#include "omalloc.h"
#include "febase.h"
#include "intvec.h"

class int64vec
{
private:
  int64 *v;
  int row;
  int col;
public:

  int64vec(int l = 1)
    {
      v = (int64 *)omAlloc0(sizeof(int64)*l);
      row = l;
      col = 1;
    }
  int64vec(int s, int e);
  int64vec(int r, int c, int64 init);
  int64vec(int64vec* iv);
  int64vec(intvec* iv);
  void resize(int new_length);
  int range(int i)
    { return ((i<row) && (i>=0) && (col==1)); }
  int range(int i, int j)
    { return ((i<row) && (i>=0) && (j<col) && (j>=0)); }
  int64& operator[](int i)
    {
#ifndef NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong int64vec index:%d\n",i);
      }
#endif
      return v[i];
    }
  void operator+=(int64 intop);
  void operator-=(int64 intop);
  void operator*=(int64 intop);
  void operator/=(int64 intop);
  void operator%=(int64 intop);
  // -2: not compatible, -1: <, 0:=, 1: >
  int compare(int64vec* o);
  int compare(int64 o);
  int  length() const { return col*row; }
  int  cols() const { return col; }
  int  rows() const { return row; }
  void length(int l) { row = l; col = 1; }
  void show(int mat=0,int spaces=0);
  void makeVector() { row*=col;col=1; }
  char * String(int dim = 2);
  char * iv64String(int not_mat=1,int mat=0,int spaces=0, int dim=2);
  // keiner (ausser obachman) darf das folgenden benutzen !!!
  int64 * iv64GetVec() { return v; }
  ~int64vec()
    {
      if (v!=NULL)
      {
        omFreeSize((ADDRESS)v,sizeof(int64)*row*col);
        v=NULL;
      }
    }
  void iv64TEST()
    {
      omCheckAddrSize((ADDRESS)v,sizeof(int64)*row*col);
    }
};
int64vec * iv64Copy(int64vec * o);
int64vec * iv64Add(int64vec * a, int64vec * b);
int64vec * iv64Sub(int64vec * a, int64vec * b);
int64vec * iv64Tranp(int64vec * o);
int64      iv64Trace(int64vec * o);
int64vec * iv64Mult(int64vec * a, int64vec * b);

#ifdef MDEBUG
#define iv64Test(v) v->iv64TEST()
#else
#define iv64Test(v)   ((void)0)
#endif

#endif











