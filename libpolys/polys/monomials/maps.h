#ifndef MAPS_H
#define MAPS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include <coeffs/coeffs.h>
#include <polys/monomials/ring.h>
//#include <kernel/ideals.h>

typedef number (*nMapFunc)(number a, coeffs src, coeffs dst);
struct sip_smap;
typedef struct sip_smap *         map;

poly maEval(map theMap, poly p, ring dst_ring, nMapFunc nMap, ideal s=NULL);
map maCopy(map theMap, const ring dst_ring);

ideal maGetPreimage(ring theImageRing, map theMap,ideal id);

poly maIMap(ring src_ring, ring dst_ring, poly p);

/*
BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w,
                     ring preimage_r, int *perm,
                     int *par_perm,int P, nMapFunc nMap);
*/
void maFindPerm(char **preim_names, int preim_n, char **preim_par, int preim_p,
                char **names,       int n,       char **par,       int nop,
                int * perm, int *par_perm, int ch);
poly pSubstPoly(poly p, int var, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);

poly pMinPolyNormalize(poly p);
int maMaxDeg_P(poly p,ring preimage_r);
int maMaxDeg_Ma(ideal a,ring preimage_r);
#endif
