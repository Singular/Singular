/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: ideals of points (with multiplicities)
*/

#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include "polys/monomials/ring.h"
#include "misc/intvec.h"
#include <vector>

ideal interpolation(const std::vector<ideal>& L, intvec *v);


#endif

