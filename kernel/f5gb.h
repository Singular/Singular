/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.3 2008-05-23 19:47:39 ederc Exp $ */
/*
* ABSTRACT: ringgb interface
*/
#ifndef F5_HEADER
#define F5_HEADER
#include "mod2.h"

#ifdef HAVE_F5
ideal F5main(ideal i, ring r);
#endif
#endif

