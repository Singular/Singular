#ifndef KSTD1_H
#define KSTD1_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd1.h,v 1.2 2007-07-25 10:53:15 Singular Exp $ */
/*
* ABSTRACT
*/
#include "structs.h"

/*2
*input: F: a set (array) of polys with <= Fmax entries (0..Fmax-1)
*       *S: a set (array) of polys with *Smax initialized entries (0..*Smax-1)
*output:*S: the standard base: a set (array) with <= *Smax+1 entries (0..*Smax)
*/
ideal mora (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);

// lazy_reduce flags: can be combined by |
#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
#define KSTD_NF_ECART  2
  // only local: recude even with bad ecart
#define KSTD_NF_NONORM 4
  // only global: avoid normamlization, return a multiply of NF

poly kNF1(ideal F, ideal Q, poly q, kStrategy strat, int lazyReduce);
ideal kNF1 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce);

poly kNF (ideal F, ideal Q, poly p,int syzComp=0, int lazyReduce=0);
ideal kNF(ideal F, ideal Q, ideal p,int syzComp=0, int lazyReduce=0);

ideal kStd(ideal F, ideal Q, tHomog h, intvec ** mw,intvec *hilb=NULL,
          int syzComp=0,int newIdeal=0, intvec *vw=NULL);

/* the following global data are defined in kutil.cc */
//extern int syzComp;
  /*stop building pairs after that component --> ideals.cc, syz.cc */
extern int LazyPass,LazyDegree,mu,Kstd1_deg;
#define Kstd1_mu mu
  /*parameters for Lazy or global stops --> ipshell.cc, grammar.y*/
extern BITSET kOptions;
  /*the known test options (a constant)*/
extern BITSET validOpts;

void initMora(ideal F,kStrategy strat);

ideal kInterRed (ideal F, ideal Q=NULL);
long   kModDeg(poly p, ring r = currRing);
long  kHomModDeg(poly p, ring r = currRing);

ideal stdred(ideal F, ideal Q, tHomog h,intvec ** w);

ideal kMin_std(ideal F, ideal Q, tHomog h,intvec ** w, ideal &M,
              intvec *hilb=NULL, int syzComp=0,int reduced=0);


extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;
extern intvec * kModW;

/* options:
0 prot
1 redSB
2 Gebauer/Moeller: obachman 10/00: replaced by notBucket
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
21 keep local variable with keepring of a loacal ring
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

