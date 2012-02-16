#include <polymake/Main.h>
#include <polymake/Matrix.h>
#include <polymake/Rational.h>
#include <polymake/Integer.h>
#include <polymake/perl/macros.h>
#include <polymake/Set.h>
#include <polymake/IncidenceMatrix.h>

#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_q.h>

#include <gmpxx.h>

#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

#include <Singular/blackbox.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>

using namespace polymake;


polymake::Main* init_polymake=NULL;

/* Functions for converting Integers, Rationals and their Matrices 
   in between C++, gfan, polymake and singular */

gfan::Integer PmInteger2GfInteger (const polymake::Integer& pi)
{
  mpz_class cache(pi.get_rep());
  gfan::Integer gi(cache.get_mpz_t());
  return gi;  
}

polymake::Integer GfInteger2PmInteger (const gfan::Integer& gi)
{
  mpz_t cache; mpz_init(cache);
  gi.setGmp(cache);
  polymake::Integer pi(cache);
  return pi;
}

int PmInteger2Int(const polymake::Integer& pi, bool &ok)
{
  gfan::Integer gi = PmInteger2GfInteger(pi);
  int i = integerToInt(gi, ok);
  return i;
}

gfan::Rational PmRational2GfRational (const polymake::Rational& pr)
{
  mpq_class cache(pr.get_rep());
  gfan::Rational gr(cache.get_mpq_t());
  return gr;
}

polymake::Rational GfRational2PmRational (const gfan::Rational& gr)
{
  mpq_t cache; mpq_init(cache);
  gr.setGmp(cache);
  polymake::Rational pr(cache);
  return pr;
}

polymake::Vector<polymake::Integer> Intvec2PmVectorInteger (const intvec* iv)
{
  polymake::Vector<polymake::Integer> vi(iv->length()); 
  for(int i=1; i<=iv->length(); i++)
  {
    vi[i-1]=(*iv)[i-1];
  }
  return vi;
}

intvec* PmVectorInteger2Intvec (const polymake::Vector<polymake::Integer>* vi)
{
  intvec* iv = new intvec(vi->size());
  for(int i=1; i<=vi->size(); i++)
    {
      (*iv)[i-1]=(*vi)[i-1];
    }
  return iv;
}

