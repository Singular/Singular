#include "kernel/mod2.h"

#ifdef HAVE_POLYMAKE
#ifndef POLYMAKE_VERSION
#define POLYAMKE_VERSION POLYMAKEVERSION
#endif

#include "Singular/dyn_modules/gfanlib/bbcone.h"
#include "Singular/dyn_modules/gfanlib/bbfan.h"
#include "Singular/dyn_modules/gfanlib/bbpolytope.h"

#include "Singular/blackbox.h"
#include "Singular/ipshell.h"
#include "Singular/subexpr.h"
#include "Singular/mod_lib.h"

#include <polymake/client.h>
#include <polymake_conversion.h>
#include <polymake_documentation.h>
#include <polymake/Graph.h>

polymake::Main* init_polymake=NULL;

static BOOLEAN bbpolytope_Op2(int op, leftv res, leftv i1, leftv i2)
{
  gfan::ZCone* zp = (gfan::ZCone*) i1->Data();
  switch(op)
  {
    case '+':
    {
      if (i2->Typ()==polytopeID || i2->Typ()==coneID)
      {
        gfan::initializeCddlibIfRequired();
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        gfan::ZCone* ms;
        try
        {
          polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
          polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
          polymake::perl::Object pms;
          #if (POLYMAKE_VERSION >= 305)
          polymake::call_function("minkowski_sum", *pp, *pq) >> pms;
          #else
          CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
          #endif
          ms = PmPolytope2ZPolytope(&pms);
          delete pp;
          delete pq;
        }
        catch (const std::exception& ex)
        {
          gfan::deinitializeCddlibIfRequired();
          WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
          return TRUE;
        }
        gfan::deinitializeCddlibIfRequired();
        res->rtyp = polytopeID;
        res->data = (void*) ms;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    case '*':
    {
      if (i2->Typ()==INT_CMD)
      {
        gfan::initializeCddlibIfRequired();
        int s = (int)(long) i2->Data();
        gfan::ZMatrix zm = zp->extremeRays();
        for (int i=0; i<zm.getHeight(); i++)
          for (int j=1; j<zm.getWidth(); j++)
            zm[i][j] *= s;
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
        gfan::deinitializeCddlibIfRequired();
        res->rtyp = polytopeID;
        res->data = (void*) zs;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    case '&':
    {
      if (i2->Typ()==polytopeID)
      {
        gfan::initializeCddlibIfRequired();
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
          gfan::deinitializeCddlibIfRequired();
          WerrorS("mismatching ambient dimensions");
          return TRUE;
        }
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::intersection(*zp, *zq);
        zs->canonicalize();
        gfan::deinitializeCddlibIfRequired();
        res->rtyp = polytopeID;
        res->data = (void*) zs;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    case '|':
    {
      if(i2->Typ()==polytopeID)
      {
        gfan::initializeCddlibIfRequired();
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
          gfan::deinitializeCddlibIfRequired();
          WerrorS("mismatching ambient dimensions");
          return TRUE;
        }
        gfan::ZMatrix rays = zp->extremeRays();
        rays.append(zq->extremeRays());
        gfan::ZMatrix lineality = zp->generatorsOfLinealitySpace();
        lineality.append(zq->generatorsOfLinealitySpace());
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::ZCone::givenByRays(rays,lineality);
        zs->canonicalize();
        gfan::deinitializeCddlibIfRequired();
        res->rtyp = polytopeID;
        res->data = (void*) zs;
        return FALSE;
      }
    return blackboxDefaultOp2(op,res,i1,i2);
    }
    case EQUAL_EQUAL:
    {
      if(i2->Typ()==polytopeID)
      {
        gfan::initializeCddlibIfRequired();
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        zp->canonicalize();
        zq->canonicalize();
        bool b = !((*zp)!=(*zq));
        gfan::deinitializeCddlibIfRequired();
        res->rtyp = INT_CMD;
        res->data = (char*) (long) b;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    default:
      return blackboxDefaultOp2(op,res,i1,i2);
  }
  return blackboxDefaultOp2(op,res,i1,i2);
}


/* Functions for using Polymake in Singular */

// BOOLEAN cube(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u !=NULL) && (u->Typ() == INT_CMD))
//     {
//       int ambientDim = (int)(long)u->Data();
//       if (ambientDim < 0)
//         {
//           Werror("expected non-negative ambient dim but got %d", ambientDim);
//           return TRUE;
//         }
//       gfan::ZMatrix zm(ambientDim*2,ambientDim+1);
//       int j=1;
//       for (int i=0; i<ambientDim*2; i=i+2)
//         {
//           zm[i][0] = 1;
//           zm[i][j] = 1;
//           zm[i+1][0] = 1;
//           zm[i+1][j] = -1;
//           j = j+1;
//         }
//       gfan::ZCone* zc = new gfan::ZCone(zm, gfan::ZMatrix(0, zm.getWidth()));
//       res->rtyp = coneID;
//       res->data = (char *)zc;
//       return FALSE;
//     }
//   WerrorS("cube: unexpected parameters");
//   return TRUE;
// }

// BOOLEAN cross(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u !=NULL) && (u->Typ() == INT_CMD))
//     {
//       int ambientDim = (int)(long)u->Data();
//       if (ambientDim < 0)
//         {
//           Werror("expected non-negative ambient dim but got %d", ambientDim);
//           return TRUE;
//         }
//       gfan::ZMatrix zm(ambientDim*2,ambientDim+1);
//       int j=1;
//       for (int i=0; i<ambientDim*2; i=i+2)
//         {
//           zm[i][0] = 1;
//           zm[i][j] = 1;
//           zm[i+1][0] = 1;
//           zm[i+1][j] = -1;
//           j = j+1;
//         }
//       gfan::ZCone* zc = new gfan::ZCone();
//       *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
//       res->rtyp = coneID;
//       res->data = (char *)zc;
//       return FALSE;
//     }
//   WerrorS("cross: unexpected parameters");
//   return TRUE;
// }


BOOLEAN PMisLatticePolytope(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("Lattice");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isLatticePolytope: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisBounded(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("BOUNDED");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isBounded: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisReflexive(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("REFLEXIVE");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isReflexive: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisGorenstein(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("GORENSTEIN");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isGorenstein: unexpected parameters");
  return TRUE;
}


BOOLEAN PMgorensteinIndex(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int gi;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      bool b = p->give("GORENSTEIN");
      if (b)
      {
        polymake::Integer pgi = p->give("GORENSTEIN_INDEX");
        gi = PmInteger2Int(pgi,ok);
        delete p;
      }
      else
      {
        delete p;
        gfan::deinitializeCddlibIfRequired();
        WerrorS("gorensteinIndex: input polytope not gorenstein");
        return TRUE;
      }
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) gi;
    return FALSE;
  }
  WerrorS("gorensteinIndex: unexpected parameters");
  return TRUE;
}


BOOLEAN PMgorensteinVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* gv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      bool b = p->give("GORENSTEIN");
      if (b)
      {
        polymake::Vector<polymake::Integer> pgv = p->give("GORENSTEIN_VECTOR");
        gv = PmVectorInteger2Intvec(&pgv,ok);
        delete p;
      }
      else
      {
        delete p;
        gfan::deinitializeCddlibIfRequired();
        WerrorS("gorensteinVector: input polytope not gorenstein");
        return TRUE;
      }
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("gorensteinVector: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) gv;
    return FALSE;
  }
  WerrorS("gorensteinVector: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisCanonical(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("CANONICAL");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isCanonical: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisTerminal(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("TERMINAL");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isTerminal: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisLatticeEmpty(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("LATTICE_EMPTY");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isLatticeEmpty: unexpected parameters");
  return TRUE;
}


BOOLEAN PMlatticeVolume(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int lv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer plv = p->give("LATTICE_VOLUME");
      delete p;
      lv = PmInteger2Int(plv,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) lv;
    return FALSE;
  }
  WerrorS("latticeVolume: unexpected parameters");
  return TRUE;
}


BOOLEAN PMlatticeDegree(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int ld;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer pld = p->give("LATTICE_DEGREE");
      delete p;
      ld = PmInteger2Int(pld,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) ld;
    return FALSE;
  }
  WerrorS("latticeDegree: unexpected parameters");
  return TRUE;
}


BOOLEAN PMlatticeCodegree(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int lc;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer plc = p->give("LATTICE_CODEGREE");
      delete p;
      lc = PmInteger2Int(plc,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) lc;
    return FALSE;
  }
  WerrorS("latticeCodegree: unexpected parameters");
  return TRUE;
}


BOOLEAN PMehrhartPolynomialCoeff(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* ec;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> pec = p->give("EHRHART_POLYNOMIAL_COEFF");
      delete p;
      ec = PmVectorInteger2Intvec(&pec,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("ehrhartPolynomialCoeff: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) ec;
    return FALSE;
  }
  WerrorS("ehrhartPolynomialCoeff: unexpected parameters");
  return TRUE;
}


BOOLEAN PMfVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* hv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> phv = p->give("F_VECTOR");
      delete p;
      hv = PmVectorInteger2Intvec(&phv,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("fVectorP: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) hv;
    return FALSE;
  }
  WerrorS("fVectorP: unexpected parameters");
  return TRUE;
}


BOOLEAN PMhVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* hv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> phv = p->give("H_VECTOR");
      delete p;
      hv = PmVectorInteger2Intvec(&phv,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("hVector: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) hv;
    return FALSE;
  }
  WerrorS("hVector: unexpected parameters");
  return TRUE;
}


BOOLEAN PMhStarVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* hv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> phv = p->give("H_STAR_VECTOR");
      delete p;
      hv = PmVectorInteger2Intvec(&phv,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("hStarVector: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) hv;
    return FALSE;
  }
  WerrorS("hStarVector: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisNormal(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("NORMAL");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isNormal: unexpected parameters");
  return TRUE;
}


BOOLEAN PMfacetWidths(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* fw;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> pfw = p->give("FACET_WIDTHS");
      delete p;
      fw = PmVectorInteger2Intvec(&pfw,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("facetWidths: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) fw;
    return FALSE;
  }
  WerrorS("facetWidths: unexpected parameters");
  return TRUE;
}


BOOLEAN PMfacetWidth(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int fw;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer pfw = p->give("FACET_WIDTH");
      delete p;
      fw = PmInteger2Int(pfw,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) fw;
    return FALSE;
  }
  WerrorS("facetWidth: unexpected parameters");
  return TRUE;
}


BOOLEAN PMfacetVertexLatticeDistances(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* ld;
    bool ok=true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> pld = p->give("FACET_VERTEX_LATTICE_DISTANCES");
      delete p;
      ld = PmMatrixInteger2Intvec(&pld,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INTMAT_CMD;
    res->data = (char*) ld;
    return FALSE;
  }
  WerrorS("facetVertexLatticeDistances: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisCompressed(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("COMPRESSED");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isCompressed: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisSmooth(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZCone2PmCone(zc);
      b = p->give("SMOOTH_CONE");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("SMOOTH");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZFan2PmFan(zf);
      b = p->give("SMOOTH_FAN");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isSmooth: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisVeryAmple(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      b = p->give("VERY_AMPLE");
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = INT_CMD;
    res->data = (char*) (long) b;
    return FALSE;
  }
  WerrorS("isVeryAmple: unexpected parameters");
  return TRUE;
}


BOOLEAN PMlatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      #if (POLYMAKEVERSION >=305)
      polymake::Matrix<polymake::Integer> lp = p->call_method("LATTICE_POINTS");
      #elif (POLYMAKEVERSION >=214)
      polymake::Matrix<polymake::Integer> lp = p->CallPolymakeMethod("LATTICE_POINTS");
      #elif (POLYMAKEVERSION >=212)
      polymake::Matrix<polymake::Integer> lp = p->give("LATTICE_POINTS");
      #else
        #error polymake version too old
      #endif
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INTMAT_CMD;
    res->data = (char*) iv;
    return FALSE;
  }
  WerrorS("LatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMnLatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int n;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p->give("N_LATTICE_POINTS");
      delete p;
      n = PmInteger2Int(nlp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) n;
    return FALSE;
  }
  WerrorS("nLatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMinteriorLatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p->give("INTERIOR_LATTICE_POINTS");
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INTMAT_CMD;
    res->data = (char*) iv;
    return FALSE;
  }
  WerrorS("interiorLatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMnInteriorLatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int n;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p->give("N_INTERIOR_LATTICE_POINTS");
      delete p;
      n = PmInteger2Int(nlp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) n;
    return FALSE;
  }
  WerrorS("nInteriorLatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMboundaryLatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p->give("BOUNDARY_LATTICE_POINTS");
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INTMAT_CMD;
    res->data = (char*) iv;
    return FALSE;
  }
  WerrorS("boundaryLatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMnBoundaryLatticePoints(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int n;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p->give("N_BOUNDARY_LATTICE_POINTS");
      delete p;
      n = PmInteger2Int(nlp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) n;
    return FALSE;
  }
  WerrorS("nBoundaryLatticePoints: unexpected parameters");
  return TRUE;
}


BOOLEAN PMhilbertBasis(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      #if (POLYMAKEVERSION >=305)
      polymake::Matrix<polymake::Integer> lp = p->call_method("HILBERT_BASIS");
      #elif (POLYMAKEVERSION >=214)
      polymake::Matrix<polymake::Integer> lp = p->CallPolymakeMethod("HILBERT_BASIS");
      #elif (POLYMAKEVERSION >=212)
      polymake::Matrix<polymake::Integer> lp = p->give("HILBERT_BASIS");
      #else
        #error polymake version too old
      #endif
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INTMAT_CMD;
    res->data = (char*) iv;
    return FALSE;
  }
  WerrorS("hilbertBasis: unexpected parameters");
  return TRUE;
}


BOOLEAN PMnHilbertBasis(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int n;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p->give("N_HILBERT_BASIS");
      delete p;
      n = PmInteger2Int(nlp,ok);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (long) n;
    return FALSE;
  }
  WerrorS("nHilbertBasis: unexpected parameters");
  return TRUE;
}


BOOLEAN PMminkowskiSum(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      gfan::ZCone* zq = (gfan::ZCone*)v->Data();
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        #if (POLYMAKE_VERSION >= 305)
        polymake::call_function("minkowski_sum", *pp, *pq) >> pms;
        #else
        CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
        #endif
        delete pp;
        delete pq;
        ms = PmPolytope2ZPolytope(&pms);
      }
      catch (const std::exception& ex)
      {
        gfan::deinitializeCddlibIfRequired();
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        return TRUE;
      }
      gfan::deinitializeCddlibIfRequired();
      res->rtyp = polytopeID;
      res->data = (char*) ms;
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      gfan::ZCone* zq = new gfan::ZCone(liftUp(*zc));
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        #if (POLYMAKE_VERSION >= 305)
        polymake::call_function("minkowski_sum", *pp, *pq) >> pms;
        #else
        CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
        #endif
        delete pp;
        delete pq;
        ms = PmPolytope2ZPolytope(&pms);
      }
      catch (const std::exception& ex)
      {
        delete zq;
        gfan::deinitializeCddlibIfRequired();
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        return TRUE;
      }
      delete zq;
      gfan::deinitializeCddlibIfRequired();
      res->rtyp = polytopeID;
      res->data = (char*) ms;
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zp = new gfan::ZCone(liftUp(*zc));
      gfan::ZCone* zq = (gfan::ZCone*)v->Data();
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        #if (POLYMAKE_VERSION >= 305)
        polymake::call_function("minkowski_sum", *pp, *pq) >> pms;
        #else
        CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
        #endif
        delete pp;
        delete pq;
        ms = PmPolytope2ZPolytope(&pms);
      }
      catch (const std::exception& ex)
      {
        delete zp;
        gfan::deinitializeCddlibIfRequired();
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        return TRUE;
      }
      delete zp;
      gfan::deinitializeCddlibIfRequired();
      res->rtyp = polytopeID;
      res->data = (char*) ms;
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      gfan::ZCone* zq = (gfan::ZCone*)v->Data();
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        #if (POLYMAKE_VERSION >= 305)
        polymake::call_function("minkowski_sum", *pp, *pq) >> pms;
        #else
        CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
        #endif
        delete pp;
        delete pq;
        ms = PmPolytope2ZPolytope(&pms);
      }
      catch (const std::exception& ex)
      {
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::deinitializeCddlibIfRequired();
      res->rtyp = coneID;
      res->data = (char*) ms;
      return FALSE;
    }
  }
  WerrorS("minkowskiSum: unexpected parameters");
  return TRUE;
}


