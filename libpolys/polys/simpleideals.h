#ifndef SIMPLEIDEALS_H
#define SIMPLEIDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/
#include <polys/monomials/ring.h>
#include <omalloc/omalloc.h>

struct sip_sideal
{
  poly*  m;
  long rank;
  int nrows;
  int ncols;
  #define IDELEMS(i) ((i)->ncols)
  #define MATCOLS(i) ((i)->ncols)
  #define MATROWS(i) ((i)->nrows)
  #define MATELEM(mat,i,j) ((mat)->m)[MATCOLS((mat)) * ((i)-1) + (j)-1]
   
};

struct sip_smap
{
  poly *m;
  char *preimage;
  int nrows;
  int ncols;
};

class ip_smatrix;
typedef ip_smatrix *       matrix;

struct sideal_list;
typedef struct sideal_list *      ideal_list;

struct sideal_list
{
  ideal_list next;
  ideal      d;
#ifndef NDEBUG
  int nr;
#endif
};

extern omBin sip_sideal_bin;

#ifdef PDEBUG
ideal idDBInit (int size, int rank, const char *f, int l);
#define idInit(A,B) idDBInit(A,B,__FILE__,__LINE__)
#else
/*- creates an ideal -*/
ideal idInit (int size, int rank=1);
#endif
/*- deletes an ideal -*/
#define idDelete(h) id_Delete(h, currRing)
void id_Delete (ideal* h, ring r);
void id_ShallowDelete (ideal* h, ring r);
void idSkipZeroes (ideal ide);
  /*gives an ideal the minimal possible size*/

#ifdef PDEBUG
void id_DBTest(ideal h1, int level, const char *f,const int l, const ring r);
#define id_Test(A, r) id_DBTest(A, PDEBUG, __FILE__,__LINE__, r)
// #define id_Print(id, r) id_Show(id, r)
#else
#define id_Test(A, r)  (TRUE)
// #define id_Print(A, r) ((void)0)
#endif

ideal id_Copy (ideal h1,const ring r);
#ifdef PDEBUG
ideal idDBCopy(ideal h1,const char *f,int l);
#define idCopy(A) idDBCopy(A,__FILE__,__LINE__)
#else
#define idCopy(A) id_Copy(A,currRing)
#endif
  /*adds two ideals without simplifying the result*/
ideal idSimpleAdd (ideal h1,ideal h2);
  /*adds the quotient ideal*/
ideal idAdd (ideal h1,ideal h2);
  /* h1 + h2 */
BOOLEAN idIs0 (ideal h);

long id_RankFreeModule(ideal m, ring lmRing, ring tailRing);
static inline long id_RankFreeModule(ideal m, ring r)
{return id_RankFreeModule(m, r, r);}
// returns TRUE, if idRankFreeModule(m) > 0
BOOLEAN id_IsModule(ideal m, ring r);
ideal   idFreeModule (int i);
int     idElem(const ideal F);
int id_PosConstant(ideal id, const ring r);
ideal id_MaxIdeal (const ring r);
ideal id_CopyFirstK (const ideal ide, const int k,const ring r);
void id_DelMultiples(ideal id, const ring r);
void id_Norm(ideal id, const ring r);
void id_DelEquals(ideal id, const ring r);
void id_DelLmEquals(ideal id, const ring r);
void id_DelDiv(ideal id, const ring r);
BOOLEAN id_IsConstant(ideal id, const ri ng r);


#endif
