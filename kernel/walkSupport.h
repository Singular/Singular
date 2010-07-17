#ifndef WALKSUPPORT_H
#define WALKSUPPORT_H

#include <kernel/intvec.h>
#include <kernel/int64vec.h>

int tdeg(poly p);
int getMaxTdeg(ideal I);
int getMaxPosOfNthRow(intvec *v,int n);
int64 getInvEps64(ideal G,intvec *targm,int pertdeg);
int invEpsOk64(ideal I, intvec *targm, int pertdeg, int64 inveps64);
intvec* getNthRow(intvec *v, int n); 
int64vec* getNthRow64(intvec *v, int n); 
//int64vec* gett64(int64vec* listw, int64vec* currw, int64vec* targw);
void gett64(int64vec* listw, int64vec* currw, int64vec* targw, int64 &t1, int64 &t2);
void nextt64(ideal G, int64vec* currw, int64vec* targw, int64 &t1, int64 &t2);
int64vec* nextw64(int64vec* currw, int64vec* targw, int64 nexttvec0, int64 nexttvec1);
int DIFFspy(ideal G);
intvec* DIFF(ideal G);
ideal init64(ideal G,int64vec* currw);
BOOLEAN currwOnBorder64(ideal I, int64vec* currw64);
void getTaun64(ideal G,intvec* targm,int pertdeg, int64vec** v64, int64 & i64);
int64vec* getiv64(lists l);
//int64 getint64(lists l);// not used


//functions not originating from the oroginal SINGULAR implementation
ideal idStd(ideal G);
ideal idInterRed(ideal G);
matrix matIdLift(ideal Gomega, ideal M);
void rCopyAndChangeA(int64vec* w);
ring rCopy0AndAddA(ring r, int64vec *wv64, BOOLEAN copy_qideal = TRUE,
                   BOOLEAN copy_ordering = TRUE);
int64vec* rGetGlobalOrderMatrix(ring r);
int64vec* rGetGlobalOrderWeightVec(ring r);
BOOLEAN noPolysWithMoreThanTwoTerms(ideal Gw);
#define idealSize(I) IDELEMS(I)
inline int ivSize(intvec* v){ return((v->rows())*(v->cols())); }
inline int iv64Size(int64vec* v){ return((v->rows())*(v->cols())); }
intvec* leadExp(poly p);
int64vec* leadExp64(poly p);
void setPosOfIM(intvec* im,int i,int j,int val);
poly getNthPolyOfId(ideal I,int n);
int gcd(int a, int b);
int64 gcd64(int64 a, int64 b);
inline int64 abs64(int64 i) { return ABS(i); }
static inline long  scalarProduct(intvec* a, intvec* b);
static inline int64  scalarProduct64(int64vec* a, int64vec* b);
ideal sortRedSB(ideal G);
intvec* int64VecToIntVec(int64vec* source);
int64vec* rGetGlobalOrderWeightVec(ring r);

#endif