polymake::Matrix<polymake::Integer> verticesOf(const polymake::perl::Object* p,
                                               const polymake::Set<polymake::Integer>* s)
{
  polymake::Matrix<polymake::Integer> allrays = p->give("VERTICES");
  polymake::Matrix<polymake::Integer> wantedrays;
  bool ok = true;
  #if (POLYMAKE_VERSION >= 305)
  for(const auto i : *s)
  {
    wantedrays = wantedrays / allrays.row(PmInteger2Int(i,ok));
  }
  #else
  for(polymake::Entire<polymake::Set<polymake::Integer> >::const_iterator i=polymake::entire(*s); !i.at_end(); i++)
  {
    wantedrays = wantedrays / allrays.row(PmInteger2Int(*i,ok));
  }
  #endif
  if (!ok)
  {
    WerrorS("overflow while converting polymake::Integer to int in raysOf");
  }
  return wantedrays;
}


BOOLEAN PMmaximalFace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      intvec* iv = (intvec*) v->Data();
      intvec* maxface;
      bool ok = true;
      try
      {
        polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
        p->take("LP") << o;
        polymake::Set<polymake::Integer> mf = p->give("LP.MAXIMAL_FACE");
        polymake::Matrix<polymake::Integer> vertices = verticesOf(p,&mf);
        delete p;
        maxface = new intvec(PmMatrixInteger2Intvec(&vertices,ok));
      }
      catch (const std::exception& ex)
      {
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::deinitializeCddlibIfRequired();
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
      res->rtyp = INTVEC_CMD;
      res->data = (char*) maxface;
      return FALSE;
    }
  }
  WerrorS("maximalFace: unexpected parameters");
  return TRUE;
}


