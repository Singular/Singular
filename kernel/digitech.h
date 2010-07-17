#ifndef DIGITECH_HEADER
#define DIGITECH_HEADER
#include <kernel/mod2.h>
#include <kernel/polys.h>
void bit_reduce(poly & f,ring r);
poly uni_subst_bits(poly outer_uni, poly inner_multi, ring r);
#endif
