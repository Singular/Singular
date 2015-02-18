#include <misc/options.h>
#include <bbfan.h>

#include <groebnerCone.h>
#include <startingCone.h>
#include <groebnerFan.h>
#include <tropicalTraversal.h>

#include <ppinitialReduction.h>

gfan::ZFan* groebnerComplex(const tropicalStrategy currentStrategy)
{
  groebnerCone startingCone = groebnerStartingCone(currentStrategy);
  groebnerCones tropicalVariety = groebnerTraversal(startingCone);
  return toFanStar(tropicalVariety);
}


BOOLEAN groebnerComplex(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->Data();
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      leftv w = v->next;
      if (w==NULL)
      {
        number p = (number) v->Data();
        tropicalStrategy currentStrategy(I,p,currRing);
        if (idSize(I)==1)
        {
          try
          {
            ideal startingIdeal = currentStrategy.getStartingIdeal();
            ring startingRing = currentStrategy.getStartingRing();
            currentStrategy.pReduce(startingIdeal,startingRing);
            poly g = startingIdeal->m[0];
            pReduceInhomogeneous(g, currentStrategy.getUniformizingParameter(), startingRing);
            gfan::ZFan* zf = groebnerFanOfPolynomial(g,startingRing,true);
            res->rtyp = fanID;
            res->data = (char*) zf;
            return FALSE;
          }
          catch (const std::exception& ex)
          {
            WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
            return TRUE;
          }
        }
        else
        {
          try
          {
            gfan::ZFan* zf = groebnerComplex(currentStrategy);
            res->rtyp = fanID;
            res->data = (char*) zf;
            return FALSE;
          }
          catch (const std::exception& ex)
          {
            WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
            return TRUE;
          }
        }
      }
    }
  }
  if ((u!=NULL) && (u->Typ()==POLY_CMD))
  {
    poly g = (poly) u->Data();
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      leftv w = v->next;
      if (w==NULL)
      {
        try
        {
          number p = (number) v->Data();
          ideal I = idInit(1);
          I->m[0] = p_Copy(g,currRing);
          tropicalStrategy currentStrategy(I,p,currRing);
          ideal startingIdeal = currentStrategy.getStartingIdeal();
          ring startingRing = currentStrategy.getStartingRing();
          poly gg = startingIdeal->m[0];
          pReduceInhomogeneous(gg, currentStrategy.getUniformizingParameter(), startingRing);
          gfan::ZFan* zf = groebnerFanOfPolynomial(gg,startingRing,true);
          id_Delete(&I,currRing);
          res->rtyp = fanID;
          res->data = (char*) zf;
          return FALSE;
        }
        catch (const std::exception& ex)
        {
          WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
          return TRUE;
        }
      }
    }
  }
  WerrorS("groebnerComplex: unexpected parameters");
  return TRUE;
}
