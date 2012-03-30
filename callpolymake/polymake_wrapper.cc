#include <polymake_conversion.h>
#include <polymake_documentation.h>

// #include <polymake/Main.h>
// #include <polymake/Matrix.h>
// #include <polymake/Rational.h>
// #include <polymake/Integer.h>
// #include <polymake/common/lattice_tools.h>
// #include <polymake/perl/macros.h>
// #include <polymake/Set.h>
// #include <polymake/IncidenceMatrix.h>

// #include <gfanlib/gfanlib.h>
// #include <gfanlib/gfanlib_q.h>

// #include <gmpxx.h>

// #include <kernel/mod2.h>
// #include <kernel/structs.h>
// #include <kernel/febase.h>
// #include <kernel/intvec.h>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

#include <Singular/blackbox.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>
// #include <Singular/tok.h>

using namespace polymake;


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
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        gfan::ZCone* ms;
        try
        {
          polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
          polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
          polymake::perl::Object pms;
          CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
          ms = PmPolytope2ZPolytope(&pms);
          delete pp, pq;
        }
        catch (const std::exception& ex) 
        {
          WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
          return TRUE;
        }
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
        int s = (int)(long) i2->Data();
        gfan::ZMatrix zm = zp->extremeRays();
        for (int i=0; i<zm.getHeight(); i++)
          for (int j=1; j<zm.getWidth(); j++)
            zm[i][j] *= s;
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
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
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
          Werror("mismatching ambient dimensions");
          return TRUE;
        }
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::intersection(*zp, *zq);
        zs->canonicalize();
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
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
          Werror("mismatching ambient dimensions");
          return TRUE;
        }
        gfan::ZMatrix rays = zp->extremeRays();
        rays.append(zq->extremeRays());
        gfan::ZMatrix lineality = zp->generatorsOfLinealitySpace();
        lineality.append(zq->generatorsOfLinealitySpace());
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::ZCone::givenByRays(rays,lineality);
        zs->canonicalize();
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
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        zp->canonicalize();
        zq->canonicalize();
        bool b = !((*zp)!=(*zq));
        res->rtyp = INT_CMD;
        res->data = (char*) (int) b;
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
// 	{
//           Werror("expected non-negative ambient dim but got %d", ambientDim);
// 	  return TRUE;
// 	}
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
// 	{
//           Werror("expected non-negative ambient dim but got %d", ambientDim);
// 	  return TRUE;
// 	}
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
        WerrorS("gorensteinIndex: input polytope not gorenstein");
        return TRUE;
      }
    }
    catch (const std::exception& ex)
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) gi;
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
        WerrorS("gorensteinVector: input polytope not gorenstein");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) lv;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) ld;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) lc;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("fVector: overflow in PmVectorInteger2Intvec");
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) hv;
    return FALSE;
  }
  WerrorS("fVector: unexpected parameters");
  return TRUE;
}


BOOLEAN PMhVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    { 
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) fw;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
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
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p->give("LATTICE_POINTS");
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok); 
    }
    catch (const std::exception& ex) 
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) n;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) n;
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) n;
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
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    bool ok = true;
    try
    {
      polymake::perl::Object* p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p->give("HILBERT_BASIS");
      delete p;
      iv = PmMatrixInteger2Intvec(&lp,ok); 
    }
    catch (const std::exception& ex) 
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
      return TRUE;
    }
    res->rtyp = INT_CMD;
    res->data = (char*) n;
    return FALSE;
  }
  WerrorS("nHilbertBasis: unexpected parameters");
  return TRUE;
}


