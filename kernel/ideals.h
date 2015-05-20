#ifndef IDEALS_H
#define IDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - all basic methods to manipulate ideals
*/

#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>
#include <polys/simpleideals.h>

extern ring currRing;

#include <kernel/structs.h> // for tHomog

//typedef struct sip_sideal *        ideal;
//typedef struct sip_smap *          map;
typedef ideal *            resolvente;

inline ideal idCopyFirstK (const ideal ide, const int k)
{
  return id_CopyFirstK(ide, k, currRing);
}

void idKeepFirstK(ideal ide, const int k);
void idDelEquals(ideal id);

/// delete an ideal
inline void idDelete (ideal* h)
{
  id_Delete(h, currRing);
}

/// initialise the maximal ideal (at 0)
//ideal id_MaxIdeal(int deg, const ring r);
#define idMaxIdeal(D) id_MaxIdeal(D,currRing)

/// index of generator with leading term in ground ring (if any); otherwise -1
//int id_PosConstant(ideal id, const ring r)
#define idPosConstant(I) id_PosConstant(I,currRing)

/// Count the effective size of an ideal
/// (without the trailing allocated zero-elements)
static inline int idSize(const ideal id)
{
  int j = IDELEMS(id) - 1;
  poly* mm = id->m;
  while ((j >= 0) && (mm[j] == NULL)) j--;
  return (j + 1);
}


//BOOLEAN id_IsConstant(ideal id, const ring r);
#define idIsConstant(I) id_IsConstant(I,currRing)

#define idSimpleAdd(A,B) id_SimpleAdd(A,B,currRing)

ideal id_Copy (ideal h1, const ring r);

#define idPrint(id) id_Print(id, currRing, currRing)
#define idTest(id)  id_Test(id, currRing)

#if 0

// ifdef PDEBUG // Sorry: the following was lost........ :((((((((
ideal idDBCopy(ideal h1,const char *f,int l,const ring r);
#define id_DBCopy(A,r) idDBCopy(A,__FILE__,__LINE__,r)

inline ideal idCopy(ideal A)
{
  return id_DBCopy(A,currRing); // well, just for now... ok? Macros can't  have default args values :(
}
#else
inline ideal idCopy(ideal A)
{
  return id_Copy(A, currRing);
}
#endif


/// h1 + h2
inline ideal idAdd (ideal h1, ideal h2)
{
  return id_Add(h1, h2, currRing);
}

BOOLEAN idInsertPoly (ideal h1,poly h2);  /* h1 + h2 */
inline BOOLEAN idInsertPolyWithTests (ideal h1, const int validEntries, const poly h2, const bool zeroOk, const bool duplicateOk)
{
  return id_InsertPolyWithTests (h1, validEntries, h2, zeroOk, duplicateOk, currRing);
}


/* h1 + h2 */

/// hh := h1 * h2
inline ideal idMult (ideal h1, ideal h2)
{
  return id_Mult(h1, h2, currRing);
}

BOOLEAN idIs0 (ideal h);

inline BOOLEAN idHomIdeal (ideal id, ideal Q=NULL)
{
  return id_HomIdeal(id, Q, currRing);
}

inline BOOLEAN idHomModule(ideal m, ideal Q,intvec **w)
{
   return id_HomModule(m, Q, w, currRing);
}

BOOLEAN idTestHomModule(ideal m, ideal Q, intvec *w);

ideal idMinBase (ideal h1);
  /*returns a minimized set of generators of h1*/
void    idInitChoise (int r,int beg,int end,BOOLEAN *endch,int * choise);
void    idGetNextChoise (int r,int end,BOOLEAN *endch,int * choise);
int     idGetNumberOfChoise(int t, int d, int begin, int end, int * choise);

int     binom (int n,int r);

inline ideal idFreeModule (int i)
{
  return id_FreeModule (i, currRing);
}


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

// ideal   idPower(ideal gid,int deg);

//ideal   idElimination (ideal h1,poly delVar);
ideal   idElimination (ideal h1,poly delVar, intvec *hilb=NULL);

#ifdef WITH_OLD_MINOR
poly idMinor(matrix a, int ar, unsigned long which, ideal R = NULL);
#endif
ideal   idMinors(matrix a, int ar, ideal R = NULL);

ideal idMinEmbedding(ideal arg,BOOLEAN inPlace=FALSE, intvec **w=NULL);

ideal   idHead(ideal h);

// ideal   idHomogen(ideal h, int varnum);

BOOLEAN idIsSubModule(ideal id1,ideal id2);

inline ideal idVec2Ideal(poly vec)
{
  return id_Vec2Ideal(vec, currRing);
}

ideal   idSeries(int n,ideal M,matrix U=NULL,intvec *w=NULL);

inline BOOLEAN idIsZeroDim(ideal i)
{
  return id_IsZeroDim(i, currRing);
}

matrix  idDiff(matrix i, int k);
matrix  idDiffOp(ideal I, ideal J,BOOLEAN multiply=TRUE);

inline intvec *idSort(ideal id,BOOLEAN nolex=TRUE)
{
  return id_Sort(id, nolex, currRing);
}

ideal   idModulo (ideal h1,ideal h2, tHomog h=testHomog, intvec ** w=NULL);
matrix  idCoeffOfKBase(ideal arg, ideal kbase, poly how);

/// transpose a module
inline ideal   idTransp(ideal a)
{
  return id_Transp(a, currRing);
}

// intvec *idQHomWeight(ideal id);

ideal idXXX (ideal  h1, int k);

poly id_GCD(poly f, poly g, const ring r);

ideal id_Farey(ideal x, number N, const ring r);

ideal id_TensorModuleMult(const int m, const ideal M, const ring rRing); // image of certain map for BGG


#endif
