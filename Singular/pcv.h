/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: pcv.h,v 1.6 1998-11-25 10:27:49 mschulze Exp $ */
/*
* ABSTRACT: conversion between polys and coeff vectors
*/

#ifndef PCV_H
#define PCV_H

int pcvDeg(poly p);
int pcvOrd(poly p);
int pcvOrd(matrix m);
BOOLEAN pcvOrd(leftv res,leftv h);
void pcvInit(int d);
void pcvClean();
int pcvM2n(poly m);
poly pcvN2m(int n);
poly pcvP2cv(poly p,int d0,int d1);
poly pcvCv2p(poly cv,int d0,int d1);
lists pcvP2cv(lists pl,int d0,int d1);
lists pcvCv2p(lists cvl,int d0,int d1);
BOOLEAN pcvP2cv(leftv res,leftv h);
BOOLEAN pcvCv2p(leftv res,leftv h);
int pcvDim(int d0,int d1);
BOOLEAN pcvDim(leftv res,leftv h);
int pcvBasis(lists b,int i,poly m,int d,int n);
lists pcvBasis(int d0,int d1);
BOOLEAN pcvBasis(leftv res,leftv h);

#endif
