#ifndef BBPOLYTOPE_H
#define BBPOLYTOPE_H

#ifdef HAVE_FANS
#include <gfanlib/gfanlib.h>

extern int polytopeID;

void bbpolytope_setup();

intvec* getFacetNormals(gfan::ZCone *zc);
int getAmbientDimension(gfan::ZCone* zc); // zc is meant to represent a polytope here
int getCodimension(gfan::ZCone *zc);
int getDimension(gfan::ZCone* zc);

#endif
#endif
