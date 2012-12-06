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
#include <Singular/blackbox.h>
#include <Singular/ipshell.h>

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


/// blackbox support - initialization via autoloading
void* pyobject_autoload(blackbox* bbx)
{
  if (jjLOAD("pyobject.so", TRUE)) return NULL;

  return bbx->blackbox_Init(bbx);
}

void pyobject_default_destroy(blackbox  *b, void *d)
{
  Werror("Python-based functionality not available!");
}

// Setting up an empty blackbox type, which can be filled with pyobject
void pyobject_setup() 
{
  blackbox *bbx = (blackbox*)omAlloc0(sizeof(blackbox));
  bbx->blackbox_Init = pyobject_autoload;
  bbx->blackbox_destroy = pyobject_default_destroy;
  setBlackboxStuff(bbx, "pyobject");
}
 
#else                // Case: no python
void pyobject_setup() { }

#endif  // HAVE_PYTHON

