#ifndef IPPRINT_H
#define IPPRINT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipprint.h,v 1.3 1997-04-09 12:19:50 Singular Exp $ */
/*
* ABSTRACT: interpreter: printing
*/
#include "structs.h"

BOOLEAN jjPRINT_GEN(leftv res, leftv u);
BOOLEAN jjPRINT_INTVEC(leftv res, leftv u);
BOOLEAN jjPRINT_INTMAT(leftv res, leftv u);
BOOLEAN jjPRINT_MA(leftv res, leftv u);
void    jjPRINT_MA0(matrix m, const char *name);
BOOLEAN jjPRINT_V(leftv res, leftv u);
BOOLEAN jjPRINT_LIST(leftv res, leftv u);
BOOLEAN jjDBPRINT(leftv res, leftv u);
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v);
#endif

