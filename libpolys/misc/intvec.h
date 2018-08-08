#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#include <string.h>
#include "misc/auxiliary.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#include "omalloc/omallocClass.h"
#else
#include "xalloc/omalloc.h"
#endif

#include "reporter/reporter.h"


class intvec
#ifndef XMEMORY_H
             :public omallocClass
#endif
{
private:
  int *v;
  int row;
  int col;
public:

  inline intvec(int l = 1)
  {
    assume(l >= 0);
    if (l>0) v = (int *)omAlloc0(sizeof(int)*l);
    else     v = NULL;
    row = l;
    col = 1;
  }
  intvec(int s, int e);
  intvec(int r, int c, int init);
  intvec(const intvec* iv)
  {
    assume( iv != NULL );
    row = iv->rows();
    col = iv->cols();
    assume(row >= 0);
    assume(col >= 0);
    if (row*col>0)
    {
      v   = (int *)omAlloc(sizeof(int)*row*col);
      for (int i=row*col-1;i>=0; i--)
      {
        v[i] = (*iv)[i];
      }
    }
    else v=NULL;
  }

  void resize(int new_length);
  inline int range(int i) const
    //{ return ((i<row) && (i>=0) && (col==1)); }
    { return ((((unsigned)i)<((unsigned)row)) && (col==1)); }
  inline int range(int i, int j) const
    //{ return ((i<row) && (i>=0) && (j<col) && (j>=0)); }
    { return ((((unsigned)i)<((unsigned)row)) && (((unsigned)j)<((unsigned)col))); }
  inline int& operator[](int i)
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong intvec index:%d\n",i);
      }
#endif
      return v[i];
    }
  inline const int& operator[](int i) const
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong intvec index:%d\n",i);
      }
#endif
      return v[i];
    }
#define IMATELEM(M,I,J) (M)[(I-1)*(M).cols()+J-1]
  void operator+=(int intop);
  void operator-=(int intop);
  void operator*=(int intop);
  void operator/=(int intop);
  void operator%=(int intop);
  // -2: not compatible, -1: <, 0:=, 1: >
  int compare(const intvec* o) const;
  int compare(int o) const;
  inline int  length() const { return col*row; }
  inline int  cols() const { return col; }
  inline int  rows() const { return row; }
  void show(int mat=0,int spaces=0) const;
  #ifndef SING_NDEBUG
  void view() const;
  #endif

  inline void makeVector() { row*=col;col=1; }
  char * String(int dim = 2) const;
  char * ivString(int not_mat=1,int spaces=0, int dim=2) const;
  inline ~intvec()
    {
      assume(row>=0);
      assume(col>=0);
      if (v!=NULL)
      {
        omFreeSize((ADDRESS)v,sizeof(int)*row*col);
        v=NULL;
      }
    }
  inline void ivTEST() const
    {
      assume(row>=0);
      assume(col>=0);
      if (row>0) omCheckAddrSize((ADDRESS)v,sizeof(int)*row*col);
    }
  inline int min_in()
  {
    int m=0;
    if (row>0)
    {
      m=v[0];
      for (int i=row*col-1; i>0; i--) if (v[i]<m) m=v[i];
    }
    return m;
  }
  intvec* delete_pos(int p);
  // keiner (ausser obachman) darf das folgenden benutzen !!!
  inline int * ivGetVec() { return v; }
};
inline intvec * ivCopy(const intvec * o)
{
  if( o != NULL )
    return new intvec(o);
  return NULL;
}

intvec * ivAdd(intvec * a, intvec * b);
intvec * ivSub(intvec * a, intvec * b);
intvec * ivTranp(intvec * o);
int      ivTrace(intvec * o);
intvec * ivMult(intvec * a, intvec * b);
//void     ivTriangMat(intvec * imat);
void     ivTriangIntern(intvec * imat, int &ready, int &all);
intvec * ivSolveKern(intvec * imat, int ready);
intvec * ivConcat(intvec * a, intvec * b);

#ifdef MDEBUG
inline void ivTest(intvec * v)
{
  v->ivTEST();
}
#else
#define ivTest(v) do {} while (0)
#endif

#undef INLINE_THIS

#endif
