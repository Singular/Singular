#ifndef MAPS_H
#define MAPS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: maps.h,v 1.1.1.1 2003-10-06 12:15:57 Singular Exp $ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include "structs.h"

poly maEval(map theMap, poly p, ring preimage_r, nMapFunc nMap,matrix s=NULL);
map maCopy(map theMap);

ideal maGetPreimage(ring theImageRing, map theMap,ideal id);

poly maIMap(ring r, poly p);

BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w,
                     ring preimage_r, int *perm,
                     int *par_perm,int P, nMapFunc nMap);
void maFindPerm(char **preim_names, int preim_n, char **preim_par, int preim_p,
                char **names,       int n,       char **par,       int nop,
                int * perm, int *par_perm, int ch);
poly pSubstPoly(poly p, int var, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);

poly pMinPolyNormalize(poly p);
int maMaxDeg_P(poly p,ring preimage_r);
int maMaxDeg_Ma(ideal a,ring preimage_r);
#endif
