#ifndef IDEALS_H
#define IDEALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/
// #include <kernel/structs.h>

//typedef struct sip_sideal *        ideal;
//typedef struct sip_smap *          map;
typedef ideal *            resolvente;


extern omBin sip_sideal_bin;

/*- creates an ideal -*/
ideal idInit (int size, int rank=1);
ideal idCopyFirstK (const ideal ide, const int k);

/// delete an ideal
// #define idDelete(h) id_Delete(h, currRing)
void id_Delete (ideal* h, ring r);
void id_ShallowDelete (ideal* h, ring r);
/*- initialise an ideal -*/ // ?

/// initialise the maximal ideal (at 0)
ideal idMaxIdeal (int deg);

/// gives an ideal the minimal possible size
void idSkipZeroes (ideal ide);

/// index of generator with leading term in ground ring (if any); otherwise -1
int idPosConstant (ideal id);

/// Count the effective size of an ideal
/// (without the trailing allocated zero-elements)
static inline int idSize(const ideal id)
{
  int j = IDELEMS(id) - 1;
  poly* mm = id->m;
  while ((j >= 0) && (mm[j] == NULL)) j--;
  return (j + 1); 
}

void idNorm(ideal id);
void idDelMultiples(ideal id);
void idDelEquals(ideal id);
void idDelLmEquals(ideal id);
void idDelDiv(ideal id);
BOOLEAN idIsConstant(ideal id);

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
ideal idDBCopy(ideal h1,const char *f,int l,const ring r);
#define id_DBCopy(A,r) idDBCopy(A,__FILE__,__LINE__,r)
#define idCopy(A,r) id_DBCopy(A,r)
#else
#define idCopy(A,r) id_Copy(A,r)
#endif


  /*adds two ideals without simplifying the result*/
ideal idSimpleAdd (ideal h1,ideal h2);
  /*adds the quotient ideal*/
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
int pLowVar (poly p);
  /*-the minimal index of used variables - 1-*/
void pShift (poly * p,int i);
  /*- verschiebt die Indizes der Modulerzeugenden um i -*/
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

poly idMinor(matrix a, int ar, unsigned long which, ideal R=NULL);

ideal   idMinors(matrix a, int ar, ideal R=NULL);

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
int idMinDegW(ideal M,intvec *w);
ideal   idSeries(int n,ideal M,matrix U=NULL,intvec *w=NULL);

BOOLEAN idIsZeroDim(ideal i);
matrix  idDiff(matrix i, int k);
matrix  idDiffOp(ideal I, ideal J,BOOLEAN multiply=TRUE);

intvec *idSort(ideal id,BOOLEAN nolex=TRUE);
ideal   idModulo (ideal h1,ideal h2, tHomog h=testHomog, intvec ** w=NULL);
int     idElem(const ideal F);
matrix  idCoeffOfKBase(ideal arg, ideal kbase, poly how);
// transpose a module
ideal   idTransp(ideal a);
// version of "ideal idTransp(ideal)" which works within a given ring.
ideal id_Transp(ideal a, const ring rRing);

intvec *idQHomWeight(ideal id);

void    idNormalize(ideal id);

ideal idXXX (ideal  h1, int k);

poly id_GCD(poly f, poly g, const ring r);

ideal idChineseRemainder(ideal *x, number *q, int rl);
//ideal idChineseRemainder(ideal *x, intvec *iv); /* currently unused */
ideal idFarey(ideal x, number N);

ideal id_TensorModuleMult(const int m, const ideal M, const ring rRing); // image of certain map for BGG

#ifdef PDEBUG
/* Shows an ideal -- only for debugging */
void idShow(const ideal id, const ring lmRing, const ring tailRing, const int debugPrint = 0);
#else
#define idShow(id, lmRing, tailRing, debugPrint) ((void)0)
#endif
#endif
