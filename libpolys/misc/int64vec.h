#ifndef INT64VEC_H
#define INT64VEC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: class intvec: lists/vectors of int64
*/
#include <string.h>
#include "misc/auxiliary.h"

#include "misc/auxiliary.h"
#include "omalloc/omalloc.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omallocClass.h"
#endif

#include "misc/intvec.h"

class int64vec
#ifdef HAVE_OMALLOC
               :public omallocClass
#endif
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
  int64vec(int r, int c, int64 init);
  int64vec(int64vec* iv);
  int64vec(intvec* iv);
  int64& operator[](int i)
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong int64vec index:%d\n",i);
      }
#endif
      return v[i];
    }
  inline const int64& operator[](int i) const
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong int64vec index:%d\n",i);
      }
#endif
      return v[i];
    }
  void operator*=(int64 intop);
  void operator/=(int64 intop);
  // -2: not compatible, -1: <, 0:=, 1: >
  int compare(const int64vec* o) const;
  int  length() const { return col*row; }
  int  cols() const { return col; }
  int  rows() const { return row; }
  void show(int mat=0,int spaces=0);
  char * String(int dim = 2);
  char * iv64String(int not_mat=1,int mat=0,int spaces=0, int dim=2);
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
inline int64vec * iv64Copy(int64vec * o)
{
  int64vec * iv=new int64vec(o);
  return iv;
}

int64vec * iv64Add(int64vec * a, int64vec * b);
int64vec * iv64Sub(int64vec * a, int64vec * b);

#ifdef MDEBUG
#define iv64Test(v) v->iv64TEST()
#else
#define iv64Test(v)   do {} while (0)
#endif

#endif
