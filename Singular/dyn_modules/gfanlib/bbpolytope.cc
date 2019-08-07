#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/blackbox.h"
#include "misc/intvec.h"
#include "coeffs/bigintmat.h"

#include "callgfanlib_conversion.h"

#include "gfanlib/gfanlib.h"
#include "gfanlib/gfanlib_q.h"

THREAD_VAR int polytopeID;

std::string bbpolytopeToString(gfan::ZCone const &c)
{
  std::stringstream s;
  gfan::ZMatrix i=c.getInequalities();
  gfan::ZMatrix e=c.getEquations();
  s<<"AMBIENT_DIM"<<std::endl;
  s<<c.ambientDimension()-1<<std::endl;
  s<<"INEQUALITIES"<<std::endl;
  s<<toString(i)<<std::endl;
  s<<"EQUATIONS"<<std::endl;
  s<<toString(e)<<std::endl;
  return s.str();
}

void *bbpolytope_Init(blackbox* /*b*/)
{
  return (void*)(new gfan::ZCone());
}

BOOLEAN bbpolytope_Assign(leftv l, leftv r)
{
  gfan::ZCone* newZc;
  if (r==NULL)
  {
    if (l->Data()!=NULL)
    {
      gfan::ZCone* zd = (gfan::ZCone*)l->Data();
      delete zd;
    }
    newZc = new gfan::ZCone();
  }
  else if (r->Typ()==l->Typ())
  {
    if (l->Data()!=NULL)
    {
      gfan::ZCone* zd = (gfan::ZCone*)l->Data();
      delete zd;
    }
    gfan::ZCone* zc = (gfan::ZCone*)r->Data();
    newZc = new gfan::ZCone(*zc);
  }
  // else if (r->Typ()==INT_CMD)  TODO:r->Typ()==BIGINTMAT_CMD
  // {
  //   int ambientDim = (int)(long)r->Data();
  //   if (ambientDim < 0)
  //   {
  //     Werror("expected an int >= 0, but got %d", ambientDim);
  //     return TRUE;
  //   }
  //   if (l->Data()!=NULL)
  //   {
  //     gfan::ZCone* zd = (gfan::ZCone*)l->Data();
  //     delete zd;
  //   }
  //   newZc = new gfan::ZCone(ambientDim);
  // }
  else
  {
    Werror("assign Type(%d) = Type(%d) not implemented",l->Typ(),r->Typ());
    return TRUE;
  }

  if (l->rtyp==IDHDL)
  {
    IDDATA((idhdl)l->data) = (char*) newZc;
  }
  else
  {
    l->data=(void *)newZc;
  }
  return FALSE;
}

char* bbpolytope_String(blackbox* /*b*/, void *d)
{ if (d==NULL) return omStrDup("invalid object");
   else
   {
     gfan::ZCone* zc = (gfan::ZCone*)d;
     std::string s=bbpolytopeToString(*zc);
     return omStrDup(s.c_str());
   }
}

void bbpolytope_destroy(blackbox* /*b*/, void *d)
{
  if (d!=NULL)
  {
    gfan::ZCone* zc = (gfan::ZCone*) d;
    delete zc;
  }
}

void* bbpolytope_Copy(blackbox* /*b*/, void *d)
{
  gfan::ZCone* zc = (gfan::ZCone*)d;
  gfan::ZCone* newZc = new gfan::ZCone(*zc);
  return newZc;
}

static BOOLEAN ppCONERAYS1(leftv res, leftv v)
{
  /* method for generating a cone object from half-lines
     (cone = convex hull of the half-lines; note: there may be
     entire lines in the cone);
     valid parametrizations: (bigintmat) */
  bigintmat* rays = NULL;
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* rays0 = (intvec*) v->Data();
    rays = iv2bim(rays0,coeffs_BIGINT);
  }
  else
    rays = (bigintmat*) v->Data();

  gfan::ZMatrix* zm = bigintmatToZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
  res->rtyp = polytopeID;
  res->data = (void*) zc;

  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete rays;
  return FALSE;
}