BOOLEAN PMminkowskiSum(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == polytopeID) || (u->Typ() == coneID)))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == polytopeID) && (v->Typ() == coneID)))
    {
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      gfan::ZCone* zq = (gfan::ZCone*)v->Data();
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object* pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        CallPolymakeFunction("minkowski_sum", *pp, *pq) >> pms;
        delete pp, pq;
        ms = PmPolytope2ZPolytope(&pms);
      }
      catch (const std::exception& ex) 
      {
        WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
        return TRUE;
      }
      res->rtyp = polytopeID;
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
  for(Entire<Set<Integer> >::const_iterator i=polymake::entire(*s); !i.at_end(); i++)
  {
    wantedrays = wantedrays / allrays.row(PmInteger2Int(*i,ok));
  }
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
        return TRUE;
      }
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
        return TRUE;
      }
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
          return TRUE;
        }
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
        res->rtyp = INT_CMD;
        res->data = (char*) m;
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
          return TRUE;
        }
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
        res->rtyp = INT_CMD;
        res->data = (char*) m;
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
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    try
    {
      polymake::perl::Object* pp = ZPolytope2PmPolytope(zp);
      VoidCallPolymakeFunction("jreality",pp->CallPolymakeMethod("VISUAL"));
      delete pp;
    }
    catch (const std::exception& ex)
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = NONE;
    res->data = NULL;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    try
    {
      polymake::perl::Object* pf=ZFan2PmFan(zf);
      VoidCallPolymakeFunction("jreality",pf->CallPolymakeMethod("VISUAL"));
    }    
    catch (const std::exception& ex)
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
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
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    gfan::ZFan* zf = new gfan::ZFan();
    try
    {
      polymake::perl::Object* p=ZPolytope2PmPolytope(zp);
      polymake::perl::Object pf;
      CallPolymakeFunction("normal_fan", *p) >> pf;
      delete p;
      zf = PmFan2ZFan(&pf);
    }
    catch (const std::exception& ex) 
    {
      WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
      return TRUE;
    }
    res->rtyp = fanID;
    res->data = (char*) zf;
    return FALSE;
  }
  WerrorS("normalFan: unexpected parameters");
  return TRUE; 
}


// BOOLEAN testingtypes(leftv res, leftv args)
// {
//   leftv u = args;
//   if (u != NULL)
//   {
//     leftv v = u->next;
//     if (v != NULL)
//     {
//       leftv w = v->next;
//       if (w != NULL)
//       {
// 	Print("\n (u->Typ() ->) %d =?= %d (<-coneID) \n", u->Typ(),coneID);
// 	Print("\n (u->Typ() ->) %d =?= %d (<-fanID) \n", v->Typ(),fanID);
// 	Print("\n (u->Typ() ->) %d =?= %d (<-polytopeID) \n", w->Typ(),polytopeID);
// 	res->rtyp = NONE;
// 	res->data = NULL;
// 	return FALSE;
//       }
//     }
//   }
//   return TRUE;
// }


// BOOLEAN testingintvec(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ()==INTVEC_CMD))
//   {
//     intvec* iv = (intvec*)u->Data();
//     polymake::Vector<polymake::Integer> pmvec = Intvec2PmVectorInteger(iv);
//     res->rtyp = NONE;
//     res->data = NULL;
//     return FALSE;
//   }
//   return TRUE;
// }


// BOOLEAN testingfans(leftv res, leftv args)  // for testing purposes    
// {                                           // creating a fan in polymake 
//   leftv u = args;                           // and handing it to Singular
//   if ((u != NULL) && (u->Typ() == fanID))
//   {
//     gfan::ZFan* zf = (gfan::ZFan*) u->Data();
//     perl::Object pf = ZFan2PmFan(zf);
//     gfan::ZFan* zff = new gfan::ZFan(PmFan2ZFan(&pf));
//     res->rtyp = fanID;
//     res->data = (char *)zff;
//     return FALSE;
//   }
//   return TRUE;
// }

// BOOLEAN testingmatrices(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == coneID))
//     {
//       gfan::ZCone* zc = (gfan::ZCone*) u->Data();
//       gfan::ZMatrix zm = zc->getInequalities();
//       polymake::Matrix<polymake::Integer> pm = GfZMatrix2PmMatrixInteger(&zm);
//       gfan::ZMatrix zn = PmMatrixInteger2GfZMatrix(&pm);
//       res->rtyp = NONE;
//       res->data = NULL;
//       return FALSE;
//     }
//   return TRUE;
// }

// BOOLEAN testingcones(leftv res, leftv args)  // for testing purposes       
// {                                            // taking a cone from Singular,
//                                              // handing it over to polymake 
//                                              // and back                    
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == coneID))
//     {
//       gfan::ZCone* zc = (gfan::ZCone*) u->Data();
//       Print("converting gfan cone to polymake cone...\n");
//       polymake::perl::Object pc = ZCone2PmCone(zc);
//       Print("converting polymake cone to gfan cone...\n");
//       gfan::ZCone* zd = new gfan::ZCone(PmCone2ZCone(&pc));
//       // res->rtyp = coneID;
//       // res->data = (char *) zd;
//       res->rtyp = NONE;
//       res->data = NULL;
//       return FALSE;
//     }
//   return TRUE;
// }

