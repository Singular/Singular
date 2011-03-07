/*
 * Developer's BB tests
 */

#include <Singular/mod2.h>

#include <Singular/blackbox.h>

#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/lists.h>

#include <dlfcn.h>

#include "bigintm.h"


static inline void NoReturn(leftv& res)
{
  res->rtyp = NONE;
  res->data = NULL;
}


/// listing all blackbox types (debug stuff)
BOOLEAN printBlackboxTypes(leftv __res, leftv __v)
{
  NoReturn(__res);
  printBlackboxTypes();
  return FALSE;
}

/// init the bigintm (a sample blackbox) type
BOOLEAN bigintm_setup(leftv __res, leftv __v)
{
  NoReturn(__res);
  return bigintm_setup();
}

extern "C" 
{
  int mod_init(SModulFunctions* psModulFunctions) 
  {
    psModulFunctions->iiAddCproc(currPack->libname,(char*)"printBlackboxTypes",FALSE, printBlackboxTypes);
    psModulFunctions->iiAddCproc(currPack->libname,(char*)"bigintm_setup",FALSE, bigintm_setup);
    return 0;
  }
}
