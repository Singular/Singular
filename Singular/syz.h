#ifndef SYZ_H
#define SYZ_h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.h,v 1.4 1997-04-09 12:20:16 Singular Exp $ */
/*
* ABSTRACT: resolutions
*/
#include "structs.h"

struct sSObject{
                 poly  p;
                 poly  p1,p2; /*- the pair p comes from -*/
                 poly  lcm;   /*- the lcm of p1,p2 -*/
                 poly  syz;   /*- the syzygy associated to p1,p2 -*/
                 int   ind1,ind2; /*- the indeces of p1,p2 -*/
                 int isNotMinimal;
                 int order;
               };
typedef struct sSObject SObject;
typedef SObject * SSet;
typedef SSet * SRes;

void sySchreyersSyzygiesM(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

void sySchreyersSyzygiesB(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

resolvente sySchreyerResolvente(ideal arg, int maxlength, int * length,
   BOOLEAN isMonomial=FALSE, BOOLEAN notReplace=FALSE);

resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim);

resolvente syMinRes(ideal arg, int maxlength, int * length, BOOLEAN minim);

void syMinimizeResolvente(resolvente res, int length, int first);

intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights=NULL);

ideal syMinBase(ideal arg);

BOOLEAN syTestOrder(ideal i);

void syReOrderResolventFB(resolvente res,int length, int initial=1);

resolvente syLaScala(ideal arg,int * length);
resolvente syLaScala1(ideal arg,int * length);

#endif
