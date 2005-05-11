#ifndef TGB_H
#define TGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tgb.h,v 1.7 2005-05-11 10:11:54 bricken Exp $ */
/*
* ABSTRACT: trepgb interface
*/

//! \file tgb.h
//#define OM_CHECK 3
//#define OM_TRACK 5
#include "mod2.h"

#include "ring.h"

#include "structs.h"



ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode=FALSE);
#endif
