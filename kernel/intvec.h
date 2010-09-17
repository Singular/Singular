#ifndef INTVEC_H
#define INTVEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: class intvec: lists/vectors of integers
*/
#include <string.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>


//extern omBin intvec_bin;

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
  intvec(const intvec* iv)
  {
    assume( iv != NULL );
    row = iv->rows();
    col = iv->cols();
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
    { return ((i<row) && (i>=0) && (col==1)); }
  inline int range(int i, int j) const
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
  inline const int& operator[](int i) const
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
  int compare(const intvec* o) const;
  int compare(int o) const;
  inline int  length() const { return col*row; }
  inline int  cols() const { return col; }
  inline int  rows() const { return row; }
  inline void length(int l) { row = l; col = 1; }
  void show(int mat=0,int spaces=0) const;
  inline void makeVector() { row*=col;col=1; }
  char * String(int dim = 2) const;
  char * ivString(int not_mat=1,int spaces=0, int dim=2) const;
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
    for (int i=row*col-1; i>0; i--) if (v[i]<m) m=v[i];
    return m;
  }
#if 0
  void* operator new ( size_t size )
  {
    void* addr;
    //omTypeAlloc(void*, addr, size);
    addr=omAlloc0Bin(intvec_bin);
    return addr;
  }
  void operator delete ( void* block )
  { //omfree( block );
    omFreeBin((ADDRESS)block, intvec_bin);
  }
#endif
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


#ifdef MDEBUG
#define ivTest(v) v->ivTEST()
#else
#define ivTest(v)   ((void)0)
#endif
#undef INLINE_THIS

#endif
