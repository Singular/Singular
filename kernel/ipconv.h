#ifndef IPCONVERT_H
#define IPCONVERT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipconv.h,v 1.1.1.1 2003-10-06 12:15:55 Singular Exp $ */
/*
* ABSTRACT: interpreter: converting types
*/
#include "structs.h"

int iiTestConvert (int inputType, int outputType);
BOOLEAN iiConvert (int inputType, int outputType, int index,
                   leftv input, leftv output);
#endif