static BOOLEAN ppCONERAYS3(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from half-lines
     (any point in the cone being the sum of a point
     in the convex hull of the half-lines and a point in the span
     of the lines), and an integer k;
     valid parametrizations: (bigintmat, int);
     Errors will be invoked in the following cases:
     - k not 0 or 1;
     if the k=1, then the extreme rays are known:
     each half-line spans a (different) extreme ray */
  bigintmat* rays = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* rays0 = (intvec*) u->Data();
    rays = iv2bim(rays0,coeffs_BIGINT);
  }
  else
    rays = (bigintmat*) u->Data();
  int k = (int)(long)v->Data();

  if ((k < 0) || (k > 1))
  {
    WerrorS("expected int argument in [0..1]");
    return TRUE;
  }
  k=k*2;
  gfan::ZMatrix* zm = bigintmatToZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm,gfan::ZMatrix(0, zm->getWidth()));
  //k should be passed on to zc; not available yet
  res->rtyp = polytopeID;
  res->data = (void*) zc;

  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete rays;
  return FALSE;
}

BOOLEAN polytopeViaVertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL)
    {
      gfan::initializeCddlibIfRequired();
      BOOLEAN bo = ppCONERAYS1(res, u);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      if (v->next == NULL)
      {
        gfan::initializeCddlibIfRequired();
        BOOLEAN bo = ppCONERAYS3(res, u, v);
        gfan::deinitializeCddlibIfRequired();
        return bo;
      }
    }
  }
  WerrorS("polytopeViaPoints: unexpected parameters");
  return TRUE;
}

static BOOLEAN ppCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (bigintmat) */
  bigintmat* ineq = NULL;
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) v->Data();
    ineq = iv2bim(ineq0,coeffs_BIGINT);
  }
  else
    ineq = (bigintmat*) v->Data();
  gfan::ZMatrix* zm = bigintmatToZMatrix(ineq);
  gfan::ZCone* zc = new gfan::ZCone(*zm, gfan::ZMatrix(0, zm->getWidth()));
  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete ineq;
  res->rtyp = polytopeID;
  res->data = (void*) zc;
  return FALSE;
}

static BOOLEAN ppCONENORMALS2(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from iequalities,
     and equations (...)
     valid parametrizations: (bigintmat, bigintmat)
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns */
  bigintmat* ineq = NULL; bigintmat* eq = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) u->Data();
    ineq = iv2bim(ineq0,coeffs_BIGINT);
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0,coeffs_BIGINT);
  }
  else
    eq = (bigintmat*) v->Data();

  if (ineq->cols() != eq->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           ineq->cols(), eq->cols());
    return TRUE;
  }
  gfan::ZMatrix* zm1 = bigintmatToZMatrix(ineq);
  gfan::ZMatrix* zm2 = bigintmatToZMatrix(eq);
  gfan::ZCone* zc = new gfan::ZCone(*zm1, *zm2);
  delete zm1;
  delete zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;

  res->rtyp = polytopeID;
  res->data = (void*) zc;
  return FALSE;
}

static BOOLEAN ppCONENORMALS3(leftv res, leftv u, leftv v, leftv w)
{
  /* method for generating a cone object from inequalities, equations,
     and an integer k;
     valid parametrizations: (bigintmat, bigintmat, int);
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns,
     - k not in [0..3];
     if the 2^0-bit of k is set, then ... */
  bigintmat* ineq = NULL; bigintmat* eq = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) u->Data();
    ineq = iv2bim(ineq0,coeffs_BIGINT);
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0,coeffs_BIGINT);
  }
  else
    eq = (bigintmat*) v->Data();

  if (ineq->cols() != eq->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           ineq->cols(), eq->cols());
    return TRUE;
  }
  int k = (int)(long)w->Data();
  if ((k < 0) || (k > 3))
  {
    WerrorS("expected int argument in [0..3]");
    return TRUE;
  }
  gfan::ZMatrix* zm1 = bigintmatToZMatrix(ineq);
  gfan::ZMatrix* zm2 = bigintmatToZMatrix(eq);
  gfan::ZCone* zc = new gfan::ZCone(*zm1, *zm2, k);
  delete zm1;
  delete zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;

  res->rtyp = polytopeID;
  res->data = (void*) zc;
  return FALSE;
}