BOOLEAN PMminimalFace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      intvec* iv = (intvec*) v->Data();
      intvec* minface;
      bool ok = true;
      try
      {
        polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
        p->take("LP") << o;
        polymake::Set<polymake::Integer> mf = p->give("LP.MINIMAL_FACE");
        polymake::Matrix<polymake::Integer> vertices = verticesOf(p,&mf);
        delete p;
        minface = new intvec(PmMatrixInteger2Intvec(&vertices,ok));
      }
      catch (const std::exception& ex)
      {
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::deinitializeCddlibIfRequired();
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
      res->rtyp = INTVEC_CMD;
      res->data = (char*) minface;
      return FALSE;
    }
  }
  WerrorS("minimalFace: unexpected parameters");
  return TRUE;
}


BOOLEAN PMmaximalValue(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      intvec* iv = (intvec*) v->Data();
      if (iv->rows()==zp->ambientDimension())
      {
        int m;
        bool ok = true;
        try
        {
          polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
          polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
          polymake::perl::Object o("LinearProgram<Rational>");
          o.take("LINEAR_OBJECTIVE") << lo;
          p->take("LP") << o;
          polymake::Integer mv = p->give("LP.MAXIMAL_VALUE");
          delete p;
          m = PmInteger2Int(mv,ok);
        }
        catch (const std::exception& ex)
        {
          WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        gfan::deinitializeCddlibIfRequired();
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
        res->rtyp = INT_CMD;
        res->data = (char*) (long) m;
        return FALSE;
      }
    }
    WerrorS("maximalValue: vector is of wrong size");
    return TRUE;
  }
  WerrorS("maximalValue: unexpected parameters");
  return TRUE;
}

