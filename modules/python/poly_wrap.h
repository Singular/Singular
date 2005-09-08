#ifndef POLY_WRAP_HEADER
#define POLY_WRAP_HEADER
#include <boost/python.hpp>
#include "mod2.h"
#include "Poly.h"
void export_poly();
boost::python::str Poly_as_str(Poly& p);
#endif
