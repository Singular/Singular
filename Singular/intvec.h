#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: intvec.h,v 1.12 1999-11-15 17:20:07 obachman Exp $ */
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#include <string.h>
#include "mmemory.h"
#include "febase.h"

// Macros for getting new intvecs to enable debugging of memory
#ifdef MDEBUG
#define NewIntvec0()        new intvec(__FILE__, __LINE__)
#define NewIntvec1(l)       new intvec(__FILE__, __LINE__, l)
#define NewIntvec2(s, e)    new intvec(__FILE__, __LINE__, s, e)
#define NewIntvec3(r, c, i) new intvec(__FILE__, __LINE__, r, c, i)
#define NewIntvecIv(iv)     new intvec(__FILE__, __LINE__, iv)
#else
#define NewIntvec0  new intvec
#define NewIntvec1  new intvec
#define NewIntvec2  new intvec
#define NewIntvec3  new intvec
#define NewIntvecIv new intvec
#endif

class intvec
{
private:
  int *v;
  int row;
  int col;
public:

#ifdef MDEBUG
  intvec(char* file, int line, int l = 1);
  intvec(char* file, int line, int s, int e);
  intvec(char* file, int line, int r, int c, int init);
  intvec(char* file, int line, intvec* iv);
#endif

  intvec(int l = 1)
    {
      v = (int *)Alloc0(sizeof(int)*l);
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
  void show(int mat=0,int spaces=0);
  void makeVector() { row*=col;col=1; }
  // keiner (ausser obachman) darf das folgenden benutzen !!!
  int * ivGetVec() { return v; }
  char * String(int dim = 2);
  char * ivString(int mat=0,int spaces=0, int dim=2);
  ~intvec()
    {
      mmTestL(this);
      if (v!=NULL)
      {
        Free((ADDRESS)v,sizeof(int)*row*col);
        v=NULL;
      }
    }
  void ivTEST()
    {
      mmTestL(this);
      mmTest((ADDRESS)v,sizeof(int)*row*col);
    }
};
intvec * ivCopy(intvec * o);
intvec * ivAdd(intvec * a, intvec * b);
intvec * ivSub(intvec * a, intvec * b);
intvec * ivTranp(intvec * o);
int      ivTrace(intvec * o);
intvec * ivMult(intvec * a, intvec * b);
void     ivTriangMat(intvec * imat, int srw, int col);
int      ivFirstEmptyRow(intvec * imat);
void     ivCancelContent(intvec * imat,int from=1);
intvec * ivSolveIntMat(intvec * imat);

#undef INLINE_THIS

#endif
