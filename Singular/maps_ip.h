#ifndef MAPS_IP_H
#define MAPS_IP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: maps_ip.h,v 1.1 2005-04-18 15:10:04 Singular Exp $ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include "structs.h"

poly pSubstPoly(poly p, int var, poly image);
poly pSubstPar(poly p, int par, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);
ideal  idSubstPar(ideal id, int n, poly e);
#endif
