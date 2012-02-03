#include </usr/local/include/polymake/Main.h>
#include </usr/local/include/polymake/Matrix.h>
#include </usr/local/include/polymake/Rational.h>
#include </usr/local/include/polymake/Integer.h>
#include </usr/local/include/polymake/perl/macros.h>
#include </usr/local/include/polymake/next/Set.h>
#include </usr/local/include/polymake/next/IncidenceMatrix.h>

#include </usr/local/include/gfanlib.h>
#include </usr/local/include/gfanlib_q.h>

#include <gmpxx.h>

#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>

#include <kernel/bbcone.h>
#include <kernel/bbfan.h>
#include <kernel/bbpolytope.h>

#include <Singular/ipshell.h>
#include <Singular/subexpr.h>

using namespace polymake;

/* Functions for converting Integers, Rationals and their Matrices 
   in between C++, gfan, polymake and singular */

gfan::Integer PmInteger2GfInteger (const polymake::Integer pi)
{
  mpz_class cache(pi.get_rep());
  gfan::Integer gi(cache.get_mpz_t());
  return gi;  
}

polymake::Integer GfInteger2PmInteger (const gfan::Integer gi)
{
  mpz_t cache; mpz_init(cache);
  gi.setGmp(cache);
  polymake::Integer pi(cache);
  return pi;
}

int PmInteger2Int(const polymake::Integer pi, bool &ok)
{
  gfan::Integer gi = PmInteger2GfInteger(pi);
  int i = integerToInt(gi, ok);
  return i;
}

gfan::Rational PmRational2GfRational (const polymake::Rational pr)
{
  mpq_class cache(pr.get_rep());
  gfan::Rational gr(cache.get_mpq_t());
  return gr;
}

polymake::Rational GfRational2PmRational (const gfan::Rational gr)
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

intvec PmVectorInteger2Intvec (const polymake::Vector<polymake::Integer> vi)
{
  intvec iv(vi.size());
  for(int i=1; i<=vi.size(); i++)
    {
      iv[i-1]=vi[i-1];
    }
  return iv;
}

