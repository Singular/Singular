#ifndef fast_mult_header
#define fast_mult_header
#include "mod2.h"
#include "polys.h"
poly unifastmult(poly f,poly g, ring r);
poly multifastmult(poly f, poly g, ring r);
int Mults();
poly pFastPower(poly f, int n, ring r);
poly pFastPowerMC(poly f, int n, ring r);
#endif