// BOOLEAN testingpolytopes(leftv res, leftv args) // for testing purposes
// {                                                // taking a cone from Singular,
//                                                  // handing it over to polymake
//                                                  // and back
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == polytopeID))
//   {
//     gfan::ZCone* zp = (gfan::ZCone*) u->Data();
//     polymake::perl::Object pp = ZPolytope2PmPolytope(zp);
//     gfan::ZCone* zq = new gfan::ZCone(PmPolytope2ZPolytope(&pp));
//     res->rtyp = polytopeID;
//     res->data = (char *) zq;
//     return FALSE;
//   }
//   return TRUE;
// }   

// BOOLEAN testingvisuals(leftv res, leftv args)   // for testing purposes
// {                                               // testing visualization of fans
//   try{                                          // exactly same as smalltest
//     // perl::Object p("Polytope<Rational>");
//     // p = CallPolymakeFunction("cube",3);
//     perl::Object p("PolyhedralFan");
//     Matrix<Integer> zm=(unit_matrix<Integer>(3));
//     p.take("INPUT_RAYS") << zm;
//     Set<int> s;
//     s = s+0;
//     s = s+1;
//     s = s+2;
//     Array<Set<int> > ar(1);
//     ar[0]=s;
//     p.take("INPUT_CONES") << ar;
//     VoidCallPolymakeFunction("jreality",p.CallPolymakeMethod("VISUAL")); 
//     res->rtyp = NONE;
//     res->data = NULL;
//     return FALSE;
//   } 
//   catch (const std::exception& ex) 
//   {
//     WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n"); 
//     return TRUE;
//   }
// }


// BOOLEAN testingstrings(leftv res, leftv args)
// {
//   leftv u = args;
//   gfan::ZFan* zf = (gfan::ZFan*) u->Data();
//   std::string zs = zf->toString();
//   std::istringstream s(zs);
//   gfan::ZFan* zg = new gfan::ZFan(s);
//   res->rtyp = fanID;
//   res->data = (char*) zg;
//   return FALSE;
// }


// BOOLEAN callpolymakefunction(leftv res, leftv args)
// {
//   leftv u = args;
//   if (u != NULL)
//   {
//     leftv v = u->next;
//     if ((v != NULL) && (v->typ() == STRING_CMD))
//     {
//       /* checks for type of input, 
//          stores it into a polymake::perl::Object */
//       if (u->typ() == coneID)
//       {
//         gfan::ZCone* zc = (gfan::ZCone*) u->data();
//         polymake::perl::Object in = ZCone2PmCone(zc);
//       }
//       if (u->typ() == fanID)
//       {
//         gfan::ZFan* zf = (gfan::ZFan*) u->data();
//         polymake::perl::Object in = ZFan2PmFan(zf);
//       }
//       /* calls the functions in polymake */
//       string cmd = v->data();
//       polymake::perl::Object out;
//       CallPolymakeFunction(cmd, in) >> out;
//     }
//   }
// }


BOOLEAN PMconeViaRays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
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

      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        int flag = (int) (long) w->Data(); // TODO: this will indicate whether the
                                           // information provided are exact
      } 
    }
    gfan::ZCone* zc = PmCone2ZCone(&pc);
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
    res->rtyp = polytopeID;
    res->data = (char*) zp;
    return FALSE;
  }  
  WerrorS("polytopeViaVertices: unexpected parameters");
  return TRUE;
}


// BOOLEAN loadPolymakeDocumentation(leftv res, leftv args)
// {
//   init_polymake_help();

//   res->rtyp = NONE;
//   res->data = NULL;
//   return FALSE;
// }


