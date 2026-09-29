#ifndef HILB_H
#define HILB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "misc/intvec.h"
#include "misc/int64vec.h"
#include "coeffs/bigintmat.h"

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries1(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries0(ideal S, ideal Q, intvec *wdegree, const ring src, const ring Qt);
poly hFirstSeries0p(ideal A,ideal Q, intvec *wdegree, const ring src, const ring Qt);
poly hFirstSeries0p64(ideal A,ideal Q, const int64vec *wdegree, const ring src, const ring Qt);
bigintmat* hPoly2BIV(poly h, const ring Qt, const coeffs biv_cf);
poly hBIV2Poly(bigintmat* b, const ring Qt, const coeffs biv_cf);
bigintmat* hFirstSeries0b(ideal I, ideal Q, intvec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
bigintmat* hFirstSeries0b64(ideal I, ideal Q, const int64vec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
bigintmat* hSecondSeries0b(ideal I, ideal Q, intvec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
bigintmat* hSecondSeries0b64(ideal I, ideal Q, const int64vec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
poly hFirstSeries0m(ideal A,ideal Q, intvec *wdegree, intvec *shifts, const ring src, const ring Qt);
poly hFirstSeries0m64(ideal A,ideal Q, const int64vec *wdegree, intvec *shifts, const ring src, const ring Qt);

/// The shared univariate ring used by the sparse Hilbert-series interface.
/// Its exponent capacity is LONG_MAX; callers do not own the returned ring.
ring hHilbertSeriesRing();

intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
#endif
