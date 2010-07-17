#ifndef MAPS_IP_H
#define MAPS_IP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/
#include <kernel/structs.h>

poly pSubstPoly(poly p, int var, poly image);
poly pSubstPar(poly p, int par, poly image);
ideal  idSubstPoly(ideal id, int n, poly e);
ideal  idSubstPar(ideal id, int n, poly e);
#endif
