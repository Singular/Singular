// -*- c++ -*-
//*****************************************************************************
/** @file pyobject_setup.cc
 *
 * @author Alexander Dreyer
 * @date 2010-12-15
 *
 * This header file defines the @c blackbox setup operations for the pyobject
 *
 * @par Copyright:
 *   (c) 2010 by The Singular Team, see LICENSE file
**/
//*****************************************************************************

#include <Singular/mod2.h>
#include <kernel/febase.h>
#include "static.h"

/* whether development version of python is linked statically or dynamically */
#ifdef HAVE_STATIC
  #define HAVE_STATIC_PYTHON 1
#endif

#ifdef HAVE_PYTHON  // Case: python available (prefered variant) 


# ifdef HAVE_STATIC_PYTHON // Case: link pyobject interface statically
#include "pyobject.cc"
void pyobject_setup() { pyobject_init(); }


# else                     // Case: pyobject is dynamic module (prefered variant)

// forward declaration for Singular/iplib.cc
void* binary_module_function(const char* lib, const char* func); 
void pyobject_setup()
{
  void* fktn = binary_module_function("pyobject", "mod_init");
  if (fktn) (* reinterpret_cast<void (*)()>(fktn) )();
  else Werror("python related functions are not avialable");
}

# endif

#else                // Case: no python
void pyobject_setup() { }

#endif  // HAVE_PYTHON

