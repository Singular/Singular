#ifndef SYZ_H
#define SYZ_h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.h,v 1.14 1999-06-08 07:57:13 Singular Exp $ */
/*
* ABSTRACT: Resolutions
*/
#include "structs.h"

struct sSObject{
                 poly  p;
                 poly  p1,p2; /*- the pair p comes from -*/
                 poly  lcm;   /*- the lcm of p1,p2 -*/
                 poly  syz;   /*- the syzygy associated to p1,p2 -*/
                 int   ind1,ind2; /*- the indeces of p1,p2 -*/
                 poly  isNotMinimal;
                 int   syzind;
                 int   order;
               };
typedef struct sSObject SObject;
typedef SObject * SSet;
typedef SSet * SRes;

class ssyStrategy;
typedef ssyStrategy * syStrategy;
class ssyStrategy{
  public:
  int ** truecomponents;
  int ** backcomponents;
  int ** Howmuch;
  int ** Firstelem;
  intvec ** weights;
  resolvente res;
  resolvente orderedRes;
  SRes resPairs;
  intvec * Tl;
  resolvente fullres;
  resolvente minres;
  int * binom;
  intvec * resolution;
  intvec * cw;
  int highdeg_1;
  int length;
  short list_length;
  short references;
};

void sySchreyersSyzygiesM(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

void sySchreyersSyzygiesB(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

resolvente sySchreyerResolvente(ideal arg, int maxlength, int * length,
   BOOLEAN isMonomial=FALSE, BOOLEAN notReplace=FALSE);

syStrategy sySchreyer(ideal arg, int maxlength);

resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim);

syStrategy syResolution(ideal arg, int maxlength,intvec * w, BOOLEAN minim);

resolvente syMinRes(ideal arg, int maxlength, int * length, BOOLEAN minim);

void syMinimizeResolvente(resolvente res, int length, int first);

resolvente syFastMin(resolvente res,int length);

intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights=NULL);

ideal syMinBase(ideal arg);

BOOLEAN syTestOrder(ideal i);

void syReOrderResolventFB(resolvente res,int length, int initial=1);

resolvente syLaScala1(ideal arg,int * length);
syStrategy syLaScala3(ideal arg,int * length);

void syKillComputation(syStrategy syzstr);
intvec * syBettiOfComputation(syStrategy syzstr);
int syLength(syStrategy syzstr);
int sySize(syStrategy syzstr);
int syDim(syStrategy syzstr);
syStrategy syCopy(syStrategy syzstr);
void syPrint(syStrategy syzstr);
lists syConvRes(syStrategy syzstr,BOOLEAN toDel=FALSE);
syStrategy syConvList(lists li,BOOLEAN toDel=FALSE);
syStrategy syForceMin(lists li);
syStrategy syMinimize(syStrategy syzstr);
void syKillEmptyEntres(resolvente res,int length);

#endif
