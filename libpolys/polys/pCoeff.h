// emacs edit mode for this file is -*- C++ -*-
#ifndef PCOEFF_H
#define PCOEFFH
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: accces single (coeffs of) monoms
*/

#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"

#include "polys/monomials/p_polys.h"

#include "simpleideals.h"

/// find coeff of (polynomial/vector) m in polynomial/vector p
number p_CoeffTerm(poly p, poly m, const ring r);

/// find vector of coeffs of (polynomial) m in vector v
poly p_CoeffTermV(poly v, poly m, const ring r);

/// find coeffs of (polynomial) m in all polynomials from I
ideal id_CoeffTerm(ideal I, poly m, const ring r);

/// find coeffs of (polynomial) m in all vectors from I
ideal id_CoeffTermV(ideal M, poly m, const ring r);
#endif
