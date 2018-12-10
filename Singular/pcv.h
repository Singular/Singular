/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: conversion between polys and coef vectors


*/

#ifndef PCV_H
#define PCV_H

lists pcvLAddL(lists l1,lists l2);
lists pcvPMulL(poly p,lists l1);
BOOLEAN pcvLAddL(leftv res,leftv h);
BOOLEAN pcvPMulL(leftv res,leftv h);
int pcvDeg(poly p);
int pcvMinDeg(poly p);
int pcvMinDeg(matrix m);
BOOLEAN pcvMinDeg(leftv res,leftv h);
void pcvInit(int d);
void pcvClean();
int pcvM2N(poly m);
poly pcvN2M(int n);
poly pcvP2CV(poly p,int d0,int d1);
poly pcvCV2P(poly cv,int d0,int d1);
lists pcvP2CV(lists pl,int d0,int d1);
ideal pcvP2CV(ideal p,int d0,int d1);
lists pcvCV2P(lists cvl,int d0,int d1);
ideal pcvCV2P(ideal cv,int d0,int d1);
BOOLEAN pcvP2CV(leftv res,leftv h);
BOOLEAN pcvCV2P(leftv res,leftv h);
int pcvDim(int d0,int d1);
BOOLEAN pcvDim(leftv res,leftv h);
int pcvBasis(lists b,int i,poly m,int d,int n);
lists pcvBasis(int d0,int d1);
BOOLEAN pcvBasis(leftv res,leftv h);

#endif
