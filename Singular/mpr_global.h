#ifndef MPR_GLOBAL_H
#define MPR_GLOBAL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_global.h,v 1.1 1999-04-29 11:38:51 Singular Exp $ */

/* 
* ABSTRACT - multipolynomial resultants - 
*                                global definitions and debugging stuff
*/

// Define WITH_SUBDIV to compute mixed polyhedral subdivision
//#define WITH_SUBDIV

// <matrix representation>_<determinant algorithm>
#define DENSE_FACTORY    1
#define SPARSE_BAREISS   2
#define SPARSE_GAUSS     3

#define USE_MATRIX_REP SPARSE_BAREISS

// Set to double or long double. double is recomended.
// Sets the global floating point type used in mpr_roots.cc and mpr_nric.cc.
typedef double mprfloat;

// --------------------------- debugging stuff ----------------------------
//#define mprDEBUG_ALL
#define mprDEBUG_PROT
#define mprDEBUG_STICKY

#ifdef mprDEBUG_PROT
#define mprPROT(msg) Print("%s",msg)
#define mprPROTnl(msg) Print("%s\n",msg)
#define mprPROTP(msg,poly) Print("%s",msg);pWrite0(poly) 
#define mprPROTPnl(msg,poly) Print("%s",msg);pWrite(poly) 
#define mprPROTI(msg,intval) Print("%s%d",msg,intval)
#define mprPROTInl(msg,intval) Print("%s%d\n",msg,intval)
#define mprPROTN(msg,nval) Print("%s",msg);nPrint(nval);
#define mprPROTNnl(msg,nval) Print("%s",msg);nPrint(nval);PrintLn();
#else 
#define mprPROT(msg) 
#define mprPROTnl(msg)
#define mprPROTP(msg,poly)
#define mprPROTPnl(msg,poly)
#define mprPROTI(msg,intval)
#define mprPROTInl(msg,intval)
#define mprPROTN(msg,nval)
#define mprPROTNnl(msg,nval)
#endif

#ifdef mprDEBUG_STICKY
#define mprSTICKYPROT(msg) Print(msg) 
//if (BTEST1(OPT_PROT)) Print(msg)
#else
#define mprSTICKYPROT(msg)
#endif

// output by mprSTICKYPROT
#define ST_BASE_EV "."

#define ST_DENSE_FR ":"
#define ST_DENSE_NR "."
#define ST_DENSE_MEM "+"
#define ST_DENSE_NMON "-"

#define ST_SPARSE_MEM "+"
#define ST_SPARSE_VADD "+"
#define ST_SPARSE_VREJ "-"
#define ST_SPARSE_MPEND "e"
#define ST_SPARSE_MREC1 "r"
#define ST_SPARSE_MREC2 "R"
#define ST_SPARSE_RC "+"
#define ST_SPARSE_RCRJ "-"

#define ST__DET "|"

#define ST_ROOTS_LGSTEP "|"
#define ST_ROOTS_LGPOLISH "#"
#define ST_ROOTS_LG "-"

#define ST_VANDER_STEP "."

#endif

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: *** 
