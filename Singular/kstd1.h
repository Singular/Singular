#ifndef KSTD1_H
#define KSTD1_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd1.h,v 1.3 1997-04-09 12:19:52 Singular Exp $ */
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

poly kNF1(ideal F, ideal Q, poly q, kStrategy strat, int lazyReduce);
ideal kNF1 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce);

poly kNF (ideal F, ideal Q, poly p,int syzComp=0, int lazyReduce=0);
ideal kNF(ideal F, ideal Q, ideal p,int syzComp=0, int lazyReduce=0);

ideal std(ideal F, ideal Q, tHomog h, intvec ** w,intvec *hilb=NULL,
          int syzComp=0,int newIdeal=0);

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
int   kModDeg(poly p);

ideal stdred(ideal F, ideal Q, tHomog h,intvec ** w);

lists min_std(ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb=NULL,
              int syzComp=0,int reduced=0);


extern pFDegProc pOldFDeg;
extern intvec * kModW;

/* options:
0 prot
1 redSB
2 Gebauer/Moeller
3 notSugar
4 interrupt
5 sugarCrit
6 teach
7 cancel unit
8 morePairs
9 return SB (syz,quotient,intersect)
10 fastHC
11-19 sort in L/T
20 redBest
21 keep local variable with keepring of a loacal ring
22 staircaseBound: in NF create a HC x1^degBound+1
23 multBound
24 degBound
25 no redTail(p)/redTail(s)
26 integer strategy
27 stop at HC (finiteDeterminacyTest)
28 minimizes directly during the mres command
29 std + 1 new element
30 noRedSyz
31 weight
*/

#endif

