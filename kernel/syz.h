#ifndef SYZ_H
#define SYZ_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: Resolutions
*/
#include <kernel/structs.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>

// Logarithm of estimate of maximal number of new components
#define SYZ_SHIFT_MAX_NEW_COMP_ESTIMATE 8
// Logarithm of "distance" between a new component and prev component
#define SYZ_SHIFT_BASE_LOG (BIT_SIZEOF_LONG - 1 - SYZ_SHIFT_MAX_NEW_COMP_ESTIMATE)
#define SYZ_SHIFT_BASE (((long)1) << SYZ_SHIFT_BASE_LOG)
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
  int regularity;
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

resolvente syReorder(resolvente res,int length,
        syStrategy syzstr,BOOLEAN toCopy/*=TRUE*/,resolvente totake/*=NULL*/);

syStrategy syResolution(ideal arg, int maxlength,intvec * w, BOOLEAN minim);

void syMinimizeResolvente(resolvente res, int length, int first);

intvec * syBetti(resolvente res,int length, int * regularity,
         intvec* weights=NULL,BOOLEAN tomin=TRUE, int * row_shift=NULL);

ideal syMinBase(ideal arg);

BOOLEAN syTestOrder(ideal i);

void syReOrderResolventFB(resolvente res,int length, int initial=1);

resolvente syLaScala1(ideal arg,int * length);
syStrategy syLaScala3(ideal arg,int * length);

syStrategy syLaScala(ideal arg, int& maxlength, intvec* weights = NULL);

syStrategy syHilb(ideal arg,int * length);
syStrategy syKosz(ideal arg,int * length);

void syKillComputation(syStrategy syzstr, ring r=currRing);
intvec * syBettiOfComputation(syStrategy syzstr, BOOLEAN minim=TRUE,int * row_shift=NULL, intvec *weights=NULL);
BOOLEAN syBetti1(leftv res, leftv u);
BOOLEAN syBetti2(leftv res, leftv u, leftv w);
int sySize(syStrategy syzstr);
int syDim(syStrategy syzstr);
syStrategy syCopy(syStrategy syzstr);
void syPrint(syStrategy syzstr);
lists syConvRes(syStrategy syzstr,BOOLEAN toDel=FALSE,int add_row_shift=0);
syStrategy syConvList(lists li,BOOLEAN toDel=FALSE);
syStrategy syForceMin(lists li);
syStrategy syMinimize(syStrategy syzstr);
void syKillEmptyEntres(resolvente res,int length);

extern int *  currcomponents;
extern long *  currShiftedComponents;

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
void syGaussForOne(ideal arg,int gen,int ModComp,int from=-1,int till=0);
void syEnterPair(SSet sPairs, SObject * so, int * sPlength,int index);
void syEnterPair(syStrategy syzstr, SObject * so, int * sPlength,int index);
syStrategy syKosz(ideal arg,int * length);

resolvente syReorder(resolvente res,int length,
        syStrategy syzstr,BOOLEAN toCopy=TRUE,resolvente totake=NULL);

#endif