BOOLEAN polytopeViaNormals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL)
    {
      gfan::initializeCddlibIfRequired();
      BOOLEAN bo = ppCONENORMALS1(res, u);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
  }
  leftv v = u->next;
  if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
  {
    if (v->next == NULL)
    {
      gfan::initializeCddlibIfRequired();
      BOOLEAN bo = ppCONENORMALS2(res, u, v);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL)
    {
      gfan::initializeCddlibIfRequired();
      BOOLEAN bo = ppCONENORMALS3(res, u, v, w);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
  }
  WerrorS("polytopeViaInequalities: unexpected parameters");
  return TRUE;
}

BOOLEAN vertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->extremeRays();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("vertices: unexpected parameters");
  return TRUE;
}

int getAmbientDimension(gfan::ZCone* zc) // zc is meant to represent a polytope here
{                                        // hence ambientDimension-1
  return zc->ambientDimension()-1;
}

int getCodimension(gfan::ZCone *zc)
{
  return zc->codimension();
}

int getDimension(gfan::ZCone* zc)
{
  return zc->dimension()-1;
}

gfan::ZVector intStar2ZVectorWithLeadingOne(const int d, const int* i)
{
  gfan::ZVector zv(d+1);
  zv[0]=1;
  for(int j=1; j<=d; j++)
  {
    zv[j]=i[j];
  }
  return zv;
}

gfan::ZCone newtonPolytope(poly p, ring r)
{
  int N = rVar(r);
  gfan::ZMatrix zm(0,N+1);
  int *leadexpv = (int*)omAlloc((N+1)*sizeof(int));
  while (p!=NULL)
  {
    p_GetExpV(p,leadexpv,r);
    gfan::ZVector zv = intStar2ZVectorWithLeadingOne(N, leadexpv);
    zm.appendRow(zv);
    pIter(p);
  }
  omFreeSize(leadexpv,(N+1)*sizeof(int));
  gfan::ZCone Delta = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
  return Delta;
}

BOOLEAN newtonPolytope(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    gfan::initializeCddlibIfRequired();
    poly p = (poly)u->Data();
    res->rtyp = polytopeID;
    res->data = (void*) new gfan::ZCone(newtonPolytope(p,currRing));
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("newtonPolytope: unexpected parameters");
  return TRUE;
}

BOOLEAN scalePolytope(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      int s = (int)(long) u->Data();
      gfan::ZCone* zp = (gfan::ZCone*) v->Data();
      gfan::ZMatrix zm = zp->extremeRays();
      for (int i=0; i<zm.getHeight(); i++)
        for (int j=1; j<zm.getWidth(); j++)
          zm[i][j]*=s;
      gfan::ZCone* zq = new gfan::ZCone();
      *zq = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = polytopeID;
      res->data = (void*) zq;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  WerrorS("scalePolytope: unexpected parameters");
  return TRUE;
}

BOOLEAN dualPolytope(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zp = (gfan::ZCone*) u->Data();
    gfan::ZCone* zq = new gfan::ZCone(zp->dualCone());
    res->rtyp = polytopeID;
    res->data = (void*) zq;
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("dualPolytope: unexpected parameters");
  return TRUE;
}