BOOLEAN PMminimalValue(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      intvec* iv = (intvec*) v->Data();
      if (iv->rows()==zp->ambientDimension())
      {
        int m;
        bool ok = true;
        try
        {
          polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
          polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
          polymake::perl::Object o("LinearProgram<Rational>");
          o.take("LINEAR_OBJECTIVE") << lo;
          p->take("LP") << o;
          polymake::Integer mv = p->give("LP.MINIMAL_VALUE");
          delete p;
          m = PmInteger2Int(mv,ok);
        }
        catch (const std::exception& ex)
        {
          WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        gfan::deinitializeCddlibIfRequired();
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
        res->rtyp = INT_CMD;
        res->data = (char*) (long) m;
        return FALSE;
      }
    }
    WerrorS("minimalValue: vector is of wrong size");
    return TRUE;
  }
  WerrorS("minimalValue: unexpected parameters");
  return TRUE;
}


BOOLEAN visual(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    try
    {
      polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
      #if (POLYMAKE_VERSION >= 305)
      polymake::call_function("jreality",pp->call_method("VISUAL"));
      #else
      VoidCallPolymakeFunction("jreality",pp->CallPolymakeMethod("VISUAL"));
      #endif
      delete pp;
    }
    catch (const std::exception& ex)
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      gfan::deinitializeCddlibIfRequired();
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = NONE;
    res->data = NULL;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    try
    {
      polymake::perl::Object* pf=ZFan2PmFan(zf);
      #if (POLYMAKE_VERSION >= 305)
      polymake::call_function("jreality",pf->call_method("VISUAL"));
      #else
      VoidCallPolymakeFunction("jreality",pf->CallPolymakeMethod("VISUAL"));
      #endif
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = NONE;
    res->data = NULL;
    return FALSE;
  }
  WerrorS("visual: unexpected parameters");
  return TRUE;
}

