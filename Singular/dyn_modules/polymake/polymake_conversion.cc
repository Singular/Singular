#include "kernel/mod2.h"

#ifdef HAVE_POLYMAKE

#include <gmpxx.h>

#include <polymake/Main.h>
#include <polymake/Matrix.h>
#include <polymake/Rational.h>
#include <polymake/Integer.h>
#include <polymake/Set.h>
#include <polymake/common/lattice_tools.h>
#include <polymake/IncidenceMatrix.h>

#include "gfanlib/gfanlib.h"
#include "gfanlib/gfanlib_q.h"

#include "misc/intvec.h"
#include "coeffs/numbers.h"
#include "coeffs/bigintmat.h"
#include "Singular/lists.h"
#include "Singular/ipid.h" // for bigints,
// is there really nothing better than this?

/* Functions for converting Integers, Rationals and their Matrices
   in between C++, gfan, polymake and singular */

/* gfan -> polymake */

polymake::Integer GfInteger2PmInteger (const gfan::Integer& gi)
{
  mpz_t cache; mpz_init(cache);
  gi.setGmp(cache);
  polymake::Integer pi(cache);
  return pi;
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

/* gfan <- polymake */

gfan::Integer PmInteger2GfInteger (const polymake::Integer& pi)
{
  mpz_class cache(pi.get_rep());
  gfan::Integer gi(cache.get_mpz_t());
  return gi;
}

gfan::Rational PmRational2GfRational (const polymake::Rational& pr)
{
  mpq_class cache(pr.get_rep());
  gfan::Rational gr(cache.get_mpq_t());
  return gr;
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

/* polymake -> singular */

int PmInteger2Int(const polymake::Integer& pi, bool &ok)
{
  int i=0;
  try
  {
#if POLYMAKE_VERSION >= 301 /* 3.1 */
    i=int(pi);
#else
    i = pi.to_int();
#endif
  }
  catch (const std::exception& ex)
  {
    WerrorS("ERROR: "); WerrorS(ex.what()); WerrorS("\n");
    ok = false;
  }
  return i;
}

number PmInteger2Number (const polymake::Integer& pi)
{
  mpz_class cache(pi.get_rep());
  long m = 268435456;
  if(mpz_cmp_si(cache.get_mpz_t(),m))
  {
    int temp = (int) mpz_get_si(cache.get_mpz_t());
    return n_Init(temp,coeffs_BIGINT);
  }
  else
    return n_InitMPZ(cache.get_mpz_t(),coeffs_BIGINT);
}

intvec* PmVectorInteger2Intvec (const polymake::Vector<polymake::Integer>* vi, bool &ok)
{
  intvec* iv = new intvec(vi->size());
  for(int i=1; i<=vi->size(); i++)
  {
    (*iv)[i-1] = PmInteger2Int((*vi)[i-1],ok);
  }
  return iv;
}

intvec* PmMatrixInteger2Intvec (polymake::Matrix<polymake::Integer>* mi, bool &ok)
{
  int rows = mi->rows();
  int cols = mi->cols();
  intvec* iv = new intvec(rows,cols,0);
#if POLYMAKE_VERSION >= 301 /*3.1*/
  pm::array_traits<pm::Integer>::iterator pi = concat_rows(*mi).begin();
#else
  const polymake::Integer* pi = concat_rows(*mi).begin();
#endif
  for (int r = 1; r <= rows; r++)
    for (int c = 1; c <= cols; c++)
    {
      IMATELEM(*iv,r,c) = PmInteger2Int(*pi, ok);
      pi++;
    }
  return iv;
}

bigintmat* PmMatrixInteger2Bigintmat (polymake::Matrix<polymake::Integer>* mi)
{
  int rows = mi->rows();
  int cols = mi->cols();
  bigintmat* bim= new bigintmat(rows,cols,coeffs_BIGINT);
#if POLYMAKE_VERSION >= 301 /*3.1*/
  pm::array_traits<pm::Integer>::iterator pi = concat_rows(*mi).begin();
#else
  const polymake::Integer* pi = concat_rows(*mi).begin();
#endif
  for (int r = 1; r <= rows; r++)
    for (int c = 1; c <= cols; c++)
    {
      number temp = PmInteger2Number(*pi);
      bim->set(r,c,temp);
      n_Delete(&temp,coeffs_BIGINT);
      pi++;
    }
  return bim;
}

lists PmIncidenceMatrix2ListOfIntvecs (polymake::IncidenceMatrix<polymake::NonSymmetric>* icmat)
{
  int rows = icmat->rows();
  int cols = icmat->cols();
  lists L = (lists)omAllocBin(slists_bin);
  L->Init(rows);

  for (int r = 0; r < rows; r++)
  {
    intvec* iv = new intvec(cols); int i=0;
    for (int c = 0; c < cols; c++)
    {
      if ((*icmat).row(r).exists(c))
      { (*iv)[i]=c; i++; }
    }
    iv->resize(i);
    L->m[r].rtyp = INTVEC_CMD;
    L->m[r].data = (void*) iv;
  }

  return L;
}

lists PmAdjacencyMatrix2ListOfEdges (polymake::IncidenceMatrix<polymake::NonSymmetric>* icmat)
{
  int rows = icmat->rows();
  int cols = icmat->cols();

  // counting number of edges
  int i=0; int r, c;
  for (r=0; r<rows; r++)
  {
    for (c=0; c<cols; c++)
    {
      if ((*icmat).row(r).exists(c) && r<c)
        i++;
    }
  }

  lists L = (lists)omAllocBin(slists_bin);
  L->Init(i);

  i=0;
  for (r=0; r<rows; r++)
  {
    for (c=0; c<cols; c++)
    {
      if ((*icmat).row(r).exists(c) && r<c)
      {
        intvec* iv = new intvec(2);
        (*iv)[0]=r; (*iv)[1]=c;
        L->m[i].rtyp = INTVEC_CMD;
        L->m[i].data = (void*) iv;
        i++;
      }
    }
  }

  return L;
}

intvec* PmSetInteger2Intvec (polymake::Set<polymake::Integer>* si, bool &b)
{
  polymake::Vector<polymake::Integer> vi(*si);
  return PmVectorInteger2Intvec(&vi,b);
}

/* polymake <- singular */

polymake::Matrix<polymake::Integer> Intvec2PmMatrixInteger (const intvec* im)
{
  int rows=im->rows();
  int cols=im->cols();
  polymake::Matrix<polymake::Integer> mi(rows,cols);
  for(int r=0; r<rows; r++)
    for(int c=0; c<cols; c++)
      mi(r,c) = polymake::Integer(IMATELEM(*im, r+1, c+1));
  return mi;
}

/* Functions for converting cones and fans in between gfan and polymake,
   Singular shares the same cones and fans with gfan */

gfan::ZCone* PmCone2ZCone (polymake::perl::Object* pc)
{
  if (pc->isa("Cone"))
  {
    polymake::Integer ambientdim1 = pc->give("CONE_AMBIENT_DIM");
    bool ok=true; int ambientdim2 = PmInteger2Int(ambientdim1, ok);
    if (!ok)
    {
      WerrorS("PmCone2ZCone: overflow while converting polymake::Integer to int");
    }
    polymake::Matrix<polymake::Rational> ineqrational = pc->give("FACETS");
    polymake::Matrix<polymake::Rational> eqrational = pc->give("LINEAR_SPAN");
    // polymake::Matrix<polymake::Rational> exraysrational = pc->give("RAYS");
    // polymake::Matrix<polymake::Rational> linrational = pc->give("LINEALITY_SPACE");

    gfan::ZMatrix zv, zw, zx, zy, zz;
    // the following branching statements are to cover cases in which polymake returns empty matrices
    // by convention, gfanlib ignores empty matrices, hence zero matrices of right dimensions have to be supplied
    if (ineqrational.cols()!=0)
    {
      polymake::Matrix<polymake::Integer> ineqinteger = polymake::common::primitive(ineqrational);
      zv = PmMatrixInteger2GfZMatrix(&ineqinteger);
    }
    else
      zv = gfan::ZMatrix(0, ambientdim2);
    if (eqrational.cols()!=0)
    {
      polymake::Matrix<polymake::Integer> eqinteger = polymake::common::primitive(eqrational);
      zw = PmMatrixInteger2GfZMatrix(&eqinteger);
    }
    else
      zw = gfan::ZMatrix(0, ambientdim2);
    // if (exraysrational.cols()!=0)
    // {
    //   polymake::Matrix<polymake::Integer> exraysinteger = polymake::common::primitive(exraysrational);
    //   zx = PmMatrixInteger2GfZMatrix(&exraysinteger);
    // }
    // else
    //   zx = gfan::ZMatrix(0, ambientdim2);
    // if (linrational.cols()!=0)
    // {
    //   polymake::Matrix<polymake::Integer> lininteger = polymake::common::primitive(linrational);
    //   zy = PmMatrixInteger2GfZMatrix(&lininteger);
    // }
    // else
    //   zy = gfan::ZMatrix(0, ambientdim2);

    // gfan::ZCone* zc = new gfan::ZCone(zv,zw,zx,zy,zz,3);
    gfan::ZCone* zc = new gfan::ZCone(zv,zw,3);
    return zc;
  }
  WerrorS("PmCone2ZCone: unexpected parameters");
  return NULL;
}

gfan::ZCone* PmPolytope2ZPolytope (polymake::perl::Object* pp)
{
  if (pp->isa("Polytope<Rational>"))
  {
    polymake::Integer ambientdim1 = pp->give("CONE_AMBIENT_DIM");
    bool ok=true; int ambientdim2 = PmInteger2Int(ambientdim1, ok);
    if (!ok)
    {
      WerrorS("overflow while converting polymake::Integer to int");
    }
    polymake::Matrix<polymake::Rational> ineqrational = pp->give("FACETS");
    polymake::Matrix<polymake::Rational> eqrational = pp->give("AFFINE_HULL");
    // polymake::Matrix<polymake::Rational> vertrational = pp->give("VERTICES");
    // polymake::Matrix<polymake::Rational> linrational = pp->give("LINEALITY_SPACE");

    gfan::ZMatrix zv, zw;
    // the following branching statements are to cover the cases when polymake returns empty matrices
    // by convention, gfanlib ignores empty matrices, hence zero matrices of right dimensions have to be supplied
    if (ineqrational.cols()!=0)
    {
      polymake::Matrix<polymake::Integer> ineqinteger = polymake::common::primitive(ineqrational);
      zv = PmMatrixInteger2GfZMatrix(&ineqinteger);
    }
    else
      zv = gfan::ZMatrix(0, ambientdim2);

    if (eqrational.cols()!=0)
    {
      polymake::Matrix<polymake::Integer> eqinteger = polymake::common::primitive(eqrational);
      zw = PmMatrixInteger2GfZMatrix(&eqinteger);
    }
    else
      zw = gfan::ZMatrix(0, ambientdim2);

    // if (vertrational.cols()!=0)
    // {
    //   polymake::Matrix<polymake::Integer> vertinteger = polymake::common::primitive(vertrational);
    //   zx = PmMatrixInteger2GfZMatrix(&vertinteger);
    // }
    // else
    //   zx = gfan::ZMatrix(0, ambientdim2);
    // if (linrational.cols()!=0)
    //   {
    //     polymake::Matrix<polymake::Integer> lininteger = polymake::common::primitive(linrational);
    //     zy = PmMatrixInteger2GfZMatrix(&lininteger);
    //   }
    // else
    //   zy = gfan::ZMatrix(0, ambientdim2);

    // gfan::ZCone* zp = new gfan::ZCone(zv,zw,zx,zy,zz,3);
    gfan::ZCone* zp = new gfan::ZCone(zv,zw,3);

    return zp;
  }
  WerrorS("PmPolytope2ZPolytope: unexpected parameters");
  return NULL;
}

gfan::ZFan* PmFan2ZFan (polymake::perl::Object* pf)
{
  if (pf->isa("PolyhedralFan"))
  {
    int d = (int) pf->give("FAN_AMBIENT_DIM");
    gfan::ZFan* zf = new gfan::ZFan(d);

    int n = pf->give("N_MAXIMAL_CONES");
    for (int i=0; i<n; i++)
    {
      polymake::perl::Object pmcone=pf->call_method("cone",i);
      gfan::ZCone* zc=PmCone2ZCone(&pmcone);
      zf->insert(*zc);
    }
    return zf;
  }
  WerrorS("PmFan2ZFan: unexpected parameters");
  return NULL;
}

polymake::perl::Object* ZCone2PmCone (gfan::ZCone* zc)
{
  polymake::perl::Object* gc = new polymake::perl::Object("Cone<Rational>");

  gfan::ZMatrix inequalities = zc->getInequalities();
  gc->take("FACETS") << GfZMatrix2PmMatrixInteger(&inequalities);

  gfan::ZMatrix equations = zc->getEquations();
  gc->take("LINEAR_SPAN") << GfZMatrix2PmMatrixInteger(&equations);

  // if(zc->areExtremeRaysKnown())
  //   {
  //     gfan::ZMatrix extremeRays = zc->extremeRays();
  //     gc->take("RAYS") << GfZMatrix2PmMatrixInteger(&extremeRays);
  //   }

  // if(zc->areGeneratorsOfLinealitySpaceKnown())
  //   {
  //     gfan::ZMatrix lineality = zc->generatorsOfLinealitySpace();
  //     gc->take("LINEALITY_SPACE") << GfZMatrix2PmMatrixInteger(&lineality);
  //   }

  return gc;
}

polymake::perl::Object* ZPolytope2PmPolytope (gfan::ZCone* zc)
{
  polymake::perl::Object* pp = new polymake::perl::Object("Polytope<Rational>");

  gfan::ZMatrix inequalities = zc->getInequalities();
  pp->take("FACETS") << GfZMatrix2PmMatrixInteger(&inequalities);

  gfan::ZMatrix equations = zc->getEquations();
  pp->take("LINEAR_SPAN") << GfZMatrix2PmMatrixInteger(&equations);

  // if(zc->areExtremeRaysKnown())
  //   {
  //     gfan::ZMatrix vertices = zc->extremeRays();
  //     pp->take("VERTICES") << GfZMatrix2PmMatrixInteger(&vertices);
  //   }

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

  for (int i=0; i<=d; i++)
    {
      n = n + zf->numberOfConesOfDimension(i,0,1);
    }

  return n;
}

polymake::Array<polymake::Set<int> > conesOf(gfan::ZFan* zf)
{
  int r = numberOfMaximalConesOf(zf);

  polymake::Matrix<polymake::Integer> pm=raysOf(zf);
  polymake::Array<polymake::Set<int> > L(r);

  int ii = 0;
  for (int d=1; d<=zf->getAmbientDimension(); d++)
    {
      for (int i=0; i<zf->numberOfConesOfDimension(d,0,1); i++)
        {
          gfan::IntVector v = zf->getConeIndices(d,i,0,1);
          polymake::Set<int> s;
          for (int j=0; j<(int)v.size(); j++)
            {
              s = s+v[j];
            }
          L[ii] = s;
          ii = ii + 1;
        }
    }
  return L;
}

polymake::perl::Object* ZFan2PmFan (gfan::ZFan* zf)
{
  polymake::perl::Object* pf = new polymake::perl::Object("PolyhedralFan");

  polymake::Matrix<polymake::Integer> zm = raysOf(zf);
  pf->take("RAYS") << zm;  // using rays here instead of INPUT_RAYS prevents redundant computations

  polymake::Array<polymake::Set<int> > ar = conesOf(zf);
  pf->take("MAXIMAL_CONES") << ar;

  return pf;
}

#endif
