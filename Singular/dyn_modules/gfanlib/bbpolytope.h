#ifndef BBPOLYTOPE_H
#define BBPOLYTOPE_H

#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "Singular/ipid.h"
#include "gfanlib/gfanlib.h"

THREAD_VAR extern int polytopeID;

void bbpolytope_setup(SModulFunctions* p);

// zc is meant to represent a polytope here
bigintmat* getFacetNormals(gfan::ZCone *zc);
int getAmbientDimension(gfan::ZCone* zc);
int getCodimension(gfan::ZCone *zc);
int getDimension(gfan::ZCone* zc);

gfan::ZVector intStar2ZVectorWithLeadingOne(const int d, const int* i);

#endif
#endif
