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

#include "config.h"
#include <kernel/mod2.h>
#include <kernel/febase.h>
#include <Singular/blackbox.h>
#include <Singular/ipshell.h>

/* whether pyobject module is linked statically or dynamically */

#ifdef EMBED_PYTHON // Case: we include the pyobject interface in the binary

#ifdef HAVE_PYTHON
#include "pyobject.cc"
static BOOLEAN pyobject_load()
{
   pyobject_init(iiAddCproc);
   return FALSE;
}

#else // Forced embedding, but no (development version of) python available!
static BOOLEAN pyobject_load() { return TRUE; } 
#endif


# else // Case: pyobject may be loaded from a dynamic module (prefered variant)
// Note: we do not need python at compile time.
static BOOLEAN pyobject_load()
{
  return jjLOAD("pyobject.so", TRUE);
}
#endif



/// blackbox support - initialization via autoloading
void* pyobject_autoload(blackbox* bbx)
{
  assume(bbx != NULL);
  return (pyobject_load()? NULL: bbx->blackbox_Init(bbx));
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

/// Explicitely load, if not loaded already
BOOLEAN pyobject_ensure() {

  int tok = -1;
  blackbox* bbx = (blackboxIsCmd("pyobject", tok) == ROOT_DECL?
                   getBlackboxStuff(tok): (blackbox*)NULL);
  if (bbx == NULL) return TRUE;
  return (bbx->blackbox_Init == pyobject_autoload?  pyobject_load(): FALSE);  
}



