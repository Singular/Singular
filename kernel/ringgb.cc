/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.cc,v 1.1 2005-07-08 12:21:53 bricken Exp $ */
/*
* ABSTRACT: trepgb interface
*/
#include "mod2.h"
#include "ringgb.h"
#include "polys.h"
int foo(int bar){
  return bar + 1;
}
poly myadd(poly p, poly q, ring r){
  return p_Add_q(p_Copy(p,r),p_Copy(q,r),r);
}
