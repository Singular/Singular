#ifndef KERNEL_PREIMAGE_H
#define KERNEL_PREIMAGE_H

#include <polys/monomials/ring.h>

ideal maGetPreimage(ring theImageRing, map theMap, ideal id, const ring dst_r);

#endif // KERNEL_PREIMAGE_H