gfan::ZMatrix PmMatrixInteger2GfZMatrix (const polymake::Matrix<polymake::Integer> mi)
{
  int rows=mi.rows();
  int cols=mi.cols();
  gfan::ZMatrix zm(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      zm[r-1][c-1] = PmInteger2GfInteger(mi(r-1,c-1));
  return zm;
}

gfan::QMatrix PmMatrixRational2GfQMatrix (const polymake::Matrix<polymake::Rational> mr)
{
  int rows=mr.rows();
  int cols=mr.cols();
  gfan::QMatrix qm(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      qm[r-1][c-1] = PmRational2GfRational(mr(r-1,c-1));
  return qm;
}

polymake::Matrix<polymake::Integer> GfZMatrix2PmMatrixInteger (const gfan::ZMatrix zm)
{
  int rows=zm.getHeight();
  int cols=zm.getWidth();
  polymake::Matrix<polymake::Integer> mi(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      mi(r-1,c-1) = GfInteger2PmInteger(zm[r-1][c-1]);
  return mi;
}

polymake::Matrix<polymake::Rational> GfQMatrix2PmMatrixRational (const gfan::QMatrix qm)
{
  int rows=qm.getHeight();
  int cols=qm.getWidth();
  polymake::Matrix<polymake::Rational> mr(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      mr(r-1,c-1) = GfRational2PmRational(qm[r-1][c-1]);
  return mr;
}

intvec* PmMatrixInteger2Intvec (polymake::Matrix<polymake::Integer> mi)
{
  int rows = mi.rows();
  int cols = mi.cols();
  intvec* iv = new intvec(rows,cols,0);
  const polymake::Integer* pi = concat_rows(mi).begin();
  for (int r = 1; r <= rows; r++) 
    for (int c = 1; c <= cols; c++) 
       IMATELEM(*iv,r,c) = *pi++;
  return iv;  
}

intvec* PmIncidenceMatrix2Intvec (polymake::IncidenceMatrix<NonSymmetric> icmat)
{
  int rows = icmat.rows();
  int cols = icmat.cols();
  intvec* iv = new intvec(rows,cols,0);
  for (int r = 1; r <= rows; r++)
    for (int c = 1; c <= cols; c++)
      IMATELEM(*iv,r,c) = (int) icmat.row(r).exists(c);
  return iv;
}

intvec PmSetInteger2Intvec (polymake::Set<polymake::Integer> si)
{
  polymake::Vector<polymake::Integer> vi(si);
  return PmVectorInteger2Intvec(vi);
}

/* Functions for converting cones and fans in between gfan and polymake,
   Singular shares the same cones and fans with gfan */

gfan::ZCone PmCone2ZCone (polymake::perl::Object pc)
{
  if (pc.isa("Cone"))
  {
    polymake::Matrix<polymake::Integer> rays = pc.give("RAYS");
    gfan::ZMatrix zm = PmMatrixInteger2GfZMatrix(rays);

    gfan::ZCone zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
    return zc;
  }
  WerrorS("PmCone2ZCone: unexpected parameters");
}

gfan::ZCone PmPolytope2ZPolytope (polymake::perl::Object pp)
{
  if (pp.isa("Polytope<Rational>"))
  {
    polymake::Matrix<polymake::Integer> vertices = pp.give("VERTICES");
    gfan::ZMatrix zv = PmMatrixInteger2GfZMatrix(vertices);

    gfan::ZCone zp = gfan::ZCone::givenByRays(zv, gfan::ZMatrix(0, zv.getWidth()));
    return zp;
  }
  WerrorS("PmPolytope2ZPolytope: unexpected parameters");
}

gfan::ZFan PmFan2ZFan (polymake::perl::Object pf)
{
  if (pf.isa("PolyhedralFan"))
  {
    int d = (int) pf.give("FAN_AMBIENT_DIM");
    gfan::ZFan zf = gfan::ZFan(d);

    int n = pf.give("N_MAXIMAL_CONES");
    for (int i=0; i<n; i++)
      {
        polymake::perl::Object pmcone=pf.CallPolymakeMethod("cone",i);
	gfan::ZCone zc=PmCone2ZCone(pmcone);
        zf.insert(zc);
      }
    return zf;
  }
  WerrorS("PmFan2ZFan: unexpected parameters");
}

polymake::perl::Object ZCone2PmCone (gfan::ZCone zc)
{
  gfan::ZMatrix zm = zc.extremeRays();
  polymake::Matrix<polymake::Integer> pm = GfZMatrix2PmMatrixInteger(zm);

  polymake::perl::Object gc("Cone");
  gc.take("INPUT_RAYS") << pm;  

  return gc;
}

polymake::perl::Object ZPolytope2PmPolytope (gfan::ZCone zc)
{
  gfan::ZMatrix zm = zc.extremeRays();
  polymake::Matrix<polymake::Integer> pm = GfZMatrix2PmMatrixInteger(zm);

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

  return GfZMatrix2PmMatrixInteger(zm);
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
    polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
    bool b = p.give("BOUNDED");
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
    polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
    bool b = p.give("REFLEXIVE");
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  WerrorS("isReflexive: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisNormal(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
    bool b = p.give("NORMAL");
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  WerrorS("isNormal: unexpected parameters");
  return TRUE;
}


BOOLEAN PMisSmooth(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*)u->Data();
    polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
    bool b = p.give("SMOOTH");
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  WerrorS("isSmooth: unexpected parameters");
  return TRUE;
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
      polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
      intvec* iv = (intvec*) v->Data();
      polymake::perl::Object o("LinearProgram<Rational>");
      o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
      p.take("LP") << o;
      polymake::Set<polymake::Integer> mf = p.give("LP.MAXIMAL_FACE");
      intvec* maxface = new intvec(PmSetInteger2Intvec(mf));
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
	  polymake::perl::Object p = ZPolytope2PmPolytope(*zp);
	  intvec* iv = (intvec*) v->Data();
	  polymake::perl::Object o("LinearProgram<Rational>");
	  o.take("LINEAR_OBJECTIVE") << Intvec2PmVectorInteger(iv);
	  p.take("LP") << o;
	  polymake::Set<polymake::Integer> mf = p.give("LP.MINIMAL_FACE");
	  intvec* maxface = new intvec(PmSetInteger2Intvec(mf));
	  res->rtyp = INTVEC_CMD;
	  res->data = (char*) maxface;
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
      polymake::perl::Object p = ZPolytope2PmPolytope(*zp); 
      polymake::Matrix<polymake::Rational> ver = p.give("VERTICES");
      intvec* iv = (intvec*) v->Data();
      polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
      polymake::perl::Object o("LinearProgram<Rational>");
      o.take("LINEAR_OBJECTIVE") << lo;
      p.take("LP") << o;
      polymake::Integer mv = p.give("LP.MAXIMAL_VALUE"); 
      bool ok = true;
      int m = PmInteger2Int(mv,ok); 
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
      polymake::perl::Object p = ZPolytope2PmPolytope(*zp); 
      polymake::Matrix<polymake::Rational> ver = p.give("VERTICES");
      intvec* iv = (intvec*) v->Data();
      polymake::Vector<polymake::Integer> lo = Intvec2PmVectorInteger(iv);
      polymake::perl::Object o("LinearProgram<Rational>");
      o.take("LINEAR_OBJECTIVE") << lo;
      p.take("LP") << o;
      polymake::Integer mv = p.give("LP.MINIMAL_VALUE"); 
      bool ok = true;
      int m = PmInteger2Int(mv,ok); 
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
  WerrorS("minimalValue: unexpected parameters");
  return TRUE;
}


BOOLEAN visual(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {  
    try
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZMatrix rays = zc->extremeRays();
      polymake::perl::Object p("Polytope<Rational>");
      p.take("VERTICES") << GfZMatrix2PmMatrixInteger(rays);
      CallPolymakeFunction("jreality",p.CallPolymakeMethod("VISUAL"));
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }
    catch (const std::exception& ex)
    {
      std::cerr << "Error: " << ex.what() << std::endl; 
      return TRUE;
    }
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    try
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      polymake::perl::Object pf=ZFan2PmFan(zf);
      CallPolymakeFunction("jreality",pf.CallPolymakeMethod("VISUAL"));
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }    
    catch (const std::exception& ex)
    {
      std::cerr << "Error: " << ex.what() << std::endl; 
      return TRUE;
    }
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
    polymake::perl::Object p("Polytope<Rational>");
    p.take("VERTICES") << GfZMatrix2PmMatrixInteger(rays);
    polymake::perl::Object pf;
    CallPolymakeFunction("normal_fan", p) >> pf;
    gfan::ZFan* zf = new gfan::ZFan(PmFan2ZFan(pf));
    res->rtyp = fanID;
    res->data = (char*) zf;
    return FALSE;
  }
  WerrorS("normal_fan: unexpected parameters");
  return TRUE; 
}


BOOLEAN testingtypes(leftv res, leftv args)
{
  leftv u = args;
  if (u != NULL)
  {
    leftv v = u->next;
    if (v != NULL)
    {
      leftv w = v->next;
      if (w != NULL)
      {
	Print("\n (u->Typ() ->) %d =?= %d (<-coneID) \n", u->Typ(),coneID);
	Print("\n (u->Typ() ->) %d =?= %d (<-fanID) \n", v->Typ(),fanID);
	Print("\n (u->Typ() ->) %d =?= %d (<-polytopeID) \n", w->Typ(),polytopeID);
	res->rtyp = NONE;
	res->data = NULL;
	return FALSE;
      }
    }
  }
  return TRUE;
}


BOOLEAN testingintvec(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ()==INTVEC_CMD))
  {
    intvec* iv = (intvec*)u->Data();
    polymake::Vector<polymake::Integer> pmvec = Intvec2PmVectorInteger(iv);
    res->rtyp = NONE;
    res->data = NULL;
    return FALSE;
  }
  return TRUE;
}


