/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: pcv.h,v 1.2 1998-11-18 13:00:12 mschulze Exp $ */
/*
* ABSTRACT: conversion between polys and coeff vectors
*/

#ifndef PCV_H
#define PCV_H

int pcvDegW(poly p,short w[]);
int pcvOrdW(poly p,short w[]);
void pcvInit(int d,short w[]);
void pcvInitW(int d,short w[]);
void pcvClean();
int pcvMon2Num(poly m);
poly pcvNum2Mon(int n);
poly pcvPoly2Vec(poly p,int d0,int d1,short w[]);
poly pcvVec2Poly(poly v,int d0,int d1,short w[]);
ideal pcvId2Mod(ideal I,int d0,int d1,short w[]);
ideal pcvMod2Id(ideal M,int d0,int d1,short w[]);
int pcvDimW(int d0,int d1,short w[]);
int pcvDegBasisW(ideal I,int i,poly m,int d,int j,short w[]);
ideal pcvBasisW(int d0,int d1,short w[]);

// interface to interpreter
BOOLEAN iiPcvConv(leftv res, leftv h);
BOOLEAN iiPcvDim(leftv res, leftv h);
BOOLEAN iiPcvBasis(leftv res, leftv h);
BOOLEAN iiPcvOrd(leftv res, leftv h);

#endif
