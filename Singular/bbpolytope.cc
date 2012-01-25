#include <Singular/mod2.h>

#ifdef HAVE_FANS

#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <gfanlib/gfanlib.h>
#include <kernel/bbcone.h>
#include <Singular/ipshell.h>
#include <kernel/intvec.h>
#include <sstream>


int polytopeID;

std::string bbpolytopeToString(gfan::ZCone const &c)
{
  std::stringstream s;
  gfan::ZMatrix r=c.extremeRays();
  s<<"VERTICES"<<std::endl;
  s<<toString(r);
  return s.str();
}

void *bbpolytope_Init(blackbox *b)
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
  // else if (r->Typ()==INT_CMD)  TODO:r->Typ()==INTMAT_CMD
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
    IDDATA((idhdl)l->data)=(char *)newZc;
  }
  else
  {
    l->data=(void *)newZc;
  }
  return FALSE;
}

char* bbpolytope_String(blackbox *b, void *d)
{ if (d==NULL) return omStrDup("invalid object");
   else
   {
     std::string s=bbpolytopeToString(*((gfan::ZCone*)d));
     return omStrDup(s.c_str());
   }
}

void bbpolytope_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    gfan::ZCone* zc = (gfan::ZCone*) d;
    delete zc;
  }
}

void * bbpolytope_Copy(blackbox*b, void *d)
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
     valid parametrizations: (intmat) */
  intvec* rays = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
  zc->canonicalize();
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN ppCONERAYS3(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from half-lines
     (any point in the cone being the sum of a point
     in the convex hull of the half-lines and a point in the span
     of the lines), and an integer k;
     valid parametrizations: (intmat, int);
     Errors will be invoked in the following cases:
     - k not 0 or 1;
     if the k=1, then the extreme rays are known:
     each half-line spans a (different) extreme ray */
  intvec* rays = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* linSpace = (intvec *)v->CopyD(INTVEC_CMD);
  if (rays->cols() != linSpace->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           rays->cols(), linSpace->cols());
    return TRUE;
  }
  int k = (int)(long)v->Data();
  if ((k < 0) || (k > 1))
  {
    WerrorS("expected int argument in [0..1]");
    return TRUE;
  }
  k=k*2;
  gfan::ZMatrix zm1 = intmat2ZMatrix(rays);
  gfan::ZMatrix zm2;
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm1, zm2);
  zc->canonicalize();
  //k should be passed on to zc; not available yet
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN polytopeViaVertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return ppCONERAYS1(res, u);
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      if (v->next == NULL) return ppCONERAYS3(res, u, v);
    }
  }
  WerrorS("polytopeViaVertices: unexpected parameters");
  return TRUE;
}

static BOOLEAN pqCONERAYS1(leftv res, leftv v)
{
  /* method for generating a cone object from half-lines
     (cone = convex hull of the half-lines; note: there may be
     entire lines in the cone);
     valid parametrizations: (intmat) */
  intvec* rays = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN pqCONERAYS3(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from half-lines
     (any point in the cone being the sum of a point
     in the convex hull of the half-lines and a point in the span
     of the lines), and an integer k;
     valid parametrizations: (intmat, int);
     Errors will be invoked in the following cases:
     - k not 0 or 1;
     if the k=1, then the extreme rays are known:
     each half-line spans a (different) extreme ray */
  intvec* rays = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* linSpace = (intvec *)v->CopyD(INTVEC_CMD);
  if (rays->cols() != linSpace->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           rays->cols(), linSpace->cols());
    return TRUE;
  }
  int k = (int)(long)v->Data();
  if ((k < 0) || (k > 1))
  {
    WerrorS("expected int argument in [0..1]");
    return TRUE;
  }
  k=k*2;
  gfan::ZMatrix zm1 = intmat2ZMatrix(rays);
  gfan::ZMatrix zm2;
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm1, zm2);
  //k should be passed on to zc; not available yet
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN quickPolytopeViaVertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return pqCONERAYS1(res, u);
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      if (v->next == NULL) return pqCONERAYS3(res, u, v);
    }
  }
  WerrorS("quickPolytopeViaVertices: unexpected parameters");
  return TRUE;
}