gfan::ZMatrix PmMatrixInteger2GfZMatrix (const polymake::Matrix<polymake::Integer>* mi)
{
  int rows=mi->rows();
  int cols=mi->cols();
  gfan::ZMatrix zm(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      zm[r-1][c-1] = PmInteger2GfInteger((*mi)(r-1,c-1));
  return zm;
}

gfan::QMatrix PmMatrixRational2GfQMatrix (const polymake::Matrix<polymake::Rational>* mr)
{
  int rows=mr->rows();
  int cols=mr->cols();
  gfan::QMatrix qm(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      qm[r-1][c-1] = PmRational2GfRational((*mr)(r-1,c-1));
  return qm;
}

polymake::Matrix<polymake::Integer> GfZMatrix2PmMatrixInteger (const gfan::ZMatrix* zm)
{
  int rows=zm->getHeight();
  int cols=zm->getWidth();
  polymake::Matrix<polymake::Integer> mi(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      mi(r-1,c-1) = GfInteger2PmInteger((*zm)[r-1][c-1]);
  return mi;
}

polymake::Matrix<polymake::Rational> GfQMatrix2PmMatrixRational (const gfan::QMatrix* qm)
{
  int rows=qm->getHeight();
  int cols=qm->getWidth();
  polymake::Matrix<polymake::Rational> mr(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      mr(r-1,c-1) = GfRational2PmRational((*qm)[r-1][c-1]);
  return mr;
}

intvec* PmMatrixInteger2Intvec (polymake::Matrix<polymake::Integer>* mi)
{
  int rows = mi->rows();
  int cols = mi->cols();
  intvec* iv = new intvec(rows,cols,0);
  const polymake::Integer* pi = concat_rows(*mi).begin();
  for (int r = 1; r <= rows; r++) 
    for (int c = 1; c <= cols; c++) 
       IMATELEM(*iv,r,c) = *pi++;
  return iv;  
}

intvec* PmIncidenceMatrix2Intvec (polymake::IncidenceMatrix<NonSymmetric>* icmat)
{
  int rows = icmat->rows();
  int cols = icmat->cols();
  intvec* iv = new intvec(rows,cols,0);
  for (int r = 1; r <= rows; r++)
    for (int c = 1; c <= cols; c++)
      IMATELEM(*iv,r,c) = (int) (*icmat).row(r).exists(c);
  return iv;
}

intvec* PmSetInteger2Intvec (polymake::Set<polymake::Integer>* si)
{
  polymake::Vector<polymake::Integer> vi(*si);
  return PmVectorInteger2Intvec(&vi);
}

/* Functions for converting cones and fans in between gfan and polymake,
   Singular shares the same cones and fans with gfan */

gfan::ZCone PmCone2ZCone (polymake::perl::Object* pc)
{
  if (pc->isa("Cone"))
  {
    polymake::Matrix<polymake::Integer> rays = pc->give("RAYS");
    gfan::ZMatrix zm = PmMatrixInteger2GfZMatrix(&rays);

    gfan::ZCone zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
    return zc;
  }
  WerrorS("PmCone2ZCone: unexpected parameters");
}

gfan::ZCone PmPolytope2ZPolytope (polymake::perl::Object* pp)
{
  if (pp->isa("Polytope<Rational>"))
  {
    polymake::Matrix<polymake::Integer> vertices = pp->give("VERTICES");
    gfan::ZMatrix zv = PmMatrixInteger2GfZMatrix(&vertices);

    gfan::ZCone zp = gfan::ZCone::givenByRays(zv, gfan::ZMatrix(0, zv.getWidth()));
    return zp;
  }
  WerrorS("PmPolytope2ZPolytope: unexpected parameters");
}

gfan::ZFan PmFan2ZFan (polymake::perl::Object* pf)
{
  if (pf->isa("PolyhedralFan"))
  {
    int d = (int) pf->give("FAN_AMBIENT_DIM");
    gfan::ZFan zf = gfan::ZFan(d);

    int n = pf->give("N_MAXIMAL_CONES");
    for (int i=0; i<n; i++)
      {
        polymake::perl::Object pmcone=pf->CallPolymakeMethod("cone",i);
        gfan::ZCone zc=PmCone2ZCone(&pmcone);
        zf.insert(zc);
      }
    return zf;
  }
  WerrorS("PmFan2ZFan: unexpected parameters");
}

polymake::perl::Object ZCone2PmCone (gfan::ZCone* zc)
{
  gfan::ZMatrix zm = zc->extremeRays();
  polymake::Matrix<polymake::Integer> pm = GfZMatrix2PmMatrixInteger(&zm);

  polymake::perl::Object gc("Cone");
  gc.take("INPUT_RAYS") << pm;  

  return gc;
}

polymake::perl::Object ZPolytope2PmPolytope (gfan::ZCone* zc)
{
  gfan::ZMatrix zm = zc->extremeRays();
  polymake::Matrix<polymake::Integer> pm = GfZMatrix2PmMatrixInteger(&zm);

  polymake::perl::Object pp("Polytope<Rational>");
  pp.take("VERTICES") << pm;

  return pp;
}

polymake::Matrix<polymake::Integer> raysOf(gfan::ZFan* zf)
{
  int d = zf->getAmbientDimension();
  int n = zf->numberOfConesOfDimension(1,0,0);
  gfan::ZMatrix zm(n,d);

  for (int i=0; i<n; i++)
    {
      gfan::ZCone zc = zf->getCone(1,i,0,0);
      gfan::ZMatrix ray = zc.extremeRays();
      for (int j=0; j<d; j++)
        {
          zm[i][j]=ray[0][j];
        }
    }

  return GfZMatrix2PmMatrixInteger(&zm);
}

int numberOfRaysOf(gfan::ZFan* zf)
{
  int n = zf->numberOfConesOfDimension(1,0,0);
  return n;
}

int numberOfMaximalConesOf(gfan::ZFan* zf)
{
  int d = zf->getAmbientDimension();
  int n = 0;

  for (int i=0; i<=n; i++)
    {
      int n = n + zf->numberOfConesOfDimension(i,0,1);
    }

  return n;
}

polymake::Array<polymake::Set<int> > conesOf(gfan::ZFan* zf)
{
  int c = numberOfRaysOf(zf);
  int r = numberOfMaximalConesOf(zf);

  polymake::Matrix<polymake::Integer> pm=raysOf(zf);
  polymake::Array<polymake::Set<int> > L(r);

  int d = 1;  // runs through all dimensions
  int ii = 0; // runs through all cones of a given dimension
  for (int i=0; i<r; i++)
    {
      if (ii>zf->numberOfConesOfDimension(d,0,1))  // if all cones of a dimension are through,
        {
          d = d+1;                                 // increase dimension
          ii = 0;                                  // set counter to 0
        }
      gfan::IntVector v = zf->getConeIndices(d,ii,0,1);
      polymake::Set<int> s;
      for (int j=0; j<v.size(); j++)
        {
          s = s+v[j];
        }
      
      L[i] = s;
      ii = ii+1;
    }
  return L;
}

polymake::perl::Object ZFan2PmFan (gfan::ZFan* zf)
{
  polymake::perl::Object pf("PolyhedralFan");

  polymake::Matrix<polymake::Integer> zm = raysOf(zf);
  pf.take("INPUT_RAYS") << zm;

  polymake::Array<polymake::Set<int> > ar = conesOf(zf);
  pf.take("INPUT_CONES") << ar;

  return pf;
}


/*******************************************************/


static BOOLEAN bbpolytope_Op2(int op, leftv res, leftv p, leftv q)
{
  gfan::ZCone* zp = (gfan::ZCone*) p->Data();
  switch(op)
  {
    case '+':
    {
      if (q->Typ()==polytopeID)
      {
        gfan::ZCone* zq = (gfan::ZCone*) q->Data();
        gfan::ZCone* ms;
        try
        {
          polymake::perl::Object pp = ZPolytope2PmPolytope(zp);
          polymake::perl::Object pq = ZPolytope2PmPolytope(zq);
          polymake::perl::Object pms;
          CallPolymakeFunction("minkowski_sum", pp, pq) >> pms;
          ms = new gfan::ZCone(PmPolytope2ZPolytope(&pms));
        }
        catch (const std::exception& ex) 
        {
          std::cerr << "ERROR: " << ex.what() << endl; 
          return TRUE;
        }
        res->rtyp = polytopeID;
        res->data = (void*) ms;
        return FALSE;
      }
      return TRUE;
    }
    default:
      return blackboxDefaultOp2(op,res,p,q);
  }
  return blackboxDefaultOp2(op,res,p,q);
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


BOOLEAN PMisBounded(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("BOUNDED");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("REFLEXIVE");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("GORENSTEIN");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer pgi = p.give("GORENSTEIN_INDEX");
      bool ok = true;
      gi = PmInteger2Int(pgi,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
      res->rtyp = INT_CMD;
      res->data = (char*) (int) gi;
      return FALSE;
    }
    catch (const std::exception& ex)
    {
      std::cerr << "ERROR: " << ex.what() << endl;
      return TRUE;
    }
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> pgv = p.give("GORENSTEIN_VECTOR");
      gv = PmVectorInteger2Intvec(&pgv);
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("CANONICAL");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("TERMINAL");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("LATTICE_EMPTY");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer plv = p.give("LATTICE_VOLUME");
      bool ok = true;
      lv = PmInteger2Int(plv,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
      res->rtyp = INT_CMD;
      res->data = (char*) (int) lv;
      return FALSE;
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
      return TRUE;
    }
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer pld = p.give("LATTICE_DEGREE");
      bool ok = true;
      ld = PmInteger2Int(pld,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
      res->rtyp = INT_CMD;
      res->data = (char*) (int) ld;
      return FALSE;
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
      return TRUE;
    }
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer plc = p.give("LATTICE_CODEGREE");
      bool ok = true;
      lc = PmInteger2Int(plc,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> pec = p.give("EHRHART_POLYNOMIAL_COEFF");
      ec = PmVectorInteger2Intvec(&pec);
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
      return TRUE;
    }
    res->rtyp = INTVEC_CMD;
    res->data = (char*) ec;
    return FALSE;
  }
  WerrorS("ehrhartPolynomialCoeff: unexpected parameters");
  return TRUE;
}


BOOLEAN PMhStarVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* hv;
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> phv = p.give("H_STAR_VECTOR");
      hv = PmVectorInteger2Intvec(&phv);
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("NORMAL");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Vector<polymake::Integer> pfw = p.give("FACET_WIDTHS");
      fw = PmVectorInteger2Intvec(&pfw);
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer pfw = p.give("FACET_WIDTH");
      bool ok = true;
      fw = PmInteger2Int(pfw,ok); 
      if (!ok)
      { 
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> pld = p.give("FACET_VERTEX_LATTICE_DISTANCES");
      ld = PmMatrixInteger2Intvec(&pld);
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("COMPRESSED");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    bool b;
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("SMOOTH");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      b = p.give("VERY_AMPLE");
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p.give("LATTICE_POINTS");
      iv = PmMatrixInteger2Intvec(&lp); 
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {    
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p.give("N_LATTICE_POINTS");
      bool ok = true;
      n = PmInteger2Int(nlp,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p.give("INTERIOR_LATTICE_POINTS");
      iv = PmMatrixInteger2Intvec(&lp); 
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p.give("N_INTERIOR_LATTICE_POINTS");
      bool ok = true;
      n = PmInteger2Int(nlp,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p.give("BOUNDARY_LATTICE_POINTS");
      iv = PmMatrixInteger2Intvec(&lp); 
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p.give("N_BOUNDARY_LATTICE_POINTS");
      bool ok = true;
      n = PmInteger2Int(nlp,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    intvec* iv;
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Matrix<polymake::Integer> lp = p.give("HILBERT_BASIS");
      iv = PmMatrixInteger2Intvec(&lp); 
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    int n;
    try
    {
      polymake::perl::Object p = ZPolytope2PmPolytope(zp);
      polymake::Integer nlp = p.give("N_HILBERT_BASIS");
      bool ok = true;
      n = PmInteger2Int(nlp,ok); 
      if (!ok)
      {
        WerrorS("overflow while converting polymake::Integer to int");
        return TRUE;
      }
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zp = (gfan::ZCone*)u->Data();
      gfan::ZCone* zq = (gfan::ZCone*)v->Data();
      gfan::ZCone* ms;
      try
      {
        polymake::perl::Object pp = ZPolytope2PmPolytope(zp);
        polymake::perl::Object pq = ZPolytope2PmPolytope(zq);
        polymake::perl::Object pms;
        CallPolymakeFunction("minkowski_sum", pp, pq) >> pms;
        ms = new gfan::ZCone(PmPolytope2ZPolytope(&pms));
      }
      catch (const std::exception& ex) 
      {
        std::cerr << "ERROR: " << ex.what() << endl; 
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


polymake::Matrix<polymake::Integer> raysOf(const polymake::perl::Object* zf, 
                                           const polymake::Set<polymake::Integer>* s)
{
  polymake::Matrix<polymake::Integer> allrays = zf->give("RAYS");
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
      try
      {
        polymake::perl::Object p = ZPolytope2PmPolytope(zp);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
        p.take("LP") << o;
        polymake::Set<polymake::Integer> mf = p.give("LP.MAXIMAL_FACE");
        polymake::Matrix<polymake::Integer> rays = raysOf(&p,&mf);
        maxface = new intvec(PmMatrixInteger2Intvec(&rays));
      }
      catch (const std::exception& ex) 
      {
        std::cerr << "ERROR: " << ex.what() << endl; 
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
      try
      {
        polymake::perl::Object p = ZPolytope2PmPolytope(zp);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
        p.take("LP") << o;
        polymake::Set<polymake::Integer> mf = p.give("LP.MINIMAL_FACE");
        polymake::Matrix<polymake::Integer> rays = raysOf(&p,&mf);
        minface = new intvec(PmMatrixInteger2Intvec(&rays));
      }
      catch (const std::exception& ex) 
      {
        std::cerr << "ERROR: " << ex.what() << endl; 
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
      int m;
      try
      {
        polymake::perl::Object p = ZPolytope2PmPolytope(zp); 
        polymake::Matrix<polymake::Rational> ver = p.give("VERTICES");
        polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << lo;
        p.take("LP") << o;
        polymake::Integer mv = p.give("LP.MAXIMAL_VALUE"); 
        bool ok = true;
        m = PmInteger2Int(mv,ok); 
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
      }
      catch (const std::exception& ex) 
      {
        std::cerr << "ERROR: " << ex.what() << endl; 
        return TRUE;
      }
      res->rtyp = INT_CMD;
      res->data = (char*) m;
      return FALSE;
    }
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
      int m;
      try
      {
        polymake::perl::Object p = ZPolytope2PmPolytope(zp); 
        polymake::Matrix<polymake::Rational> ver = p.give("VERTICES");
        polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
        polymake::perl::Object o("LinearProgram<Rational>");
        o.take("LINEAR_OBJECTIVE") << lo;
        p.take("LP") << o;
        polymake::Integer mv = p.give("LP.MINIMAL_VALUE"); 
        bool ok = true;
        m = PmInteger2Int(mv,ok); 
        if (!ok)
        {
          WerrorS("overflow while converting polymake::Integer to int");
          return TRUE;
        }
      }
      catch (const std::exception& ex) 
      {
        std::cerr << "ERROR: " << ex.what() << endl; 
        return TRUE;
      }
      res->rtyp = INT_CMD;
      res->data = (char*) m;
      return FALSE;
    }
  }
  WerrorS("minimalValue: unexpected parameters");
  return TRUE;
}


BOOLEAN visual(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {  
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix rays = zc->extremeRays();
    try
    {
      polymake::perl::Object p("Polytope<Rational>");
      p.take("VERTICES") << GfZMatrix2PmMatrixInteger(&rays);
      VoidCallPolymakeFunction("jreality",p.CallPolymakeMethod("VISUAL"));
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: " << ex.what() << std::endl; 
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
      polymake::perl::Object pf=ZFan2PmFan(zf);
      VoidCallPolymakeFunction("jreality",pf.CallPolymakeMethod("VISUAL"));
    }    
    catch (const std::exception& ex)
    {
      std::cerr << "Error: " << ex.what() << std::endl; 
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
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix rays = zc->extremeRays();
    gfan::ZFan* zf;
    try
    {
      polymake::perl::Object p("Polytope<Rational>");
      p.take("VERTICES") << GfZMatrix2PmMatrixInteger(&rays);
      polymake::perl::Object pf;
      CallPolymakeFunction("normal_fan", p) >> pf;
      gfan::ZFan* zf = new gfan::ZFan(PmFan2ZFan(&pf));
    }
    catch (const std::exception& ex) 
    {
      std::cerr << "ERROR: " << ex.what() << endl; 
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

// BOOLEAN testingcones(leftv res, leftv args)  // for testing purposes       
// {                                            // taking a cone from Singular,
//                                              // handing it over to polymake 
//                                              // and back                    
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == coneID))
//     {
//       gfan::ZCone* zc = (gfan::ZCone*) u->Data();
//       polymake::perl::Object pc = ZCone2PmCone(zc);
//       gfan::ZCone* zd = new gfan::ZCone(PmCone2ZCone(&pc));
//       res->rtyp = coneID;
//       res->data = (char *) zd;
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

BOOLEAN testingvisuals(leftv res, leftv args)   // for testing purposes
{                                               // testing visualization of fans
  try{                                          // exactly same as smalltest
    // perl::Object p("Polytope<Rational>");
    // p = CallPolymakeFunction("cube",3);
    perl::Object p("PolyhedralFan");
    Matrix<Integer> zm=(unit_matrix<Integer>(3));
    p.take("INPUT_RAYS") << zm;
    Set<int> s;
    s = s+0;
    s = s+1;
    s = s+2;
    Array<Set<int> > ar(1);
    ar[0]=s;
    p.take("INPUT_CONES") << ar;
    VoidCallPolymakeFunction("jreality",p.CallPolymakeMethod("VISUAL")); 
    res->rtyp = NONE;
    res->data = NULL;
    return FALSE;
  } 
  catch (const std::exception& ex) 
  {
    std::cerr << "ERROR: " << ex.what() << endl; 
    return TRUE;
  }
}


BOOLEAN testingstrings(leftv res, leftv args)
{
  leftv u = args;
  gfan::ZFan* zf = (gfan::ZFan*) u->Data();
  std::string zs = zf->toString();
  std::istringstream s(zs);
  gfan::ZFan* zg = new gfan::ZFan(s);
  res->rtyp = fanID;
  res->data = (char*) zg;
  return FALSE;
}


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


extern "C" int mod_init(void* polymakesingular)
{

  if (init_polymake==NULL) 
    {init_polymake = new polymake::Main();}
  init_polymake->set_application("fan");
  // iiAddCproc("","cube",FALSE,cube);
  // iiAddCproc("","cross",FALSE,cross);
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
  iiAddCproc("","testingvisuals",FALSE,testingvisuals);
  iiAddCproc("","testingstrings",FALSE,testingstrings);

  blackbox* b=getBlackboxStuff(polytopeID);
  b->blackbox_Op2=bbpolytope_Op2;
 
}
