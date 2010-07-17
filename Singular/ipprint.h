#ifndef IPPRINT_H
#define IPPRINT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interpreter: printing
*/
#include <kernel/structs.h>

BOOLEAN jjPRINT(leftv res, leftv u);
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v);
BOOLEAN jjDBPRINT(leftv res, leftv u);
#endif

