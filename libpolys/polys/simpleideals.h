#ifndef SIMPLEIDEALS_H
#define SIMPLEIDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - all basic methods to manipulate ideals
*/
#include "polys/monomials/ring.h"
#include "polys/matpol.h"

/// The following sip_sideal structure has many different uses
/// thoughout Singular. Basic use-cases for it are:
/// * ideal/module: nrows = 1, ncols >=0 and rank:1 for ideals, rank>=0 for modules
/// * matrix: nrows, ncols >=0, rank == nrows! see mp_* procedures
/// NOTE: the m member point to memory chunk of size (ncols*nrows*sizeof(poly)) or is NULL
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
/* the settings of rank, nrows, ncols, m ,   entries:
 * for IDEAL_CMD:     1    1    n    size n   poly              (n>=0)
 * for MODUL_CMD:     r    1    n    size n   vector of rank<=r (n>=0, r>=0)
 * for MATRIX_CMD     r    r    c    size r*c poly              (r>=0, c>=0)
 * for MAP_CMD:    char*   1    n    size n   poly              (n>=0)
 */

struct sip_smap
{
  poly *m;
  char *preimage;
  int nrows;
  int ncols;
};

//typedef struct sip_smap *         map;

struct sideal_list;
typedef struct sideal_list *      ideal_list;

struct sideal_list
{
  ideal_list next;
  ideal      d;
#ifdef KDEBUG
  int nr;
#endif
};

THREAD_VAR extern omBin sip_sideal_bin;

/// creates an ideal / module
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
void id_DBTest(ideal h1, int level, const char *f,const int l, const ring lR, const ring tR );
#define id_TestTail(A, lR, tR) id_DBTest(A, PDEBUG, __FILE__,__LINE__, lR, tR)
#define id_Test(A, lR) id_DBTest(A, PDEBUG, __FILE__,__LINE__, lR, lR)
#else
#define id_TestTail(A, lR, tR)  do {} while (0)
#define id_Test(A, lR) do {} while (0)
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

/// sorts the ideal w.r.t. the actual ringordering
/// uses lex-ordering when nolex = FALSE
intvec *id_Sort(const ideal id, const BOOLEAN nolex, const ring r);

/// transpose a module
ideal id_Transp(ideal a, const ring rRing);

void id_Compactify(ideal id, const ring r);
ideal  id_Mult (ideal h1,ideal  h2, const ring r);
ideal id_Homogen(ideal h, int varnum,const ring r);
BOOLEAN id_HomIdeal (ideal id, ideal Q, const ring r);
BOOLEAN id_HomModule(ideal m, ideal Q, intvec **w, const ring R);
BOOLEAN id_IsZeroDim(ideal I, const ring r);
ideal id_Jet(const ideal i,int d, const ring R);
ideal id_JetW(const ideal i,int d, intvec * iv, const ring R);
ideal  id_Subst(ideal id, int n, poly e, const ring r);
matrix id_Module2Matrix(ideal mod, const ring R);
matrix id_Module2formatedMatrix(ideal mod,int rows, int cols, const ring R);
ideal id_ResizeModule(ideal mod,int rows, int cols, const ring R);
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
#define id_Print(id, lR, tR) idShow(id, lR, tR)
#else
#define id_Print(A, lR, tR) do {} while (0)
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


ideal id_ChineseRemainder(ideal *xx, number *q, int rl, const ring r);

void id_Shift(ideal M, int s, const ring r);
ideal id_Delete_Pos(const ideal I, const int pos, const ring r);

/// for julia: convert an array of poly to vector
poly id_Array2Vector(poly *m, unsigned n, const ring R);
#endif
