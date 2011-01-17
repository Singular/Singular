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
void idDBTest(ideal h1, int level, const char *f,const int l);
#define idTest(A) idDBTest(A, PDEBUG, __FILE__,__LINE__)
#define idPrint(id) idShow(id)
#else
#define idTest(A)  (TRUE)
#define idPrint(A) ((void)0)
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
#endif
