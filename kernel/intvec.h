#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: intvec.h,v 1.1.1.1 2003-10-06 12:15:55 Singular Exp $ */
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

  intvec(int l = 1)
    {
      v = (int *)omAlloc0(sizeof(int)*l);
      row = l;
      col = 1;
    }
  intvec(int s, int e);
  intvec(int r, int c, int init);
  intvec(intvec* iv);

  void resize(int new_length);
  int range(int i)
    { return ((i<row) && (i>=0) && (col==1)); }
  int range(int i, int j)
    { return ((i<row) && (i>=0) && (j<col) && (j>=0)); }
  int& operator[](int i)
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
  int  length() const { return col*row; }
  int  cols() const { return col; }
  int  rows() const { return row; }
  void length(int l) { row = l; col = 1; }
  void show(int notmat=1,int spaces=0);
  void makeVector() { row*=col;col=1; }
  // keiner (ausser obachman) darf das folgenden benutzen !!!
  int * ivGetVec() { return v; }
  char * String(int dim = 2);
  char * ivString(int mat=0,int spaces=0, int dim=2);
  ~intvec()
    {
      if (v!=NULL)
      {
        omFreeSize((ADDRESS)v,sizeof(int)*row*col);
        v=NULL;
      }
    }
  void ivTEST()
    {
      omCheckAddrSize((ADDRESS)v,sizeof(int)*row*col);
    }
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
