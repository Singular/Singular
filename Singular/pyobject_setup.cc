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

/* whether pyobject module is linked statically or dynamically */
#ifdef HAVE_PYTHON

  #if defined(HAVE_STATIC) 
    #ifdef HAVE_STATIC_PYTHON
      #define HAVE_STATIC_PYOBJECT
    #endif
  #else
    #ifdef EMBED_PYTHON
      #define HAVE_STATIC_PYOBJECT
    #else
      #define HAVE_DYNAMIC_PYOBJECT
    #endif
  #endif 
#endif

# ifdef HAVE_STATIC_PYOBJECT // Case: link pyobject interface statically
#include "pyobject.cc"
void pyobject_setup() { pyobject_init(); }


# elif defined(HAVE_DYNAMIC_PYOBJECT) // Case: pyobject is dynamic module (prefered variant)

// forward declaration for Singular/iplib.cc
void* binary_module_function(const char* lib, const char* func); 
void pyobject_setup()
{
  void* fktn = binary_module_function("pyobject", "mod_init");
  if (fktn) (* reinterpret_cast<void (*)()>(fktn) )();
  else Werror("python related functions are not available");
}

#else                // Case: no python
void pyobject_setup() { }

#endif  // HAVE_PYTHON

