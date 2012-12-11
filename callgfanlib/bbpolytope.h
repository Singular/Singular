#ifndef BBPOLYTOPE_H
#define BBPOLYTOPE_H

#include <Singular/mod2.h>
#ifdef HAVE_FANS

#include <gfanlib/gfanlib.h>

extern int polytopeID;

void bbpolytope_setup();

// zc is meant to represent a polytope here
bigintmat* getFacetNormals(gfan::ZCone *zc);
int getAmbientDimension(gfan::ZCone* zc); 
int getCodimension(gfan::ZCone *zc);
int getDimension(gfan::ZCone* zc);

#endif
#endif
