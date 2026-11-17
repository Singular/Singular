/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 ***************************************************************/
#ifndef SSILINK_H
#define SSILINK_H

#include "Singular/links/silink.h"

BOOLEAN ssiOpen(si_link l, short flag, leftv u);
BOOLEAN ssiWrite(si_link l, leftv v);
BOOLEAN ssiWrite2(si_link l, leftv res, leftv v);
leftv ssiRead1(si_link l);
leftv ssiRead2(si_link l, leftv key);
BOOLEAN ssiClose(si_link l);
const char* slStatusSsi(si_link l, const char* request);
si_link_extension slInitSsiExtension(si_link_extension s);

si_link ssiCommandLink();

void sig_chld_hdl(int sig);
int ssiReservePort(int clients);

void singular_close_links();

// for use in kstdhelper
ideal ssiReadIdeal(ssiInfo *d);
void ssiWriteIdeal(const ssiInfo *d, int typ,const ideal I);
int ssiReadInt(const ssiInfo *d);
void ssiWriteInt(const ssiInfo *d,const int i);
char* ssiReadString(const ssiInfo *d);

// ssi via string
char* ssiWritePoly_S(poly p, const ring r);
poly ssiReadPoly_S(char *s, const ring r);
char* ssiWriteIdeal_S(const ideal I, const ring R);
ideal ssiReadIdeal_S(char *s, const ring R);
char* ssiWriteMatrix_S(const matrix M, const ring R);
matrix ssiReadMatrix_S(char *s, const ring R);
char* ssiWriteModule_S(const ideal M, const ring R); /* also for smatrix*/
char* ssiWriteRing_S(const ring r);
ring ssiReadRing_S(char *s);


leftv ssiRead1_S(char**s, const ring R);
void ssiWrite_S(leftv v,const ring R);
#endif
