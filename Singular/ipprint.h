#ifndef IPPRINT_H
#define IPPRINT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
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

