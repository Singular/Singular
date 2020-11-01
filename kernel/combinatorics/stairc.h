#ifndef STAIRC_H
#define STAIRC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "misc/intvec.h"

void scComputeHC(ideal s,ideal Q, int  k,poly &hEdge, ring tailRing = currRing);
#if 0 // - alternative implementation for tests
void scComputeHCw(ideal s,ideal Q, int  k,poly &hEdge, ring tailRing = currRing);
#endif

intvec * scIndIntvec(ideal S, ideal Q=NULL);

// lists scIndIndset(ideal S, BOOLEAN all, ideal Q=NULL); // TODO: move to Singular/

/// ideal dimension
int scDimInt(ideal  s,ideal Q=NULL);
/// scDimInt for ring-coefficients
int scDimIntRing(ideal  s,ideal Q=NULL);
int scMultInt(ideal  s,ideal Q=NULL);
int scMult0Int(ideal  s,ideal Q=NULL, const ring tailRing = currRing);
void scPrintDegree(int co, int mu);
void scDegree(ideal  s,intvec *modulweight,ideal Q=NULL);

ideal scKBase(int deg, ideal  s, ideal Q=NULL, intvec * mv=NULL);

#if HAVE_SHIFTBBA
int lp_gkDim(const ideal G);
intvec * lp_ufnarovskiGraph(ideal G, ideal &standardWords);
#endif

#endif


