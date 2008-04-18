/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.2 2008-04-18 13:32:53 wienand Exp $ */
/*
* ABSTRACT: ringgb interface
*/
#ifndef F5_HEADER
#define F5_HEADER
#include "mod2.h"

#ifdef HAVE_F5
ideal computeF5(ideal I, ring r);
#endif

#endif
