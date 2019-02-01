#include "misc/options.h"
#include "bbfan.h"

#include "groebnerCone.h"
#include "startingCone.h"
#include "tropicalTraversal.h"


VAR BITSET groebnerBitsetSave1, groebnerBitsetSave2;

/***
 * sets option(redSB)
 **/
static void setOptionRedSB()
{
  SI_SAVE_OPT(groebnerBitsetSave1,groebnerBitsetSave2);
  si_opt_1|=Sy_bit(OPT_REDSB);
}

/***
 * sets option(noredSB);
 **/
static void undoSetOptionRedSB()
{
  SI_RESTORE_OPT(groebnerBitsetSave1,groebnerBitsetSave2);
}

gfan::ZFan* groebnerFan(const tropicalStrategy currentStrategy)
{
  groebnerCone startingCone = groebnerStartingCone(currentStrategy);
  groebnerCones groebnerFan = groebnerTraversal(startingCone);
  return toFanStar(groebnerFan);
}


gfan::ZFan* groebnerFanOfPolynomial(poly g, ring r, bool onlyLowerHalfSpace=false)
{
  int n = rVar(r);

  if (g==NULL || g->next==NULL)
  {
    // if g is a term or zero, return the fan consisting of the whole weight space
    gfan::ZFan* zf = new gfan::ZFan(gfan::ZFan::fullFan(n));
    return zf;
  }
  else
  {
    gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
    lowerHalfSpaceCondition[0] = -1;

    // otherwise, obtain a list of all the exponent vectors
    int* expv = (int*) omAlloc((n+1)*sizeof(int));
    gfan::ZMatrix exponents = gfan::ZMatrix(0,n);
    for (poly s=g; s; pIter(s))
    {
      p_GetExpV(s,expv,r);
      gfan::ZVector zv = intStar2ZVector(n,expv);
      exponents.appendRow(intStar2ZVector(n,expv));
    }
    omFreeSize(expv,(n+1)*sizeof(int));

    // and construct the Groebner fan
    gfan::ZFan* zf = new gfan::ZFan(n);
    int l = exponents.getHeight();
    for (int i=0; i<l; i++)
    {
      // constructs the Groebner cone such that initial form is i-th term
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      if (onlyLowerHalfSpace)
        inequalities.appendRow(lowerHalfSpaceCondition);
      for (int j=0; j<l; j++)
      {
        if (i!=j)
          inequalities.appendRow(exponents[i].toVector()-exponents[j].toVector());
      }
      gfan::ZCone zc = gfan::ZCone(inequalities,gfan::ZMatrix(0,inequalities.getWidth()));
      zc.canonicalize();
      zf->insert(zc);
    }
    return zf;
  }
}


BOOLEAN groebnerFan(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    leftv v = u->next;

    if (v==NULL)
    {
      if ((I->m[0]!=NULL) && (idElem(I)==1))
      {
        try
        {
          poly g = I->m[0];
          gfan::ZFan* zf = groebnerFanOfPolynomial(g,currRing);
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
          tropicalStrategy currentStrategy(I,currRing);
          setOptionRedSB();
          gfan::ZFan* zf = groebnerFan(currentStrategy);
          undoSetOptionRedSB();
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
  if ((u!=NULL) && (u->Typ()==POLY_CMD))
  {
    poly g = (poly) u->Data();
    leftv v = u->next;
    if (v==NULL)
    {
      try
      {
        gfan::ZFan* zf = groebnerFanOfPolynomial(g,currRing);
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
  WerrorS("groebnerFan: unexpected parameters");
  return TRUE;
}
