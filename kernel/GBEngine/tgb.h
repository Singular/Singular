#ifndef TGB_H
#define TGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: trepgb interface
*/

//! \file tgb.h
//#define OM_CHECK 3
//#define OM_TRACK 5

#include <polys/monomials/ring.h>

#include <kernel/structs.h>



ideal t_rep_gb(ring r,ideal arg_I, int syz_comp, BOOLEAN F4_mode=FALSE);
#endif
