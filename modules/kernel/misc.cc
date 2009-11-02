/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interpreter: LIB and help
*/

#include <locals.h>

BOOLEAN iiKernelMiscNN(leftv res, leftv h)
{
  
  idhdl pck = NULL, id = NULL;
  printf("myTest 1\n");
  if(h==NULL) printf("myTest 2\n");
  else {
    iiname2hdl((char *)(h->Data()), &pck, &id);
    printf("myTest 1\n");
    if(pck != NULL) Print("Pack: '%s'\n", pck->id);
    if(id != NULL)  Print("Rec : '%s'\n", id->id);
  }
  
  res->rtyp=NONE;
  return FALSE;
}
