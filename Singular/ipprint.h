#ifndef IPPRINT_H
#define IPPRINT_H
/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: interpreter: printing


*/
#include "kernel/structs.h"

class sleftv; typedef sleftv * leftv;

BOOLEAN jjPRINT(leftv res, leftv u);
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v);
BOOLEAN jjDBPRINT(leftv res, leftv u);

void ipPrint_MA0(matrix m, const char *name);
#endif

