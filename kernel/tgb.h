#ifndef TGB_H
#define TGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tgb.h,v 1.8 2005-10-19 16:10:19 Singular Exp $ */
/*
* ABSTRACT: trepgb interface
*/

//! \file tgb.h
//#define OM_CHECK 3
//#define OM_TRACK 5

#include "ring.h"

#include "structs.h"



ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode=FALSE);
#endif
