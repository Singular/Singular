#ifndef IPPRINT_H
#define IPPRINT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipprint.h,v 1.5 1999-11-15 17:20:11 obachman Exp $ */
/*
* ABSTRACT: interpreter: printing
*/
#include "structs.h"

BOOLEAN jjPRINT(leftv res, leftv u);
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v);
BOOLEAN jjDBPRINT(leftv res, leftv u);
#endif

