#include <callgfanlib_conversion.h>
#include <std_wrapper.h>
#include <bbfan.h>
#include <groebnerCone.h>
#include <tropicalVarietyOfPolynomials.h>
#include <tropicalVarietyOfIdeals.h>
#include <libpolys/coeffs/numbers.h>
#include <libpolys/misc/options.h>
#include <kernel/structs.h>

#include <iostream>
#include <gfanlib/gfanlib_zfan.h>

BITSET bitsetSave1, bitsetSave2;

/***
 * sets option(redSB)
 **/
static void setOptionRedSB()
{
  SI_SAVE_OPT(bitsetSave1,bitsetSave2);
  si_opt_1|=Sy_bit(OPT_REDSB);
}

/***
 * sets option(noredSB);
 **/
static void undoSetOptionRedSB()
{
  SI_RESTORE_OPT(bitsetSave1,bitsetSave2);
}

static gfan::ZFan* toZFan(std::set<gfan::ZCone> maxCones)
{
  std::set<gfan::ZCone>::iterator sigma = maxCones.begin();
  gfan::ZFan* zf = new gfan::ZFan(sigma->ambientDimension());
  for (; sigma!=maxCones.end(); sigma++)
    zf->insert(*sigma);
  return zf;
}

BOOLEAN tropicalVariety(leftv res, leftv args)
{
  omUpdateInfo();
  Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    leftv v = u->next;

    if (idSize(I)==1)
    {
      poly g = I->m[0];
      if (v==NULL)
      {
        tropicalStrategy currentStrategy(I,currRing);
        std::set<gfan::ZCone> maxCones = tropicalVariety(g,currRing,currentStrategy);
        // while (1)
        // {
        //   omUpdateInfo();
        //   Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
        //   tropicalStrategy debugTest(I,currRing);
        //   maxCones = tropicalVariety(g,currRing,debugTest);
        //   gfan::ZFan* zf = toZFan(maxCones);
        //   delete zf;
        // }
        res->rtyp = fanID;
        res->data = (char*) toZFan(maxCones);
        id_Delete(&I,currRing);
        return FALSE;
      }
      if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
      {
        number p = (number) v->CopyD();
        tropicalStrategy currentStrategy(I,p,currRing);
        std::set<gfan::ZCone> maxCones = tropicalVariety(g,currRing,currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) toZFan(maxCones);
        id_Delete(&I,currRing);
        n_Delete(&p,currRing->cf);
        return FALSE;
      }
    }

    if (v==NULL)
    {
      setOptionRedSB();
      if (!hasFlag(u,FLAG_STD))
      {
        ideal stdI = gfanlib_kStd_wrapper(I,currRing);
        id_Delete(&I,currRing);
        I = stdI;
      }
      tropicalStrategy currentStrategy(I,currRing);
      gfan::ZFan* tropI = tropicalVariety(currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      undoSetOptionRedSB();
      id_Delete(&I,currRing);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      number p = (number) v->CopyD();
      if (!hasFlag(u,FLAG_STD))
      {
        ideal stdI = gfanlib_kStd_wrapper(I,currRing);
        id_Delete(&I,currRing);
        I = stdI;
      }
      tropicalStrategy currentStrategy(I,p,currRing);
      gfan::ZFan* tropI = tropicalVariety(currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      id_Delete(&I,currRing);
      return FALSE;
    }
    return FALSE;
  }
  WerrorS("tropicalVariety: unexpected parameters");
  return TRUE;
}
