/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: misc.cc,v 1.1 1998-11-19 15:49:24 krueger Exp $ */
/*
* ABSTRACT: interpreter: LIB and help
*/

#include <locals.h>

BOOLEAN iiKernelMiscNN(leftv res, leftv h)
{
  
  idhdl pck = NULL, id = NULL;
  iiname2hdl((char *)(h->Data()), &pck, &id);
  if(pck != NULL) Print("Pack: '%s'\n", pck->id);
  if(id != NULL)  Print("Rec : '%s'\n", id->id);

  res->rtyp=NONE;
  return FALSE;
}
