#include "callgfanlib_conversion.h"
#include "std_wrapper.h"
#include "bbfan.h"
#include "groebnerCone.h"
#include "tropicalVarietyOfPolynomials.h"
#include "tropicalVarietyOfIdeals.h"
#include "coeffs/numbers.h"
#include "misc/options.h"
#include "kernel/structs.h"

#include "gfanlib/gfanlib_zfan.h"

VAR BITSET bitsetSave1, bitsetSave2;


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

static gfan::ZFan* toZFan(std::set<gfan::ZCone> maxCones, int d)
{
  gfan::ZFan* zf = new gfan::ZFan(d);
  for (std::set<gfan::ZCone>::iterator sigma = maxCones.begin(); sigma!=maxCones.end(); sigma++)
    zf->insert(*sigma);
  return zf;
}


BOOLEAN tropicalVariety(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==POLY_CMD))
  {
    poly g = (poly) u->Data();
    leftv v = u->next;
    if (v==NULL)
    {
      try
      {
        ideal I = idInit(1);
        I->m[0] = g;
        tropicalStrategy currentStrategy(I,currRing);
        std::set<gfan::ZCone> maxCones = tropicalVariety(g,currRing,&currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) toZFan(maxCones,currentStrategy.getExpectedAmbientDimension());
        I->m[0] = NULL;
        id_Delete(&I,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      try
      {
        ideal I = idInit(1);
        I->m[0] = g;
        number p = (number) v->Data();
        tropicalStrategy currentStrategy(I,p,currRing);
        ideal startingIdeal = currentStrategy.getStartingIdeal();
        ring startingRing = currentStrategy.getStartingRing();
        poly gStart = startingIdeal->m[0];
        std::set<gfan::ZCone> maxCones = tropicalVariety(gStart,startingRing,&currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) toZFan(maxCones,currentStrategy.getExpectedAmbientDimension());
        I->m[0] = NULL;
        id_Delete(&I,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }

    }
  }
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    leftv v = u->next;

    if ((I->m[0]!=NULL) && (idElem(I)==1))
    {
      poly g = I->m[0];
      if (v==NULL)
      {
        try
        {
          tropicalStrategy currentStrategy(I,currRing);
          std::set<gfan::ZCone> maxCones = tropicalVariety(g,currRing,&currentStrategy);
          res->rtyp = fanID;
          res->data = (char*) toZFan(maxCones,currentStrategy.getExpectedAmbientDimension());
          return FALSE;
        }
        catch (const std::exception& ex)
        {
          Werror("ERROR: %s",ex.what());
          return TRUE;
        }
      }
      if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
      {
        try
        {
          number p = (number) v->Data();
          tropicalStrategy currentStrategy(I,p,currRing);
          ideal startingIdeal = currentStrategy.getStartingIdeal();
          ring startingRing = currentStrategy.getStartingRing();
          poly gStart = startingIdeal->m[0];
          std::set<gfan::ZCone> maxCones = tropicalVariety(gStart,startingRing,&currentStrategy);
          res->rtyp = fanID;
          res->data = (char*) toZFan(maxCones,currentStrategy.getExpectedAmbientDimension());
          return FALSE;
        }
        catch (const std::exception& ex)
        {
          Werror("ERROR: %s",ex.what());
          return TRUE;
        }
      }
    }

    if (v==NULL)
    {
      try
      {
        setOptionRedSB();
        ideal stdI;
        if (!hasFlag(u,FLAG_STD))
          stdI = gfanlib_kStd_wrapper(I,currRing);
        else
          stdI = id_Copy(I,currRing);
        tropicalStrategy currentStrategy(stdI,currRing);
        gfan::ZFan* tropI = tropicalVariety(currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) tropI;
        undoSetOptionRedSB();
        id_Delete(&stdI,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      try
      {
        number p = (number) v->Data();
        ideal stdI;
        if (!hasFlag(u,FLAG_STD))
          stdI = gfanlib_kStd_wrapper(I,currRing);
        else
          stdI = id_Copy(I,currRing);
        tropicalStrategy currentStrategy(stdI,p,currRing);
        gfan::ZFan* tropI = tropicalVariety(currentStrategy);
        res->rtyp = fanID;
        res->data = (char*) tropI;
        id_Delete(&stdI,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
    return FALSE;
  }
  WerrorS("tropicalVariety: unexpected parameters");
  return TRUE;
}
