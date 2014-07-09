#ifndef WITNESS_H
#define WITNESS_H

#include <libpolys/polys/monomials/monomials.h>
#include <libpolys/polys/simpleideals.h>

poly witness(const poly m, const ideal I, const ideal inI, const ring r);

#ifndef NDEBUG
#include <Singular/ipid.h>

BOOLEAN dwr0(leftv res, leftv args);
BOOLEAN witness0(leftv res, leftv args);
#endif

#endif
