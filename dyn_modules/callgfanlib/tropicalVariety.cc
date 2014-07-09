#include <callgfanlib_conversion.h>
#include <bbfan.h>
#include <groebnerCone.h>
#include <tropicalVarietyOfIdeals.h>
#include <libpolys/coeffs/numbers.h>
#include <kernel/structs.h>

BOOLEAN tropicalVariety(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    leftv v = u->next;
    if (v==NULL)
    {
      tropicalStrategy currentStrategy(I,currRing);
      gfan::ZFan* tropI = tropicalVariety(I,currRing,currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      number p = (number) v->CopyD();
      tropicalStrategy currentStrategy(I,p,currRing);
      gfan::ZFan* tropI = tropicalVariety(I,currRing,currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      return FALSE;
    }
    return FALSE;
  }
  WerrorS("tropicalVariety: unexpected parameters");
  return TRUE;
}
