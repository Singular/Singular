#include "kernel/mod2.h" // general settings/macros
#include"kernel/febase.h"  // for Print, WerrorS
#include"Singular/ipid.h" // for SModulFunctions, leftv
#if SINGULAR_MAJOR_VERSION>3
#include"coeffs/bigintmat.h"
#else
#include"kernel/bigintmat.h"
#endif

BOOLEAN concat_im(leftv res, leftv arg)
{
  int rt=res->rtyp=arg->Typ();  // set the result type
  if ((arg->next==NULL)||(arg->next->next!=NULL))
    return TRUE;
  switch (rt)
  {
    case INTMAT_CMD:
    {
      if (arg->next->Typ()==INTMAT_CMD)
      {
        res->data=(char*)ivConcat((intvec*)arg->Data(),
                                 (intvec*)arg->next->Data());
        return FALSE;
      }
      else return TRUE;
    }
    case BIGINTMAT_CMD:
    {
      if (arg->next->Typ()==BIGINTMAT_CMD)
      {
        #if SINGULAR_MAJOR_VERSION>3
        res->data=(char*)bimConcat((bigintmat*)arg->Data(),
                                 (bigintmat*)arg->next->Data(), coeffs_BIGINT);
	#else
        res->data=(char*)bimConcat((bigintmat*)arg->Data(),
                                 (bigintmat*)arg->next->Data());
	#endif
        return FALSE;
      }
      else return TRUE;
    }
  }
  return TRUE;
}

extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
  // this is the initialization routine of the module
  // adding the routine hello:
  psModulFunctions->iiAddCproc(
    (currPack->libname? currPack->libname: ""),// the library name,
            // rely on the loader to set it in currPack->libname
    "concat_im",// the name for the singular interpreter
    FALSE,  // should not be static
    concat_im); // the C/C++ routine
  const char* help_concat_im="USAGE: concat_im(intmat a, intmat b)\n"
                            "       concat_im(bigintmat a, bigintmat b)\n"
                       "RETURN: columwise concatination of a and b";
  module_help_proc(
    (currPack->libname? currPack->libname: ""),// the library name,
    "concat_im", // the name of the procedure
    help_concat_im); // the help string
  module_help_main(
     (currPack->libname? currPack->libname: ""),// the library name,
    "tools for intmat/bigintmat"); // the help string for the module
  return 1;
}
