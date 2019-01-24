#include "kernel/mod2.h"

#include "Singular/mod_lib.h"

#include "Singular/blackbox.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/lists.h"

#include "bigintm.h"

namespace
{

static inline void NoReturn(leftv& res)
{
  res->rtyp = NONE;
  res->data = NULL;
}


/// listing all blackbox types (debug stuff)
static BOOLEAN printBlackboxTypes0(leftv __res, leftv /*__v*/)
{
  NoReturn(__res);
  printBlackboxTypes();
  return FALSE;
}

/// init the bigintm (a sample blackbox) type
static BOOLEAN bigintm_setup0(leftv __res, leftv /*__v*/)
{
  NoReturn(__res);
  return bigintm_setup();
}

}


extern "C" int SI_MOD_INIT(bigintm)(SModulFunctions* psModulFunctions)
{
   psModulFunctions->iiAddCproc(currPack->libname,(char*)"printBlackboxTypes",FALSE, printBlackboxTypes0);
   psModulFunctions->iiAddCproc(currPack->libname,(char*)"bigintm_setup",FALSE, bigintm_setup0);

   // Q: should we call 'bigintm_setup' here??!?!?
   VAR return MAX_TOK;
}