BOOLEAN normalFan(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    gfan::ZFan* zf = new gfan::ZFan(0);
    try
    {
      polymake::perl::Object* p=ZPolytope2PmPolytope(zp);
      polymake::perl::Object pf;
      #if (POLYMAKE_VERSION >= 305)
      polymake::call_function("normal_fan", *p) >> pf;
      #else
      CallPolymakeFunction("normal_fan",*p) >> pf;
      #endif
      delete p;
      zf = PmFan2ZFan(&pf);
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = fanID;
    res->data = (char*) zf;
    return FALSE;
  }
  WerrorS("normalFan: unexpected parameters");
  return TRUE;
}

BOOLEAN PMconeViaRays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    gfan::initializeCddlibIfRequired();
    polymake::perl::Object pc("Cone<Rational>");
    intvec* hlines = (intvec*) u->Data(); // these will are half lines in the cone
    polymake::Matrix<polymake::Integer> pmhlines = Intvec2PmMatrixInteger(hlines);
    pc.take("INPUT_RAYS") << pmhlines;

    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTMAT_CMD))
    {
      intvec* lines = (intvec*) v->Data(); // these will be lines in the cone
      polymake::Matrix<polymake::Integer> pmlines = Intvec2PmMatrixInteger(lines);
      pc.take("INPUT_LINEALITY") << pmlines;

      // leftv w = v->next;
      // if ((w != NULL) && (w->Typ() == INT_CMD))
      // {
      //   int flag = (int) (long) w->Data(); // TODO: this will indicate whether the
      //                                      // information provided are exact
      // }
    }
    gfan::ZCone* zc = PmCone2ZCone(&pc);
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = coneID;
    res->data = (char*) zc;
    return FALSE;
  }
  WerrorS("coneViaRays: unexpected parameters");
  return TRUE;
}


