/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: pcv.h,v 1.9 1998-12-17 14:25:16 mschulze Exp $ */
/*
* ABSTRACT: conversion between polys and coeff vectors
*/

#ifndef PCV_H
#define PCV_H

int pcvDeg(poly p);
int pcvMinDeg(poly p);
int pcvMaxDeg(poly p);
BOOLEAN pcvMinDeg(leftv res,leftv h);
BOOLEAN pcvMaxDeg(leftv res,leftv h);
void pcvInit(int d);
void pcvClean();
poly pcvP2CV(poly p,int d0,int d1);
poly pcvCV2P(poly cv,int d0,int d1);
lists pcvP2CV(lists pl,int d0,int d1);
lists pcvCV2P(lists cvl,int d0,int d1);
BOOLEAN pcvP2CV(leftv res,leftv h);
BOOLEAN pcvCV2P(leftv res,leftv h);
int pcvDim(int d0,int d1);
BOOLEAN pcvDim(leftv res,leftv h);
int pcvBasis(lists b,int i,poly m,int d,int n);
lists pcvBasis(int d0,int d1);
BOOLEAN pcvBasis(leftv res,leftv h);

#endif