static BOOLEAN ppCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
  intvec* inequs = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(inequs);
  gfan::ZCone* zc = new gfan::ZCone(zm, gfan::ZMatrix(0, zm.getWidth()));
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN ppCONENORMALS2(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from iequalities,
     and equations (...)
     valid parametrizations: (intmat, intmat)
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns */
  intvec* inequs = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* equs = (intvec *)v->CopyD(INTVEC_CMD);
  if (inequs->cols() != equs->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           inequs->cols(), equs->cols());
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(inequs);
  gfan::ZMatrix zm2 = intmat2ZMatrix(equs);
  gfan::ZCone* zc = new gfan::ZCone(zm1, zm2);
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN ppCONENORMALS3(leftv res, leftv u, leftv v, leftv w)
{
  /* method for generating a cone object from inequalities, equations,
     and an integer k;
     valid parametrizations: (intmat, intmat, int);
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns,
     - k not in [0..3];
     if the 2^0-bit of k is set, then ... */
  intvec* inequs = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* equs = (intvec *)v->CopyD(INTVEC_CMD);
  if (inequs->cols() != equs->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           inequs->cols(), equs->cols());
    return TRUE;
  }
  int k = (int)(long)w->Data();
  if ((k < 0) || (k > 3))
  {
    WerrorS("expected int argument in [0..3]");
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(inequs);
  gfan::ZMatrix zm2 = intmat2ZMatrix(equs);
  gfan::ZCone* zc = new gfan::ZCone(zm1, zm2, k);
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN polytopeViaNormals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return ppCONENORMALS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && (v->Typ() == INTMAT_CMD))
  {
    if (v->next == NULL) return ppCONENORMALS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return ppCONENORMALS3(res, u, v, w);
  }
  WerrorS("polytopeViaNormals: unexpected parameters");
  return TRUE;
}

static BOOLEAN pqCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
  intvec* inequs = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(inequs);
  gfan::ZCone* zc = new gfan::ZCone(zm, gfan::ZMatrix(0, zm.getWidth()));
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN pqCONENORMALS2(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from iequalities,
     and equations (...)
     valid parametrizations: (intmat, intmat)
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns */
  intvec* inequs = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* equs = (intvec *)v->CopyD(INTVEC_CMD);
  if (inequs->cols() != equs->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           inequs->cols(), equs->cols());
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(inequs);
  gfan::ZMatrix zm2 = intmat2ZMatrix(equs);
  gfan::ZCone* zc = new gfan::ZCone(zm1, zm2);
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN pqCONENORMALS3(leftv res, leftv u, leftv v, leftv w)
{
  /* method for generating a cone object from inequalities, equations,
     and an integer k;
     valid parametrizations: (intmat, intmat, int);
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns,
     - k not in [0..3];
     if the 2^0-bit of k is set, then ... */
  intvec* inequs = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* equs = (intvec *)v->CopyD(INTVEC_CMD);
  if (inequs->cols() != equs->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           inequs->cols(), equs->cols());
    return TRUE;
  }
  int k = (int)(long)w->Data();
  if ((k < 0) || (k > 3))
  {
    WerrorS("expected int argument in [0..3]");
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(inequs);
  gfan::ZMatrix zm2 = intmat2ZMatrix(equs);
  gfan::ZCone* zc = new gfan::ZCone(zm1, zm2, k);
  res->rtyp = polytopeID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN quickPolytopeViaNormals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return ppCONENORMALS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && (v->Typ() == INTMAT_CMD))
  {
    if (v->next == NULL) return ppCONENORMALS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return ppCONENORMALS3(res, u, v, w);
  }
  WerrorS("quickPolytopeViaNormals: unexpected parameters");
  return TRUE;
}

BOOLEAN getVertices(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == polytopeID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZMatrix zmat = zc->extremeRays();
      res->rtyp = INTMAT_CMD;
      res->data = (void*)zMatrix2Intvec(zmat);
      return FALSE;
    }
  WerrorS("getVertices: unexpected parameters");
  return TRUE;
}

intvec* getFacetNormals(gfan::ZCone* zc)
{
  gfan::ZMatrix zmat = zc->getFacets();
  return zMatrix2Intvec(zmat);
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

void bbpolytope_setup()
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
  iiAddCproc("","polytopeViaVertices",FALSE,polytopeViaVertices);
  iiAddCproc("","polytopeViaNormals",FALSE,polytopeViaNormals);
  iiAddCproc("","quickPolytopeViaVertices",FALSE,quickPolytopeViaVertices);
  iiAddCproc("","quickPolytopeViaNormals",FALSE,quickPolytopeViaNormals);
  iiAddCproc("","getVertices",FALSE,getVertices);
  /********************************************************/
  /* the following functions are implemented in bbcone.cc */
  // iiAddCproc("","getAmbientDimension",FALSE,getAmbientDimension);                                               
  // iiAddCproc("","getCodimension",FALSE,getAmbientDimension);                                                 
  // iiAddCproc("","getDimension",FALSE,getDimension);                                                                // iiAddCproc("","getFacetNormals",FALSE,getFacetNormals);  
  /********************************************************/
  /* the following functions are identical to those in bbcone.cc */
  // iiAddCproc("","setLinearForms",FALSE,setLinearForms);
  // iiAddCproc("","getLinearForms",FALSE,getLinearForms);
  // iiAddCproc("","setMultiplicity",FALSE,setMultiplicity);
  // iiAddCproc("","getMultiplicity",FALSE,getMultiplicity);
  /***************************************************************/
  // iiAddCproc("","getEquations",FALSE,getEquations);
  // iiAddCproc("","getInequalities",FALSE,getInequalities);
  polytopeID=setBlackboxStuff(b,"polytope");
  //Print("created type %d (polytope)\n",polytopeID); 
}

#endif
/* HAVE_FANS */
