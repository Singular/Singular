#ifndef SYZ_H
#define SYZ_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.h,v 1.19 1999-10-14 14:27:34 obachman Exp $ */
/*
* ABSTRACT: Resolutions
*/
#include "structs.h"
#if HAVE_ASO == 1
#include "syz.aso"
#endif

struct sSObject{
                 poly  p;
                 poly  p1,p2; /*- the pair p comes from -*/
                 poly  lcm;   /*- the lcm of p1,p2 -*/
                 poly  syz;   /*- the syzygy associated to p1,p2 -*/
                 int   ind1,ind2; /*- the indeces of p1,p2 -*/
                 poly  isNotMinimal;
                 int   syzind;
                 int   order;
                 int   length;
                 int   reference;
               };
typedef struct sSObject SObject;
typedef SObject * SSet;
typedef SSet * SRes;

class ssyStrategy;
typedef ssyStrategy * syStrategy;
class ssyStrategy{
  public:
  int ** truecomponents;
  long** ShiftedComponents;
  int ** backcomponents;
  int ** Howmuch;
  int ** Firstelem;
  int ** elemLength;
  intvec ** weights;
  intvec ** hilb_coeffs;
  resolvente res;              //polynomial data for internal use only
  resolvente orderedRes;       //polynomial data for internal use only
  SRes resPairs;               //polynomial data for internal use only
  intvec * Tl;
  intvec * resolution;
  intvec * cw;
  short list_length;
  short references;
  kBucket_pt bucket;
  kBucket_pt syz_bucket;
  ring syRing;
  int length;
  resolvente fullres;
  resolvente minres;
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

intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights=NULL,BOOLEAN tomin=TRUE);

ideal syMinBase(ideal arg);

BOOLEAN syTestOrder(ideal i);

void syReOrderResolventFB(resolvente res,int length, int initial=1);

resolvente syLaScala1(ideal arg,int * length);
syStrategy syLaScala3(ideal arg,int * length);
syStrategy syHilb(ideal arg,int * length);
syStrategy syKosz(ideal arg,int * length);

void syDeleteRes(resolvente * res,int length);
void syKillComputation(syStrategy syzstr);
intvec * syBettiOfComputation(syStrategy syzstr, BOOLEAN minim=TRUE);
BOOLEAN syBetti1(leftv res, leftv u);
BOOLEAN syBetti2(leftv res, leftv u, leftv w);
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

extern int *  currcomponents;
extern long *  currShiftedComponents;

#endif
