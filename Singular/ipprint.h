#ifndef IPPRINT_H
#define IPPRINT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipprint.h,v 1.4 1999-04-17 14:58:50 obachman Exp $ */
/*
* ABSTRACT: interpreter: printing
*/
#include "structs.h"

BOOLEAN jjPRINT(leftv res, leftv u);
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v);
BOOLEAN jjDBPRINT(leftv res, leftv u);
#endif