BOOLEAN mixedVolume(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    gfan::initializeCddlibIfRequired();
    lists l = (lists) u->Data();
    int k = lSize(l)+1;
    std::vector<gfan::IntMatrix> P(k);
    for (int i=0; i<k; i++)
    {
      if (l->m[i].Typ() == polytopeID)
      {
        gfan::ZCone* p = (gfan::ZCone*) l->m[i].Data();
        gfan::ZMatrix pv = p->extremeRays();
        int r = pv.getHeight();
        int c = pv.getWidth();
        gfan::IntMatrix pw(r,c-1);
        for (int n=0; n<r; n++)
          for (int m=1; m<c; m++)
            pw[n][m-1] = pv[n][m].toInt();
        P[i]=pw.transposed();
      } else if (l->m[i].Typ() == POLY_CMD)
      {
        poly p = (poly) l->m[i].Data();
        int N = rVar(currRing);
        gfan::IntMatrix pw(0,N);
        int *leadexpv = (int*)omAlloc((N+1)*sizeof(int));
        while (p!=NULL)
        {
          p_GetExpV(p,leadexpv,currRing);
          gfan::IntVector zv(N);
          for (int i=0; i<N; i++)
            zv[i] = leadexpv[i+1];
          pw.appendRow(zv);
          pIter(p);
        }
        P[i]=pw.transposed();
        omFreeSize(leadexpv,(N+1)*sizeof(int));
      }
      else
      {
        WerrorS("mixedVolume: entries of unsupported type in list");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
    }
    gfan::Integer mv = gfan::mixedVolume(P);

    res->rtyp = BIGINT_CMD;
    res->data = (void*) integerToNumber(mv);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("mixedVolume: unexpected parameters");
  return TRUE;
}



void bbpolytope_setup(SModulFunctions* p)
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefule,
  // all other are simply error messages
  b->blackbox_destroy=bbpolytope_destroy;
  b->blackbox_String=bbpolytope_String;
  //b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=bbpolytope_Init;
  b->blackbox_Copy=bbpolytope_Copy;
  b->blackbox_Assign=bbpolytope_Assign;
  p->iiAddCproc("gfan.lib","polytopeViaPoints",FALSE,polytopeViaVertices);
  p->iiAddCproc("gfan.lib","polytopeViaInequalities",FALSE,polytopeViaNormals);
  p->iiAddCproc("gfan.lib","vertices",FALSE,vertices);
  p->iiAddCproc("gfan.lib","newtonPolytope",FALSE,newtonPolytope);
  p->iiAddCproc("gfan.lib","scalePolytope",FALSE,scalePolytope);
  p->iiAddCproc("gfan.lib","dualPolytope",FALSE,dualPolytope);
  p->iiAddCproc("gfan.lib","mixedVolume",FALSE,mixedVolume);
  /********************************************************/
  /* the following functions are implemented in bbcone.cc */
  // iiAddCproc("gfan.lib","getAmbientDimension",FALSE,getAmbientDimension);
  // iiAddCproc("gfan.lib","getCodimension",FALSE,getAmbientDimension);
  // iiAddCproc("gfan.lib","getDimension",FALSE,getDimension);
  /********************************************************/
  /* the following functions are identical to those in bbcone.cc */
  // iiAddCproc("gfan.lib","facets",FALSE,facets);
  // iiAddCproc("gfan.lib","setLinearForms",FALSE,setLinearForms);
  // iiAddCproc("gfan.lib","getLinearForms",FALSE,getLinearForms);
  // iiAddCproc("gfan.lib","setMultiplicity",FALSE,setMultiplicity);
  // iiAddCproc("gfan.lib","getMultiplicity",FALSE,getMultiplicity);
  // iiAddCproc("gfan.lib","hasFace",FALSE,hasFace);
  /***************************************************************/
  // iiAddCproc("gfan.lib","getEquations",FALSE,getEquations);
  // iiAddCproc("gfan.lib","getInequalities",FALSE,getInequalities);
  polytopeID=setBlackboxStuff(b,"polytope");
  //Print("created type %d (polytope)\n",polytopeID);
}

#endif
