#include <locals.h>

extern void piShowProcList();

BOOLEAN kProclist(leftv res, leftv h)
{
  piShowProcList();
  res->rtyp=NONE;
  return FALSE;
}
