#ifndef KSTD1_H
#define KSTD1_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include "kernel/structs.h"
#include "polys/monomials/ring.h"
#include "coeffs/bigintmat.h"

ideal mora (ideal F, ideal Q,intvec *w,bigintmat *hilb,kStrategy strat);

typedef BOOLEAN (*s_poly_proc_t)(kStrategy strat);

// lazy_reduce flags: can be combined by |
#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
#define KSTD_NF_ECART  2
  // only local: reduce even with bad ecart
#define KSTD_NF_NONORM 4
  // only global: avoid normalization, return a multiply of NF
#define KSTD_NF_NOLF   8
  // avoid PrintLn with OPT_PROT

poly kNF1(ideal F, ideal Q, poly q, kStrategy strat, int lazyReduce);
ideal kNF1 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce);

poly kNF (ideal F, ideal Q, poly p,int syzComp=0, int lazyReduce=0);
ideal kNF(ideal F, ideal Q, ideal p,int syzComp=0, int lazyReduce=0);

poly kNFBound (ideal F, ideal Q, poly p,int bound,int syzComp=0, int lazyReduce=0);
ideal kNFBound (ideal F, ideal Q, ideal p,int bound,int syzComp=0, int lazyReduce=0);
ideal idDivRem(ideal A, const ideal quot, ideal &factor,ideal *unit,int lazyReduce=0);

/// NOTE: this is just a wrapper which sets currRing for the actual kNF call
poly k_NF (ideal F, ideal Q, poly p,int syzComp, int lazyReduce, const ring _currRing);
ideal kSba(ideal F,ideal Q, tHomog h, intvec ** mw, int incremental=0, int arri=0, bigintmat *hilb=NULL,
          int syzComp=0,int newIdeal=0, intvec *vw=NULL);

ideal kStd(ideal F, ideal Q, tHomog h, intvec ** mw,bigintmat *hilb=NULL,
          int syzComp=0,int newIdeal=0, intvec *vw=NULL, s_poly_proc_t sp=NULL);

ideal kStd_internal(ideal F, ideal Q, tHomog h,intvec ** w, bigintmat *hilb=NULL,
         int syzComp=0, int newIdeal=0, intvec *vw=NULL, s_poly_proc_t sp=NULL);


ideal kStdShift(ideal F, ideal Q, tHomog h,intvec ** mw, bigintmat *hilb=NULL,
    int syzComp=0, int newIdeal=0, intvec *vw=NULL, BOOLEAN rightGB=FALSE);

ideal kTryHilbstd(ideal F, ideal Q);
ideal kTryHilbstd_par(ideal F, ideal Q, tHomog h, intvec ** mw);
poly kTryHC(ideal F, ideal Q);

ideal rightgb(ideal F,const ideal Q);

/* the following global data are defined in kutil.cc */
//extern int syzComp;
  /*stop building pairs after that component --> ideals.cc, syz.cc */
EXTERN_VAR int Kstd1_mu,Kstd1_deg;
  /*parameters for global stops --> ipshell.cc, grammar.y*/
EXTERN_VAR BITSET kOptions;
  /*the known test options (a constant)*/
EXTERN_VAR BITSET validOpts;

void initMora(ideal F,kStrategy strat);

ideal kInterRed (ideal F, const ideal Q=NULL);
ideal kInterRedOld (ideal F, const ideal Q=NULL);
ideal kInterRedBba (ideal F, ideal Q, int &need_retry);
long   kModDeg(poly p, const ring r = currRing);
long  kHomModDeg(poly p, const ring r = currRing);

ideal stdred(ideal F, ideal Q, tHomog h,intvec ** w);

ideal kMin_std(ideal F, ideal Q, tHomog h,intvec ** w, ideal &M,
              bigintmat *hilb=NULL, int syzComp=0,int reduced=0);

BOOLEAN kVerify(ideal F,ideal Q);

EXTERN_VAR intvec * kModW;
EXTERN_VAR intvec * kHomW;


/* options:
0 prot
1 redSB
2 notBucket
3 notSugar
4 interrupt
5 sugarCrit
6 teach
7 cancel unit: obachman 11/00 tossed
8 morepairs: obachman 11/00: tossed
9 return SB (syz,quotient,intersect)
10 fastHC
11-19 sort in L/T
20 redBest: obachman 11/00 tossed

22 staircaseBound: in NF create a HC x1^degBound+1
23 multBound
24 degBound
25 no redTail(p)/redTail(s)
26 integer strategy
27 stop at HC (finiteDeterminacyTest)
28 infRedTail: ignore ecart in local redTail-calls
29 kStd + 1 new element
30 noRedSyz
31 weight
verbose:31 stop at certain weights
*/

#endif

