/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: Gauss-Manin system normal form
*/

#ifndef GMS_H
#define GMS_H

#include <misc/auxiliary.h>
#include <polys/matpol.h>

#include <kernel/structs.h>
#include <kernel/ideals.h>

#include <Singular/lists.h>

lists gmsNF(ideal p, ideal g, matrix B, int D, int K);
BOOLEAN gmsNF(leftv res, leftv h);

#endif /* GMS_H */
