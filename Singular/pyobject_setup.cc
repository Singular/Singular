// -*- c++ -*-
//*****************************************************************************
/*!
!
* @file pyobject_setup.cc
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

#include "kernel/mod2.h"

#include "Singular/blackbox.h"
#include "Singular/ipshell.h"

static BOOLEAN pyobject_load()
{
  return jjLOAD("pyobject.so", TRUE);
}

/// blackbox support - initialization via autoloading
void* pyobject_autoload(blackbox* bbx)
{
  assume(bbx != NULL);
  return (pyobject_load() || (bbx->blackbox_Init == pyobject_autoload)?
    NULL: bbx->blackbox_Init(bbx));
}

void pyobject_default_destroy(blackbox  

*/*b

*/, void 

*/*d

*/)
{
  WerrorS("Python-based functionality not available!");
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
BOOLEAN pyobject_ensure()
{

  int tok = -1;
  blackbox* bbx = (blackboxIsCmd("pyobject", tok) == ROOT_DECL?
                   getBlackboxStuff(tok): (blackbox*)NULL);
  if (bbx == NULL) return TRUE;
  return (bbx->blackbox_Init == pyobject_autoload?  pyobject_load(): FALSE);
}
