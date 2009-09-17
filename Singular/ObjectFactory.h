#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "Poly.h"
#include "ring.h"
#include <string>

class ObjectFactory // just static methods for providing new objects, like instances of Poly
{
public:
  static Poly newStringPoly(const std::string& polyString);
  static Poly newCanonicalPoly(const int i, const ring r);
  static ring ObjectFactory::newRing(const char* ringName, const int characteristic,
                                     const int varNumber, const char** varNames,
                                     const char* ringOrder); // so far only "lp", "dp", "Dp", "ls", "ds", "Ds"
};

#endif
/* OBJECT_FACTORY_H */
