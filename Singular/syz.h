#ifndef SYZ_H
#define SYZ_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.h,v 1.21 1999-10-20 11:52:02 obachman Exp $ */
/*
* ABSTRACT: Resolutions
*/
#include "structs.h"
#if HAVE_ASO == 1
#include "syz.aso"
#endif

// Logarithm of estimate of maximal number of new components
#define SYZ_SHIFT_MAX_NEW_COMP_ESTIMATE 8
// Logarithm of "distance" between a new component and prev component
#define SYZ_SHIFT_BASE_LOG (BIT_SIZEOF_LONG - 1 - SYZ_SHIFT_MAX_NEW_COMP_ESTIMATE)
#define SYZ_SHIFT_BASE (1 << SYZ_SHIFT_BASE_LOG)
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
  intvec * betti;
  kBucket_pt bucket;
  kBucket_pt syz_bucket;
  ring syRing;
  resolvente fullres;
  resolvente minres;
  unsigned long ** sev;
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
#ifdef PDEBUG
int syzcomp2dpc(poly p1, poly p2);
#else
#define syzcomp2dpc rComp0
#endif

extern int *  currcomponents;
extern long *  currShiftedComponents;

int syzcomp1dpc(poly p1, poly p2);
void syDeletePair(SObject * so);
void syInitializePair(SObject * so);
void syCopyPair(SObject * argso, SObject * imso);
void syCompactifyPairSet(SSet sPairs, int sPlength, int first);
void syCompactify1(SSet sPairs, int* sPlength, int first);
SRes syInitRes(ideal arg,int * length, intvec * Tl, intvec * cw=NULL);
void syResetShiftedComponents(syStrategy syzstr, int index,int hilb=0);
void syEnlargeFields(syStrategy syzstr,int index);
void syEnterPair(syStrategy syzstr, SObject * so, int * sPlength,int index);
SSet syChosePairs(syStrategy syzstr, int *index, int *howmuch, int * actdeg);
int syInitSyzMod(syStrategy syzstr, int index, int init=17);
long syReorderShiftedComponents(long * sc, int n);

#endif
