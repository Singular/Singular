#ifndef IPCONVERT_H
#define IPCONVERT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipconv.h,v 1.4 1999-11-15 17:20:10 obachman Exp $ */
/*
* ABSTRACT: interpreter: converting types
*/
#include "structs.h"

int iiTestConvert (int inputType, int outputType);
BOOLEAN iiConvert (int inputType, int outputType, int index,
                   leftv input, leftv output);
#endif

