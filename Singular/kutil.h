#ifndef KUTIL_H
#define KUTIL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
*/
#include "structs.h"
#include "mmemory.h"
#include <string.h>

#define setmax 16

typedef int* intset;

struct sTObject{
                 poly  p;
                 int ecart,length;
               };
struct sLObject{
                 poly  p;
                 poly  p1,p2; /*- the pair p comes from -*/
                 poly  lcm;   /*- the lcm of p1,p2 -*/
                 int ecart,length;
               };
typedef struct sTObject TObject;
typedef struct sLObject LObject;
typedef TObject * TSet;
typedef LObject * LSet;

//extern BOOLEAN interpt;
//extern BOOLEAN kActive;
//extern int cp,c3;
//extern BOOLEAN homog;
//extern BOOLEAN news;
//extern ideal Shdl;
//extern polyset S;
//extern int sl;
//extern intset ecartS;
//extern intset fromQ;
//extern TSet T;
//extern int tl,tmax;
//extern LSet L;
//extern int Ll,Lmax;
//extern BOOLEAN honey,sugarCrit;
//extern BOOLEAN * NotUsedAxis;
//extern void (*red)(LObject * L,kStrategy strat);
//extern void (*initEcart)(LObject * L);
//extern int (*posInT)(TSet T,int tl,LObject h);
//extern int (*posInL)(LSet set, int length, LObject L,kStrategy strat);
//extern void (*enterS)(LObject h, int pos,kStrategy strat);
//extern void (*initEcartPair)(LObject * h, poly f, poly g, int ecartF, int ecartG);
//extern int ak;
extern int HCord;
//extern LObject P;
//extern poly tail;
//extern BOOLEAN fromT;
//extern sleftv kIdeal;

class skStrategy;
typedef skStrategy * kStrategy;
class skStrategy
{
  public:
    kStrategy next;
    void (*red)(LObject * L,kStrategy strat);
    void (*initEcart)(LObject * L);
    int (*posInT)(TSet T,int tl,LObject h);
    int (*posInL)(LSet set, int length, LObject L,kStrategy strat);
    void (*enterS)(LObject h, int pos,kStrategy strat);
    void (*initEcartPair)(LObject * h, poly f, poly g, int ecartF, int ecartG);
    int (*posInLOld)(LSet L,int Ll, LObject l,kStrategy strat);
    pFDegProc pOldFDeg;
    ideal Shdl;
    ideal D; /*V(S) is in D(D)*/
    ideal M; /*set of minimal generators*/
    polyset S;
    intset ecartS;
    intset fromQ;
    TSet T;
    LSet L;
    LSet    B;
    poly    kHEdge;
    poly    kNoether;
    BOOLEAN * NotUsedAxis;
    LObject P;
    poly tail;
    leftv kIdeal;
    intvec * kModW;
    BOOLEAN *pairtest;/*used for enterOnePair*/
    int cp,c3;
    int sl,mu;
    int tl,tmax;
    int Ll,Lmax;
    int Bl,Bmax;
    int ak,LazyDegree,LazyPass;
    int syzComp;
    int HCord;
    int lastAxis;
    int newIdeal;
    BOOLEAN interpt;
    BOOLEAN homog;
    BOOLEAN news;
    BOOLEAN newt;/*used for messageSets*/
    BOOLEAN kHEdgeFound;
    BOOLEAN honey,sugarCrit;
    BOOLEAN Gebauer,noTailReduction;
    BOOLEAN fromT;
    BOOLEAN noetherSet;
    BOOLEAN update;
    BOOLEAN posInLOldFlag;
           /*FALSE, if posInL == posInL10*/
    int minim;
};

void deleteHC(poly *p, int *e, int *l, kStrategy strat);
void deleteInS (int i,kStrategy strat);
void enlargeT (TSet* T,int* length,int incr);
void cleanT (kStrategy strat);
LSet initL ();
void enlargeL (LSet* L,int* length,int incr);
void deleteInL(LSet set, int *length, int j,kStrategy strat);
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at);
void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
int posInS (polyset set,int length,poly p);
int posInT0 (TSet set,int length,LObject p);
int posInT1 (TSet set,int length,LObject p);
int posInT2 (TSet set,int length,LObject p);
int posInT11 (TSet set,int length,LObject p);
int posInT13 (TSet set,int length,LObject p);
int posInT15 (TSet set,int length,LObject p);
int posInT17 (TSet set,int length,LObject p);
int posInT19 (TSet set,int length,LObject p);
void reorderS (int* suc,kStrategy strat);
int posInL0 (LSet set, int length, LObject L,kStrategy strat);
int posInL11 (LSet set, int length, LObject L,kStrategy strat);
int posInL13 (LSet set, int length, LObject L,kStrategy strat);
int posInL15 (LSet set, int length, LObject L,kStrategy strat);
int posInL17 (LSet set, int length, LObject L,kStrategy strat);
poly redtailBba (poly p,int pos,kStrategy strat);
poly redtailSyz (poly p,int pos,kStrategy strat);
poly redtail (poly p,int pos,kStrategy strat);
void enterpairs (poly h, int k, int ec, int pos,kStrategy strat);
void entersets (LObject h);
void pairs ();
void message (int i,int* reduc,int* olddeg,kStrategy strat);
void messageStat (int srmax,int lrmax,int hilbcount,kStrategy strat);
void messageSets (kStrategy strat);
void initEcartNormal (LObject* h);
void initEcartBBA (LObject* h);
void initS (ideal F, ideal Q,kStrategy strat);
void initSL (ideal F, ideal Q,kStrategy strat);
void updateS(BOOLEAN toT,kStrategy strat);
void enterSBba (LObject p, int pos,kStrategy strat);
void enterT (LObject p,kStrategy strat);
void enterTBba (LObject p, int pos,kStrategy strat);
void cancelunit (LObject* p);
void HEckeTest (poly pp,kStrategy strat);
void redtailS (poly* h,int maxIndex);
void redtailMora (poly* h,int maxIndex);
void initBuchMoraCrit(kStrategy strat);
void initHilbCrit(ideal F, ideal Q, intvec **hilb,kStrategy strat);
void initBuchMoraPos(kStrategy strat);
void initBuchMora (ideal F, ideal Q,kStrategy strat);
void exitBuchMora (kStrategy strat);
void updateResult(ideal r,ideal Q,kStrategy strat);
void completeReduce (kStrategy strat);
BOOLEAN homogTest(polyset F, int Fmax);
BOOLEAN newHEdge(polyset S, int ak,kStrategy strat);

inline TSet initT () { return (TSet)Alloc(setmax*sizeof(TObject)); }
#ifdef KDEBUG
#define kTest(A) K_Test(__FILE__,__LINE__,A)
void K_Test(char *f, int i,kStrategy strat);
#else
#define kTest(A)
#endif
#endif
