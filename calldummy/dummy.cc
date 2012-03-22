#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>

#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>
#include <Singular/tok.h>


BOOLEAN writeHUHU(leftv res, leftv arg)
{
  res->rtyp=NONE;
  if (arg->Typ()==STRING_CMD)
  {
    Print("Hello %s\n",(char *)(arg->Data()));
    return FALSE;
  }
  else
    return TRUE;
}
extern "C" int mod_init(void* polymakesingular)
{

  iiAddCproc("dummy.so","hello",FALSE,writeHUHU);
  module_help_main("dummy.so","Help for dummy.so\n"
             "Provide an example for documenting dynamic modules\n");
  module_help_proc("dummy.so","hello","say hello to the <arg>");

  return 0; 
}
