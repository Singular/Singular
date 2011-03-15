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
BOOLEAN printBlackboxTypes0(leftv __res, leftv __v)
{
  NoReturn(__res);
  printBlackboxTypes();
  return FALSE;
}

/// init the bigintm (a sample blackbox) type
BOOLEAN bigintm_setup0(leftv __res, leftv __v)
{
  NoReturn(__res);
  return bigintm_setup();
}

static long int load_counter = -1;

/// init the bigintm (a sample blackbox) type
BOOLEAN print_load_counter(leftv __res, leftv __v)
{
  Print("print_load_counter: load counter: %ld", load_counter);
  Print(", printBlackboxTypes: %p", (void*)(printBlackboxTypes0));
  Print(", bigintm_setup: %p", (void*)(bigintm_setup0));
  PrintLn();

  NoReturn(__res);
  return FALSE;
}


extern "C" 
{
  int mod_init(SModulFunctions* psModulFunctions) 
  {
    load_counter++;
    
    if( !load_counter)
    {
      
      psModulFunctions->iiAddCproc(currPack->libname,(char*)"printBlackboxTypes",FALSE, printBlackboxTypes0);
      psModulFunctions->iiAddCproc(currPack->libname,(char*)"bigintm_setup",FALSE, bigintm_setup0);
      psModulFunctions->iiAddCproc(currPack->libname,(char*)"bigintm_print_load_counter",FALSE, print_load_counter);

      // Q: should we call 'bigintm_setup' here??!?!?
      return 0;
    }
    else
    {
      PrintS("ERROR: Sorry this dynamic module was already loaded...!!!");
      PrintLn();
      // Q: what about loading this module multipple times...?
      return 0; // -1? - No difference!!!

    }
      
  }
}