BOOLEAN PMpolytopeViaVertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    gfan::initializeCddlibIfRequired();
    polymake::perl::Object pp("Polytope<Rational>");
    intvec* points = (intvec*) u->Data(); // these will be vertices of or points in the polytope
    polymake::Matrix<polymake::Integer> pmpoints = Intvec2PmMatrixInteger(points);

    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      int flag = (int) (long) v->Data();
      switch(flag)
      {
        case 0:  pp.take("POINTS") << pmpoints;   // case means the matrix may contain points inside the polytope
        case 1:  pp.take("VERTICES") << pmpoints; // case means the matrix only contains vertices of the polytope
        default: WerrorS("polytopeViaVertices: invalid flag");
      }
    }
    else
      pp.take("POINTS") << pmpoints;              // by default, we assume that matrix may contain non-vertices

    gfan::ZCone* zp = PmPolytope2ZPolytope(&pp);
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = polytopeID;
    res->data = (char*) zp;
    return FALSE;
  }
  WerrorS("polytopeViaVertices: unexpected parameters");
  return TRUE;
}


BOOLEAN PMvertexAdjacencyGraph(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*) u->Data();
    lists output=(lists)omAllocBin(slists_bin); output->Init(2);
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> vert0 = p->give("VERTICES");
      bigintmat* vert1 = PmMatrixInteger2Bigintmat(&vert0);
      output->m[0].rtyp = BIGINTMAT_CMD;
      output->m[0].data = (void*) vert1;

      polymake::Graph<> gr=p->give("GRAPH.ADJACENCY");
      polymake::IncidenceMatrix<polymake::NonSymmetric> adj = adjacency_matrix(gr);
      lists listOfEdges = PmIncidenceMatrix2ListOfIntvecs(&adj);
      output->m[1].rtyp = LIST_CMD;
      output->m[1].data = (void*) listOfEdges;
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = LIST_CMD;
    res->data = (void*) output;
    return FALSE;
  }
  WerrorS("vertexEdgeGraph: unexpected parameters");
  return TRUE;
}


