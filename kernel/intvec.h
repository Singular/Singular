#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: intvec.h,v 1.2 2004-07-16 08:43:00 Singular Exp $ */
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#include <string.h>
#include "omalloc.h"
#include "febase.h"

class intvec
{
private:
  int *v;
  int row;
  int col;
public:

  inline intvec(int l = 1)
    {
      v = (int *)omAlloc0(sizeof(int)*l);
      row = l;
      col = 1;
    }
  intvec(int s, int e);
  intvec(int r, int c, int init);
  intvec(intvec* iv);

  void resize(int new_length);
  inline int range(int i)
    { return ((i<row) && (i>=0) && (col==1)); }
  inline int range(int i, int j)
    { return ((i<row) && (i>=0) && (j<col) && (j>=0)); }
  inline int& operator[](int i)
    {
#ifndef NDEBUG
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
  int compare(intvec* o);
  int compare(int o);
  inline int  length() const { return col*row; }
  inline int  cols() const { return col; }
  inline int  rows() const { return row; }
  inline void length(int l) { row = l; col = 1; }
  void show(int mat=0,int spaces=0);
  inline void makeVector() { row*=col;col=1; }
  char * String(int dim = 2);
  char * ivString(int mat=0,int spaces=0, int dim=2);
  inline ~intvec()
    {
      if (v!=NULL)
      {
        omFreeSize((ADDRESS)v,sizeof(int)*row*col);
        v=NULL;
      }
    }
  inline void ivTEST()
    {
      omCheckAddrSize((ADDRESS)v,sizeof(int)*row*col);
    }
  inline int min_in()
  {
    int m=v[0];
    for (int i=row*col; i>0; i--) if (v[i]<m) m=v[i];
    return m;
  }
  // keiner (ausser obachman) darf das folgenden benutzen !!!
  inline int * ivGetVec() { return v; }
};
intvec * ivCopy(intvec * o);
intvec * ivAdd(intvec * a, intvec * b);
intvec * ivSub(intvec * a, intvec * b);
intvec * ivTranp(intvec * o);
int      ivTrace(intvec * o);
intvec * ivMult(intvec * a, intvec * b);
//void     ivTriangMat(intvec * imat);
void     ivTriangIntern(intvec * imat, int &ready, int &all);
intvec * ivSolveKern(intvec * imat, int ready);


#ifdef MDEBUG
#define ivTest(v) v->ivTEST()
#else
#define ivTest(v)   ((void)0)
#endif
#undef INLINE_THIS

#endif
