#ifndef MAPS_H
#define MAPS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: maps.h,v 1.6 1997-12-15 22:46:31 obachman Exp $ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include "structs.h"

poly maEval(map theMap, poly p, ring preimage_r, matrix s=NULL);
map maCopy(map theMap);

ideal maGetPreimage(ring theImageRing, map theMap,ideal id);

poly maIMap(ring r, poly p);

BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w,
                     ring preimage_r, int *perm,
                     int *par_perm,int P);
void maFindPerm(char **preim_names, int preim_n, char **preim_par, int preim_p,
                char **names,       int n,       char **par,       int nop,
                int * perm, int *par_perm);
#endif