extern "C" int mod_init(void* polymakesingular)
{
  if (init_polymake==NULL) 
    {init_polymake = new polymake::Main();}
  init_polymake->set_application("fan");
  // iiAddCproc("","cube",FALSE,cube);
  // iiAddCproc("","cross",FALSE,cross);
  iiAddCproc("","coneViaRays",FALSE,PMconeViaRays);
  iiAddCproc("","polytopeViaVertices",FALSE,PMpolytopeViaVertices);
  iiAddCproc("","isLatticePolytope",FALSE,PMisLatticePolytope);
  iiAddCproc("","isBounded",FALSE,PMisBounded);
  iiAddCproc("","isReflexive",FALSE,PMisReflexive);
  iiAddCproc("","isGorenstein",FALSE,PMisGorenstein);
  iiAddCproc("","gorensteinIndex",FALSE,PMgorensteinIndex);
  iiAddCproc("","gorensteinVector",FALSE,PMgorensteinVector);
  iiAddCproc("","isCanonical",FALSE,PMisCanonical);
  iiAddCproc("","isTerminal",FALSE,PMisTerminal);
  iiAddCproc("","isLatticeEmpty",FALSE,PMisLatticeEmpty);
  iiAddCproc("","latticeVolume",FALSE,PMlatticeVolume);
  iiAddCproc("","latticeDegree",FALSE,PMlatticeDegree);
  iiAddCproc("","latticeCodegree",FALSE,PMlatticeCodegree);
  iiAddCproc("","ehrhartPolynomialCoeff",FALSE,PMehrhartPolynomialCoeff);
  iiAddCproc("","fVector",FALSE,PMfVector);
  iiAddCproc("","hVector",FALSE,PMhVector);
  iiAddCproc("","hStarVector",FALSE,PMhStarVector);
  iiAddCproc("","isNormal",FALSE,PMisNormal);
  iiAddCproc("","facetWidths",FALSE,PMfacetWidths);
  iiAddCproc("","facetWidth",FALSE,PMfacetWidth);
  iiAddCproc("","facetVertexLatticeDistances",FALSE,PMfacetVertexLatticeDistances);
  iiAddCproc("","isCompressed",FALSE,PMisCompressed);
  iiAddCproc("","isSmooth",FALSE,PMisSmooth);
  iiAddCproc("","isVeryAmple",FALSE,PMisVeryAmple);
  iiAddCproc("","latticePoints",FALSE,PMlatticePoints);
  iiAddCproc("","nLatticePoints",FALSE,PMnLatticePoints);
  iiAddCproc("","interiorLatticePoints",FALSE,PMinteriorLatticePoints);
  iiAddCproc("","nInteriorLatticePoints",FALSE,PMnInteriorLatticePoints);
  iiAddCproc("","boundaryLatticePoints",FALSE,PMboundaryLatticePoints);
  iiAddCproc("","nBoundaryLatticePoints",FALSE,PMnBoundaryLatticePoints);
  iiAddCproc("","hilbertBasis",FALSE,PMhilbertBasis);
  iiAddCproc("","nHilbertBasis",FALSE,PMnHilbertBasis);
  iiAddCproc("","minkowskiSum",FALSE,PMminkowskiSum);
  iiAddCproc("","maximalFace",FALSE,PMmaximalFace);
  iiAddCproc("","minimalFace",FALSE,PMminimalFace);
  iiAddCproc("","maximalValue",FALSE,PMmaximalValue);
  iiAddCproc("","minimalValue",FALSE,PMminimalValue);
  iiAddCproc("","visual",FALSE,visual);
  iiAddCproc("","normalFan",FALSE,normalFan);
  // iiAddCproc("","testingtypes",FALSE,testingtypes);
  // iiAddCproc("","testingintvec",FALSE,testingintvec);
  // iiAddCproc("","testingcones",FALSE,testingcones);
  // iiAddCproc("","testingpolytopes",FALSE,testingpolytopes);
  // iiAddCproc("","testingfans",FALSE,testingfans);
  // iiAddCproc("","testingvisuals",FALSE,testingvisuals);
  // iiAddCproc("","testingstrings",FALSE,testingstrings);
  // iiAddCproc("","testingmatrices",FALSE,testingmatrices);
  // iiAddCproc("","loadPolymakeDocumentation",FALSE,loadPolymakeDocumentation);

  blackbox* b=getBlackboxStuff(polytopeID);
  b->blackbox_Op2=bbpolytope_Op2;
 
  init_polymake_help();
}
