#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: intvec.h,v 1.8 1999-06-28 16:06:23 Singular Exp $ */
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#include <string.h>
#include "mmemory.h"
#include "febase.h"

class intvec
{
  private:
    int *v;
    int row;
    int col;
  public:
    intvec(int l=1)
    {
      v = (int *)Alloc0(sizeof(int)*l);
      row = l;
      col = 1;
    }
    intvec(int s, int e);
    intvec(int r, int c, int init);
    intvec(intvec* iv);
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
#ifdef MDEBUG
         mmTestL(this);
#endif
         if (v!=NULL)
         {
            Free((ADDRESS)v,sizeof(int)*row*col);
            v=NULL;
         }
       }
    void ivTEST()
       {
#ifdef MDEBUG
         mmTestL(this);
         mmTest((ADDRESS)v,sizeof(int)*row*col);
#endif
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

#endif