BOOLEAN testingfans(leftv res, leftv args)  // for testing purposes    
{                                           // creating a fan in polymake 
  leftv u = args;                           // and handing it to Singular
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    perl::Object pf = ZFan2PmFan(zf);
    gfan::ZFan* zff = new gfan::ZFan(PmFan2ZFan(pf));
    res->rtyp = fanID;
    res->data = (char *)zff;
    return FALSE;
  }
  return TRUE;
}

BOOLEAN testingcones(leftv res, leftv args)  // for testing purposes       
{                                            // taking a cone from Singular,
                                             // handing it over to polymake 
                                             // and back                    
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
    {
      gfan::ZCone* zc = (gfan::ZCone*) u->Data();
      polymake::perl::Object pc = ZCone2PmCone(*zc);
      gfan::ZCone* zd = new gfan::ZCone(PmCone2ZCone(pc));
      res->rtyp = coneID;
      res->data = (char *) zd;
      return FALSE;
    }
  return TRUE;
}

BOOLEAN testingpolytopes(leftv res, leftv args) // for testing purposes
{                                                // taking a cone from Singular,
                                                 // handing it over to polymake
                                                 // and back
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zp = (gfan::ZCone*) u->Data();
    polymake::perl::Object pp = ZPolytope2PmPolytope(*zp);
    gfan::ZCone* zq = new gfan::ZCone(PmPolytope2ZPolytope(pp));
    res->rtyp = polytopeID;
    res->data = (char *) zq;
    return FALSE;
  }
  return TRUE;
}   

BOOLEAN testingvisuals(leftv res, leftv args)   // for testing purposes
{                                               // testing visualization of fans
  try{                                          // exactly same as smalltest
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
    CallPolymakeFunction("jreality",p.CallPolymakeMethod("VISUAL")); 
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


extern "C" int mod_init(void* polymakesingular)
{

  polymake::Main init_polymake;
  init_polymake.set_application("fan");
  // iiAddCproc("","cube",FALSE,cube);
  // iiAddCproc("","cross",FALSE,cross);
  iiAddCproc("","isBounded",FALSE,PMisBounded);
  iiAddCproc("","isNormal",FALSE,PMisNormal);
  iiAddCproc("","isReflexive",FALSE,PMisReflexive);
  iiAddCproc("","isSmooth",FALSE,PMisSmooth);
  iiAddCproc("","maximalFace",FALSE,PMmaximalFace);
  iiAddCproc("","minimalFace",FALSE,PMminimalFace);
  iiAddCproc("","maximalValue",FALSE,PMmaximalValue);
  iiAddCproc("","minimalValue",FALSE,PMminimalValue);
  iiAddCproc("","visual",FALSE,visual);
  iiAddCproc("","normalFan",FALSE,normalFan);
  // iiAddCproc("","testingtypes",FALSE,testingtypes);
  // iiAddCproc("","testingintvec",FALSE,testingintvec);
  // iiAddCproc("","testingcones",FALSE,testingcones);
  iiAddCproc("","testingpolytopes",FALSE,testingpolytopes);
  // iiAddCproc("","testingfans",FALSE,testingfans);
  // iiAddCproc("","testingvisuals",FALSE,testingvisuals);

}