BOOLEAN PMvertexEdgeGraph(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*) u->Data();
    lists output=(lists)omAllocBin(slists_bin); output->Init(2);
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> vert0 = p->give("VERTICES");
      bigintmat* vert1 = PmMatrixInteger2Bigintmat(&vert0);
      output->m[0].rtyp = BIGINTMAT_CMD;
      output->m[0].data = (void*) vert1;

      polymake::Graph<> gr=p->give("GRAPH.ADJACENCY");
      polymake::IncidenceMatrix<polymake::NonSymmetric> adj = adjacency_matrix(gr);
      lists listOfEdges = PmAdjacencyMatrix2ListOfEdges(&adj);
      output->m[1].rtyp = LIST_CMD;
      output->m[1].data = (void*) listOfEdges;
      delete p;
    }
    catch (const std::exception& ex)
    {
      gfan::deinitializeCddlibIfRequired();
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    gfan::deinitializeCddlibIfRequired();
    res->rtyp = LIST_CMD;
    res->data = (void*) output;
    return FALSE;
  }
  WerrorS("vertexEdgeGraph: unexpected parameters");
  return TRUE;
}

#include <omp.h>
// extern "C" void omp_set_num_threads(int num_threads);

extern "C" int SI_MOD_INIT(polymake)(SModulFunctions* p)
{
  omp_set_num_threads(1); // avoid multiple threads within polymake/libnormaliz
  if (init_polymake==NULL)
    {init_polymake = new polymake::Main();}
  init_polymake->set_application("fan");
  // p->iiAddCproc("polymake.so","coneViaPoints",FALSE,PMconeViaRays);
  // p->iiAddCproc("polymake.so","polytopeViaPoints",FALSE,PMpolytopeViaVertices);
  p->iiAddCproc("polymake.lib","isLatticePolytope",FALSE,PMisLatticePolytope);
  p->iiAddCproc("polymake.lib","isBounded",FALSE,PMisBounded);
  p->iiAddCproc("polymake.lib","isReflexive",FALSE,PMisReflexive);
  p->iiAddCproc("polymake.lib","isGorenstein",FALSE,PMisGorenstein);
  p->iiAddCproc("polymake.lib","gorensteinIndex",FALSE,PMgorensteinIndex);
  p->iiAddCproc("polymake.lib","gorensteinVector",FALSE,PMgorensteinVector);
  p->iiAddCproc("polymake.lib","isCanonical",FALSE,PMisCanonical);
  p->iiAddCproc("polymake.lib","isTerminal",FALSE,PMisTerminal);
  p->iiAddCproc("polymake.lib","isLatticeEmpty",FALSE,PMisLatticeEmpty);
  p->iiAddCproc("polymake.lib","latticeVolume",FALSE,PMlatticeVolume);
  p->iiAddCproc("polymake.lib","latticeDegree",FALSE,PMlatticeDegree);
  p->iiAddCproc("polymake.lib","latticeCodegree",FALSE,PMlatticeCodegree);
  p->iiAddCproc("polymake.lib","ehrhartPolynomialCoeff",FALSE,PMehrhartPolynomialCoeff);
  p->iiAddCproc("polymake.lib","fVectorP",FALSE,PMfVector);
  p->iiAddCproc("polymake.lib","hVector",FALSE,PMhVector);
  p->iiAddCproc("polymake.lib","hStarVector",FALSE,PMhStarVector);
  p->iiAddCproc("polymake.lib","isNormal",FALSE,PMisNormal);
  p->iiAddCproc("polymake.lib","facetWidths",FALSE,PMfacetWidths);
  p->iiAddCproc("polymake.lib","facetWidth",FALSE,PMfacetWidth);
  p->iiAddCproc("polymake.lib","facetVertexLatticeDistances",FALSE,PMfacetVertexLatticeDistances);
  p->iiAddCproc("polymake.lib","isCompressed",FALSE,PMisCompressed);
  p->iiAddCproc("polymake.lib","isSmooth",FALSE,PMisSmooth);
  p->iiAddCproc("polymake.lib","isVeryAmple",FALSE,PMisVeryAmple);
  p->iiAddCproc("polymake.lib","latticePoints",FALSE,PMlatticePoints);
  p->iiAddCproc("polymake.lib","nLatticePoints",FALSE,PMnLatticePoints);
  p->iiAddCproc("polymake.lib","interiorLatticePoints",FALSE,PMinteriorLatticePoints);
  p->iiAddCproc("polymake.lib","nInteriorLatticePoints",FALSE,PMnInteriorLatticePoints);
  p->iiAddCproc("polymake.lib","boundaryLatticePoints",FALSE,PMboundaryLatticePoints);
  p->iiAddCproc("polymake.lib","nBoundaryLatticePoints",FALSE,PMnBoundaryLatticePoints);
  p->iiAddCproc("polymake.lib","hilbertBasis",FALSE,PMhilbertBasis);
  p->iiAddCproc("polymake.lib","nHilbertBasis",FALSE,PMnHilbertBasis);
  p->iiAddCproc("polymake.lib","minkowskiSum",FALSE,PMminkowskiSum);
  p->iiAddCproc("polymake.lib","maximalFace",FALSE,PMmaximalFace);
  p->iiAddCproc("polymake.lib","minimalFace",FALSE,PMminimalFace);
  p->iiAddCproc("polymake.lib","maximalValue",FALSE,PMmaximalValue);
  p->iiAddCproc("polymake.lib","minimalValue",FALSE,PMminimalValue);
  p->iiAddCproc("polymake.lib","visual",FALSE,visual);
  p->iiAddCproc("polymake.lib","normalFan",FALSE,normalFan);
  p->iiAddCproc("polymake.lib","vertexAdjacencyGraph",FALSE,PMvertexAdjacencyGraph);
  p->iiAddCproc("polymake.lib","vertexEdgeGraph",FALSE,PMvertexEdgeGraph);

  blackbox* b=getBlackboxStuff(polytopeID);
  b->blackbox_Op2=bbpolytope_Op2;

  init_polymake_help();
  return MAX_TOK;
}

#endif /* HAVE_POLYMAKE */
