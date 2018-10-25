#ifndef MAPS_H
#define MAPS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include "coeffs/coeffs.h"
#include "polys/monomials/ring.h"

// poly maEval(map theMap, poly p, ring dst_ring, nMapFunc nMap, ideal s=NULL);
poly maEval(map theMap, poly p,ring preimage_r,nMapFunc nMap, ideal s, const ring dst_r);

map maCopy(map theMap, const ring dst_ring);

poly maIMap(ring src_ring, ring dst_ring, poly p);

void maFindPerm(char const * const * const preim_names, int preim_n, char const * const * const preim_par, int preim_p,
                char const * const * const names,       int n,       char const * const * const par,       int nop,
                int * perm, int *par_perm, n_coeffType ch);
#ifdef HAVE_SHIFTBBA
void maFindPermLP(char const * const * const preim_names, int preim_n, char const * const * const preim_par, int preim_p,
                char const * const * const names,       int n,       char const * const * const par,       int nop,
                int * perm, int *par_perm, n_coeffType ch, int lV);
#endif
poly pSubstPoly(poly p, int var, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);

poly p_MinPolyNormalize(poly p, const ring r);
int maMaxDeg_P(poly p,ring preimage_r);
int maMaxDeg_Ma(ideal a,ring preimage_r);
poly maEvalVariable(poly p, int v,int pExp, ideal s, const ring dst_r);

#endif
