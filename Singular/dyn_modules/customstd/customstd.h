#ifndef CUSTOMSTD_H
#define CUSTOMSTD_H

#include <vector>

#include <Singular/libsingular.h>

extern std::vector<int> satstdSaturatingVariables;
extern ideal idealCache;

BOOLEAN sat_vars_sp(kStrategy strat);
BOOLEAN abort_if_monomial_sp(kStrategy strat);

#endif
