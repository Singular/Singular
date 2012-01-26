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

#include <Singular/ipshell.h>
#include <Singular/subexpr.h>

using namespace polymake;

/* Functions for converting Integers, Rationals and their Matrices 
   in between C++, gfan, polymake and singular */

gfan::Integer PMInt2GFInt (const polymake::Integer pmint)
{
  mpz_class cache(pmint.get_rep());
  gfan::Integer gfint(cache.get_mpz_t());
  return gfint;  
}

polymake::Integer GFInt2PMInt (const gfan::Integer gfint)
{
  mpz_t cache; mpz_init(cache);
  gfint.setGmp(cache);
  polymake::Integer pmint(cache);
  return pmint;
}

gfan::Rational PMRat2GFRat (const polymake::Rational pmrat)
{
  mpq_class cache(pmrat.get_rep());
  gfan::Rational gfrat(cache.get_mpq_t());
  return gfrat;
}

polymake::Rational GFRat2PMRat (const gfan::Rational gfrat)
{
  mpq_t cache; mpq_init(cache);
  gfrat.setGmp(cache);
  polymake::Rational pmrat(cache);
  return pmrat;
}

gfan::ZMatrix PMMat2ZMat (const polymake::Matrix<polymake::Integer> pmmat)
{
  int rows=pmmat.rows();
  int cols=pmmat.cols();
  gfan::ZMatrix zmat(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      zmat[r-1][c-1] = PMInt2GFInt(pmmat(r-1,c-1));
  return zmat;
}

gfan::QMatrix PMMat2QMat (const polymake::Matrix<polymake::Integer> pmmat)
{
  int rows=pmmat.rows();
  int cols=pmmat.cols();
  gfan::QMatrix qmat(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      qmat[r-1][c-1] = PMRat2GFRat(pmmat(r-1,c-1));
  return qmat;
}

polymake::Matrix<polymake::Integer> ZMat2PMMat (const gfan::ZMatrix zmat)
{
  int rows=zmat.getHeight();
  int cols=zmat.getWidth();
  polymake::Matrix<polymake::Integer> pmmat(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      pmmat(r-1,c-1) = GFInt2PMInt(zmat[r-1][c-1]);
  return pmmat;
}

polymake::Matrix<polymake::Rational> QMat2PMMat (const gfan::QMatrix qmat)
{
  int rows=qmat.getHeight();
  int cols=qmat.getWidth();
  polymake::Matrix<polymake::Rational> pmmat(rows,cols);
  for(int r=1; r<=rows; r++)
    for(int c=1; c<=cols; c++)
      pmmat(r-1,c-1) = GFRat2PMRat(qmat[r-1][c-1]);
  return pmmat;
}

intvec* PMMat2intvec (polymake::Matrix<polymake::Integer> pmmat)
{
  int rows = pmmat.rows();
  int cols = pmmat.cols();
  intvec* simat = new intvec(rows,cols,0);
  const polymake::Integer* elem = concat_rows(pmmat).begin();
  for (int r = 1; r <= rows; r++) 
    for (int c = 1; c <= cols; c++) 
       IMATELEM(*simat,r,c) = *elem++;
  return simat;  
}

/* Functions for converting cones and fans in between gfan and polymake,
   Singular shares the same cones and fans with gfan */

gfan::ZCone PMCone2ZCone (polymake::perl::Object pc)
{
  if (pc.isa("Cone"))
  {
    polymake::Matrix<polymake::Integer> rays = pc.give("RAYS");
    gfan::ZMatrix zm = PMMat2ZMat(rays);

    gfan::ZCone zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
    return zc;
  }
  WerrorS("PMCone2ZCone: unexpected parameters");
}

gfan::ZFan PMFan2ZFan (polymake::perl::Object pf)
{
  if (pf.isa("PolyhedralFan"))
  {
    int d = (int) pf.give("FAN_AMBIENT_DIM");
    gfan::ZFan zf = gfan::ZFan(d);

    int n = pf.give("N_MAXIMAL_CONES");
    for (int i=0; i<n; i++)
      {
        polymake::perl::Object pmcone=pf.CallPolymakeMethod("cone",i);
	gfan::ZCone zc=PMCone2ZCone(pmcone);
        zf.insert(zc);
      }
    return zf;
  }
  WerrorS("PMFan2ZFan: unexpected parameters");
}

polymake::perl::Object ZCone2PMCone (gfan::ZCone zc)
{
  gfan::ZMatrix zm = zc.extremeRays();
  polymake::Matrix<polymake::Integer> pm = ZMat2PMMat(zm);

  polymake::perl::Object gc("Cone");
  gc.take("INPUT_RAYS") << pm;  

  return gc;
}

polymake::Matrix<polymake::Integer> PRaysOf(gfan::ZFan* zf)
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

  return ZMat2PMMat(zm);
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

polymake::Array<polymake::Set<int> > PConesOf(gfan::ZFan* zf)
{
  int c = numberOfRaysOf(zf);
  int r = numberOfMaximalConesOf(zf);

  polymake::Matrix<polymake::Integer> pm=PRaysOf(zf);
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

polymake::perl::Object ZFan2PMFan (gfan::ZFan* zf)
{
  polymake::perl::Object pf("PolyhedralFan");

  polymake::Matrix<polymake::Integer> zm = PRaysOf(zf);
  pf.take("INPUT_RAYS") << zm;

  polymake::Array<polymake::Set<int> > ar = PConesOf(zf);
  pf.take("INPUT_CONES") << ar;

  return pf;
}

/* Functions for using Polymake in Singular */

BOOLEAN cube(leftv res, leftv args)
{
  leftv u = args;
  if ((u !=NULL) && (u->Typ() == INT_CMD))
    {
      int ambientDim = (int)(long)u->Data();
      if (ambientDim < 0)
	{
          Werror("expected non-negative ambient dim but got %d", ambientDim);
	  return TRUE;
	}
      gfan::ZMatrix zm(ambientDim*2,ambientDim+1);
      int j=1;
      for (int i=0; i<ambientDim*2; i=i+2)
        {
          zm[i][0] = 1;
          zm[i][j] = 1;
          zm[i+1][0] = 1;
          zm[i+1][j] = -1;
          j = j+1;
        }
      gfan::ZCone* zc = new gfan::ZCone(zm, gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = coneID;
      res->data = (char *)zc;
      return FALSE;
    }
  WerrorS("cube: unexpected parameters");
  return TRUE;
}

BOOLEAN cross(leftv res, leftv args)
{
  leftv u = args;
  if ((u !=NULL) && (u->Typ() == INT_CMD))
    {
      int ambientDim = (int)(long)u->Data();
      if (ambientDim < 0)
	{
          Werror("expected non-negative ambient dim but got %d", ambientDim);
	  return TRUE;
	}
      gfan::ZMatrix zm(ambientDim*2,ambientDim+1);
      int j=1;
      for (int i=0; i<ambientDim*2; i=i+2)
        {
          zm[i][0] = 1;
          zm[i][j] = 1;
          zm[i+1][0] = 1;
          zm[i+1][j] = -1;
          j = j+1;
        }
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = coneID;
      res->data = (char *)zc;
      return FALSE;
    }
  WerrorS("cross: unexpected parameters");
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
      p.take("VERTICES") << ZMat2PMMat(rays);
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
      polymake::perl::Object pf=ZFan2PMFan(zf);
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

BOOLEAN normal_fan(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix rays = zc->extremeRays();
    polymake::perl::Object p("Polytope<Rational>");
    p.take("VERTICES") << ZMat2PMMat(rays);
    polymake::perl::Object pf;
    CallPolymakeFunction("normal_fan", p) >> pf;
    gfan::ZFan* zf = new gfan::ZFan(PMFan2ZFan(pf));
    res->rtyp = fanID;
    res->data = (char*) zf;
    return FALSE;
  }
  WerrorS("normal_fan: unexpected parameters");
  return TRUE; 
}


BOOLEAN testingfans(leftv res, leftv args)  // for testing purposes    
{                                           // creating a fan in polymake 
  leftv u = args;                           // and handing it to Singular
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    perl::Object pf = ZFan2PMFan(zf);
    // perl::Object pf("PolyhedralFan");         
    // Matrix<Integer> zm=(unit_matrix<Integer>(3));
    // pf.take("INPUT_RAYS") << zm;
    // Set<int> s;
    // s = s+0;
    // s = s+1;
    // s = s+2;
    // Array<Set<int> > ar(1);
    // ar[0]=s;
    // pf.take("INPUT_CONES") << ar;
    gfan::ZFan* zff = new gfan::ZFan(PMFan2ZFan(pf));
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
      polymake::perl::Object pc = ZCone2PMCone(*zc);
      gfan::ZCone* zd = new gfan::ZCone(PMCone2ZCone(pc));
      res->rtyp = coneID;
      res->data = (char *) zd;;
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
  iiAddCproc("","cube",FALSE,cube);
  iiAddCproc("","cross",FALSE,cross);
  iiAddCproc("","visual",FALSE,visual);
  iiAddCproc("","normalFan",FALSE,normal_fan);
  iiAddCproc("","testingcones",FALSE,testingcones);
  iiAddCproc("","testingfans",FALSE,testingfans);
  iiAddCproc("","testingvisuals",FALSE,testingvisuals);

}
