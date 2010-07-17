#include <locals.h>

extern void piShowProcList();

BOOLEAN kProclist(leftv res)
{
  piShowProcList();
  res->rtyp=NONE;
  return FALSE;
}
