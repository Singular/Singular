#ifndef IPCONVERT_H
#define IPCONVERT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipconv.h,v 1.3 1997-04-09 12:19:49 Singular Exp $ */
/*
* ABSTRACT: interpreter: converting types
*/
#include "structs.h"

int iiTestConvert (int inputType, int outputType);
BOOLEAN iiConvert (int inputType, int outputType, int index,
                   leftv input, leftv output);
#endif

