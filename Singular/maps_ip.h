#ifndef MAPS_IP_H
#define MAPS_IP_H
/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT - the mapping of polynomials to other rings


*/

#include "kernel/mod2.h"
#include "polys/matpol.h"

#include "kernel/structs.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"

poly pSubstPoly(poly p, int var, poly image);
poly pSubstPar(poly p, int par, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);
ideal  idSubstPar(ideal id, int n, poly e);


/*!
!

* maps the expression w to res,
* switch what: MAP_CMD: use theMap for mapping, N for preimage ring
*              //FETCH_CMD: use pOrdPoly for mapping
*              IMAP_CMD: use perm for mapping, N for preimage ring
*              default: map only poly-structures,
*                       use perm and par_perm, N and P,


*/
BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w, ring preimage_r,
                     int *perm, int *par_perm, int P, nMapFunc nMap);
#endif
