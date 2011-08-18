#ifndef IDEALS_H
#define IDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/
#include <polys/simpleideals.h>
#include <polys/polys.h>
// #include <kernel/structs.h>

//typedef struct sip_sideal *        ideal;
//typedef struct sip_smap *          map;
typedef ideal *            resolvente;

extern ideal currQuotient;


ideal idCopyFirstK (const ideal ide, const int k);

/// delete an ideal
inline void idDelete (ideal* h, ring r = currRing) { id_Delete(h, r); } ;

/// initialise the maximal ideal (at 0)
//ideal id_MaxIdeal(int deg, const ring r); 
#define idMaxIdeal(D) id_MaxIdeal(D,currRing)

/// index of generator with leading term in ground ring (if any); otherwise -1
//int id_PosConstant(ideal id, const ring r)
#define idPosConstant(I) id_PosConstant(I,currRing)
//
/// Count the effective size of an ideal
/// (without the trailing allocated zero-elements)
static inline int idSize(const ideal id)
{
  int j = IDELEMS(id) - 1;
  poly* mm = id->m;
  while ((j >= 0) && (mm[j] == NULL)) j--;
  return (j + 1); 
}

//void id_Norm(ideal id, const ring r);
#define idNorm(I) id_Norm(I,currRing)

//void id_DelMultiples(ideal id, const ring r);
#define idDelMultiples(I) id_DelMultiples(I,currRing)

//void id_DelEquals(ideal id, const ring r);
#define idDelEquals(I) id_DelEquals(I,currRing)

//void id_DelLmEquals(ideal id, const ring r);
#define idDelLmEquals(I) id_DelLmEquals(I,currRing)

//void id_DelDiv(ideal id, const ring r);
#define idDelDiv(I) id_DelDiv(I,currRing)

//BOOLEAN id_IsConstant(ideal id, const ring r);
#define idIsConstant(I) id_IsConstant(I,currRing)

#define idSimpleAdd(A,B) id_SimpleAdd(A,B,currRing)

#ifdef PDEBUG
#define idTest(A) id_DBTest(A, PDEBUG, __FILE__,__LINE__,currRing)
#define idPrint(id) idShow(id, currRing, currRing)
#else
#define idTest(A)  (TRUE)
#define idPrint(A) ((void)0)
#endif

ideal id_Copy (ideal h1, const ring r);

#ifdef PDEBUG
ideal idDBCopy(ideal h1,const char *f,int l,const ring r);
#define id_DBCopy(A,r) idDBCopy(A,__FILE__,__LINE__,r)

inline ideal idCopy(ideal A, const ring R = currRing)
{
  return id_DBCopy(A,R); // well, just for now... ok? Macros can't  have default args values :(
}
#else
inline ideal idCopy(ideal A, const ring R = currRing)
{
  return id_Copy(A, R);
}
#endif


ideal idAdd (ideal h1,ideal h2);
  /* h1 + h2 */
BOOLEAN idInsertPoly (ideal h1,poly h2);
  /* h1 + h2 */
BOOLEAN idInsertPolyWithTests (ideal h1, const int validEntries,
  const poly h2, const bool zeroOk, const bool duplicateOk);
  /* h1 + h2 */
ideal idMult (ideal h1,ideal h2);
  /*hh := h1 * h2*/

BOOLEAN idIs0 (ideal h);

// returns TRUE, if idRankFreeModule(m) > 0
BOOLEAN idIsModule(ideal m, const ring r);
BOOLEAN idHomIdeal (ideal id, ideal Q=NULL);
BOOLEAN idHomModule(ideal m, ideal Q,intvec **w);
BOOLEAN idTestHomModule(ideal m, ideal Q, intvec *w);

ideal idMinBase (ideal h1);
  /*returns a minimized set of generators of h1*/
void    idInitChoise (int r,int beg,int end,BOOLEAN *endch,int * choise);
void    idGetNextChoise (int r,int end,BOOLEAN *endch,int * choise);
int     idGetNumberOfChoise(int t, int d, int begin, int end, int * choise);

int     binom (int n,int r);

ideal   idFreeModule (int i);

ideal   idSect (ideal h1,ideal h2);
ideal   idMultSect(resolvente arg, int length);

//ideal   idSyzygies (ideal h1, tHomog h,intvec **w);
ideal   idSyzygies (ideal h1, tHomog h,intvec **w, BOOLEAN setSyzComp=TRUE,
                    BOOLEAN setRegularity=FALSE, int *deg = NULL);
ideal   idLiftStd  (ideal h1, matrix *m, tHomog h=testHomog, ideal *syz=NULL);

ideal   idLift (ideal mod, ideal sumod,ideal * rest=NULL,
             BOOLEAN goodShape=FALSE, BOOLEAN isSB=TRUE,BOOLEAN divide=FALSE,
             matrix *unit=NULL);

void idLiftW(ideal P,ideal Q,int n,matrix &T, ideal &R, short *w= NULL );

intvec * idMWLift(ideal mod,intvec * weights);

ideal   idQuot (ideal h1,ideal h2,
                BOOLEAN h1IsStb=FALSE, BOOLEAN resultIsIdeal=FALSE);

ideal   idPower(ideal gid,int deg);

//ideal   idElimination (ideal h1,poly delVar);
ideal   idElimination (ideal h1,poly delVar, intvec *hilb=NULL);

poly idMinor(matrix a, int ar, unsigned long which, ideal R = NULL);
ideal   idMinors(matrix a, int ar, ideal R = NULL);

void   idCompactify(ideal id);

ideal idMinEmbedding(ideal arg,BOOLEAN inPlace=FALSE, intvec **w=NULL);

ideal   idHead(ideal h);

ideal   idHomogen(ideal h, int varnum);

BOOLEAN idIsSubModule(ideal id1,ideal id2);

ideal   idVec2Ideal(poly vec);

ideal   idMatrix2Module(matrix mat);

matrix  idModule2Matrix(ideal mod);

matrix  idModule2formatedMatrix(ideal mod,int rows, int cols);

ideal   idSubst(ideal i, int n, poly e);

ideal   idJet(ideal i,int d);
ideal   idJetW(ideal i,int d, intvec * iv);
ideal   idSeries(int n,ideal M,matrix U=NULL,intvec *w=NULL);

BOOLEAN idIsZeroDim(ideal i);
matrix  idDiff(matrix i, int k);
matrix  idDiffOp(ideal I, ideal J,BOOLEAN multiply=TRUE);

intvec *idSort(ideal id,BOOLEAN nolex=TRUE);
ideal   idModulo (ideal h1,ideal h2, tHomog h=testHomog, intvec ** w=NULL);
matrix  idCoeffOfKBase(ideal arg, ideal kbase, poly how);
// transpose a module
ideal   idTransp(ideal a);
// version of "ideal idTransp(ideal)" which works within a given ring.
ideal id_Transp(ideal a, const ring rRing);

intvec *idQHomWeight(ideal id);

ideal idXXX (ideal  h1, int k);

poly id_GCD(poly f, poly g, const ring r);

ideal id_ChineseRemainder(ideal *x, number *q, int rl, const ring R);
//ideal idChineseRemainder(ideal *x, intvec *iv); /* currently unused */
ideal id_Farey(ideal x, number N, const ring r);

ideal id_TensorModuleMult(const int m, const ideal M, const ring rRing); // image of certain map for BGG
#endif
