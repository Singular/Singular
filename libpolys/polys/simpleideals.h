#ifndef SIMPLEIDEALS_H
#define SIMPLEIDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/
#include <omalloc/omalloc.h>
#include <polys/monomials/ring.h>
#include <polys/matpol.h>

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

/*- creates an ideal -*/
ideal idInit (int size, int rank=1);

/*- deletes an ideal -*/
void id_Delete (ideal* h, ring r);
void id_ShallowDelete (ideal* h, ring r);
void idSkipZeroes (ideal ide);
  /*gives an ideal the minimal possible size*/

/// number of non-zero polys in F
int     idElem(const ideal F);
/// normialize all polys in id
void    id_Normalize(ideal id, const ring r);

int id_MinDegW(ideal M,intvec *w, const ring r);

#ifdef PDEBUG
void id_DBTest(ideal h1, int level, const char *f,const int l, const ring r);
#define id_Test(A, r) id_DBTest(A, PDEBUG, __FILE__,__LINE__, r)
// #define id_Print(id, r) id_Show(id, r)
#else
#define id_Test(A, r)  (TRUE)
// #define id_Print(A, r) ((void)0)
#endif

ideal id_Copy (ideal h1,const ring r);

  /*adds two ideals without simplifying the result*/
ideal id_SimpleAdd (ideal h1,ideal h2, const ring r);
  /*adds the quotient ideal*/
ideal id_Add (ideal h1,ideal h2,const ring r);
  /* h1 + h2 */

ideal id_Power(ideal given,int exp, const ring r);
BOOLEAN idIs0 (ideal h);

long id_RankFreeModule(ideal m, ring lmRing, ring tailRing);
static inline long id_RankFreeModule(ideal m, ring r)
{return id_RankFreeModule(m, r, r);}
// returns TRUE, if idRankFreeModule(m) > 0
BOOLEAN id_IsModule(ideal m, ring r);
ideal   id_FreeModule (int i, const ring r);
int     idElem(const ideal F);
int id_PosConstant(ideal id, const ring r);
ideal id_Head(ideal h,const ring r);
ideal id_MaxIdeal (const ring r);
ideal id_MaxIdeal(int deg, const ring r);
ideal id_CopyFirstK (const ideal ide, const int k,const ring r);
void id_DelMultiples(ideal id, const ring r);
void id_Norm(ideal id, const ring r);
void id_DelEquals(ideal id, const ring r);
void id_DelLmEquals(ideal id, const ring r);
void id_DelDiv(ideal id, const ring r);
BOOLEAN id_IsConstant(ideal id, const ring r);
intvec *id_Sort(ideal id,BOOLEAN nolex, const ring r);
ideal id_Transp(ideal a, const ring rRing);
void id_Compactify(ideal id, const ring r);
ideal  id_Mult (ideal h1,ideal  h2, const ring r);
ideal id_Homogen(ideal h, int varnum,const ring r);
BOOLEAN id_HomIdeal (ideal id, ideal Q, const ring r);
BOOLEAN id_HomModule(ideal m, ideal Q, intvec **w, const ring R);
BOOLEAN id_IsZeroDim(ideal I, const ring r);
ideal id_Jet(ideal i,int d, const ring R);
ideal id_JetW(ideal i,int d, intvec * iv, const ring R);
ideal  id_Subst(ideal id, int n, poly e, const ring r);
matrix id_Module2Matrix(ideal mod, const ring R);
matrix id_Module2formatedMatrix(ideal mod,int rows, int cols, const ring R);
ideal id_Matrix2Module(matrix mat, const ring R);
ideal id_Vec2Ideal(poly vec, const ring R);

int id_ReadOutPivot(ideal arg, int* comp, const ring r);

int     binom (int n,int r);

  /*- verschiebt die Indizes der Modulerzeugenden um i -*/
void    idInitChoise (int r,int beg,int end,BOOLEAN *endch,int * choise);
void    idGetNextChoise (int r,int end,BOOLEAN *endch,int * choise);
int     idGetNumberOfChoise(int t, int d, int begin, int end, int * choise);

#ifdef PDEBUG
void idShow(const ideal id, const ring lmRing, const ring tailRing, const int debugPrint = 0);
#endif


/// insert h2 into h1 depending on the two boolean parameters:
/// - if zeroOk is true, then h2 will also be inserted when it is zero
/// - if duplicateOk is true, then h2 will also be inserted when it is
///   already present in h1
/// return TRUE iff h2 was indeed inserted
BOOLEAN id_InsertPolyWithTests (ideal h1, const int validEntries,
                                const poly h2, const bool zeroOk,
                                const bool duplicateOk, const ring r);


intvec * id_QHomWeight(ideal id, const ring r);


#endif
