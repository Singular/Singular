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

void scComputeHC(ideal s,ideal Q, int  k,poly &hEdge);

intvec * scIndIntvec(ideal S, ideal Q=NULL);

// lists scIndIndset(ideal S, BOOLEAN all, ideal Q=NULL); // TODO: move to Singular/

/// ideal dimension
int scDimInt(ideal  s,ideal Q=NULL); // dim (cf: field)
/// scDimInt for ring-coefficients
int scDimIntRing(ideal  s,ideal Q=NULL); // dim (general)
int scMultInt(ideal  s,ideal Q=NULL); // mult
long scMult0Int(ideal  s,ideal Q=NULL); // vdim
void scDegree(ideal  s,intvec *modulweight,ideal Q=NULL);
void scPrintDegree(int co, int mu);

ideal scKBase(int deg, ideal  s, ideal Q=NULL, intvec * mv=NULL);

#if HAVE_SHIFTBBA
int lp_gkDim(const ideal G);
int lp_kDim(const ideal G);
intvec * lp_ufnarovskiGraph(ideal G, ideal &standardWords);
#endif

#endif


