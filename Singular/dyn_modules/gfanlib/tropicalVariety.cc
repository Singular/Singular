#include <callgfanlib_conversion.h>
#include <std_wrapper.h>
#include <bbfan.h>
#include <groebnerCone.h>
#include <tropicalVarietyOfPolynomials.h>
#include <tropicalVarietyOfIdeals.h>
#include <coeffs/numbers.h>
#include <misc/options.h>
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
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    leftv v = u->next;

    if (idSize(I)==1)
    {
      poly g = I->m[0];
      if (v==NULL)
      {
        tropicalStrategy currentStrategy(I,currRing);
        std::set<gfan::ZCone> maxCones = tropicalVariety(g,currRing,&currentStrategy);
        // gfan::ZFan* zf = toZFan(maxCones);
        // delete zf;
        // res->rtyp = NONE;
        // res->data = NULL;
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
        return FALSE;
      }
      if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
      {
        number p = (number) v->Data();
        tropicalStrategy currentStrategy(I,p,currRing);
        ideal startingIdeal = currentStrategy.getStartingIdeal();
        ring startingRing = currentStrategy.getStartingRing();
        poly gStart = startingIdeal->m[0];
        std::set<gfan::ZCone> maxCones = tropicalVariety(gStart,startingRing,&currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) toZFan(maxCones);
        return FALSE;
      }
    }

    if (v==NULL)
    {
      setOptionRedSB();
      // ideal stdI;
      // if (!hasFlag(u,FLAG_STD))
      //   stdI = gfanlib_kStd_wrapper(I,currRing);
      // else
      //   stdI = id_Copy(I,currRing);
      tropicalStrategy currentStrategy(I,currRing);
      gfan::ZFan* tropI = tropicalVariety(currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      undoSetOptionRedSB();
      // id_Delete(&stdI,currRing);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      number p = (number) v->Data();
      // ideal stdI;
      // if (!hasFlag(u,FLAG_STD))
      //   stdI = gfanlib_kStd_wrapper(I,currRing);
      // else
      //   stdI = id_Copy(I,currRing);
      // tropicalStrategy currentStrategy(stdI,p,currRing);
      tropicalStrategy currentStrategy(I,p,currRing);
      gfan::ZFan* tropI = tropicalVariety(currentStrategy);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      // id_Delete(&stdI,currRing);
      return FALSE;
    }
    return FALSE;
  }
  WerrorS("tropicalVariety: unexpected parameters");
  return TRUE;
}
