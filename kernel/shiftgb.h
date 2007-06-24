#ifndef SHIFTGB_H
#define SHIFTGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: shiftgb.h,v 1.2 2007-06-24 16:44:42 levandov Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/

#include "structs.h"
#include "gring.h"

poly pLPshift(poly p, int sh, int uptodeg, int lV);
poly pmLPshift(poly p, int sh, int uptodeg, int lV);

int pLastVblock(poly p, int lV);
int pmLastVblock(poly p, int lV);

int isInV(poly p, int lV);

#endif
