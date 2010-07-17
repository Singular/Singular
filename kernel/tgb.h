#ifndef TGB_H
#define TGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: trepgb interface
*/

//! \file tgb.h
//#define OM_CHECK 3
//#define OM_TRACK 5

#include <kernel/ring.h>

#include <kernel/structs.h>



ideal t_rep_gb(ring r,ideal arg_I, int syz_comp, BOOLEAN F4_mode=FALSE);
#endif
