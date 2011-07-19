#include <kernel/mod2.h>

#ifdef HAVE_FANS

#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <gfanlib/gfanlib.h>
#include <kernel/bbcone.h>
#include <ipshell.h>
#include <kernel/intvec.h>
#include <sstream>


int coneID;

int integerToInt(gfan::Integer const &V, bool &ok)
{
  mpz_t v;
  mpz_init(v);
  V.setGmp(v);
  int ret=0;
  if(mpz_fits_sint_p(v))
    ret=mpz_get_si(v);
  else
    ok=false;
  mpz_clear(v);
  return ret;
}

intvec* zVector2Intvec(const gfan::ZVector zv)
{
  int d=zv.size();
  intvec* iv = new intvec(1, d, 0);
  bool ok = true;
  for(int i=1;i<=d;i++)
    IMATELEM(*iv, 1, i) = integerToInt(zv[i-1], ok);
  if (!ok) WerrorS("overflow while converting a gfan::ZVector to an intvec");
  return iv;
}

intvec* zMatrix2Intvec(const gfan::ZMatrix zm)
{
  int d=zm.getHeight();
  int n=zm.getWidth();
  intvec* iv = new intvec(d, n, 0);
  bool ok = true;
  for(int i=1;i<=d;i++)
    for(int j=1;j<=n;j++)
      IMATELEM(*iv, i, j) = integerToInt(zm[i-1][j-1], ok);
  if (!ok) WerrorS("overflow while converting a gfan::ZMatrix to an intmat");
  return iv;
}

gfan::ZMatrix intmat2ZMatrix(const intvec* iMat)
{
  int d=iMat->rows();
  int n=iMat->cols();
  gfan::ZMatrix ret(d,n);
  for(int i=0;i<d;i++)
    for(int j=0;j<n;j++)
      ret[i][j]=IMATELEM(*iMat, i+1, j+1);
  return ret;
}

/* expects iMat to have just one row */
gfan::ZVector intvec2ZVector(const intvec* iVec)
{
  int n =iVec->rows();
  gfan::ZVector ret(n);
  for(int j=0;j<n;j++)
    ret[j]=IMATELEM(*iVec, j+1, 1);
  return ret;
}

std::string toString(gfan::ZMatrix const &m, char *tab=0)
{
  std::stringstream s;

  for(int i=0;i<m.getHeight();i++)
    {
      if(tab)s<<tab;
      for(int j=0;j<m.getWidth();j++)
	{
	  s<<m[i][j];
	  if(i+1!=m.getHeight() || j+1!=m.getWidth())
	    {
	      s<<",";
	    }
	}
      s<<std::endl;
    }
  return s.str();
}

std::string toString(gfan::ZCone const &c)
{
  std::stringstream s;
  gfan::ZMatrix i=c.getInequalities();
  gfan::ZMatrix e=c.getEquations();
  s<<"AMBIENT_DIM"<<std::endl;
  s<<c.ambientDimension()<<std::endl;
  s<<"INEQUALITIES"<<std::endl;
  s<<toString(i);
  s<<"EQUATIONS"<<std::endl;
  s<<toString(e);
  return s.str();
}

void *bbcone_Init(blackbox *b)
{
  return (void*)(new gfan::ZCone());
}

BOOLEAN bbcone_Assign(leftv l, leftv r)
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
  else if (r->Typ()==INT_CMD)
  {
    int ambientDim = (int)(long)r->Data();
    if (ambientDim < 0)
    {
      Werror("expected an int >= 0, but got %d", ambientDim);
      return TRUE;
    }
    if (l->Data()!=NULL)
    {   
      gfan::ZCone* zd = (gfan::ZCone*)l->Data();
      delete zd;
    }
    newZc = new gfan::ZCone(ambientDim);
  }
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

char * bbcone_String(blackbox *b, void *d)
{ if (d==NULL) return omStrDup("invalid object");
   else
   {
     std::string s=toString(*((gfan::ZCone*)d));
     char* ns = (char*) omAlloc(strlen(s.c_str()) + 10);
     sprintf(ns, "%s", s.c_str());
     omCheckAddr(ns);
     return ns;
   }
}

void bbcone_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    gfan::ZCone* zc = (gfan::ZCone*) d;
    delete zc;
  }
}

void * bbcone_Copy(blackbox*b, void *d)
{       
  gfan::ZCone* zc = (gfan::ZCone*)d;
  gfan::ZCone* newZc = new gfan::ZCone(*zc);
  return newZc;
}

static BOOLEAN jjCONERAYS1(leftv res, leftv v)
{
  /* method for generating a cone object from half-lines
     (cone = convex hull of the half-lines; note: there may be
     entire lines in the cone);
     valid parametrizations: (intmat) */
  intvec* rays = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN jjCONERAYS2(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from half-lines,
     and lines (any point in the cone being the sum of a point
     in the convex hull of the half-lines and a point in the span
     of the lines; the second argument may contain or entirely consist
     of zero rows);
     valid parametrizations: (intmat, intmat)
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns */
  intvec* rays = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* linSpace = (intvec *)v->CopyD(INTVEC_CMD);
  if (rays->cols() != linSpace->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           rays->cols(), linSpace->cols());
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(rays);
  gfan::ZMatrix zm2 = intmat2ZMatrix(linSpace);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm1, zm2);
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN jjCONERAYS3(leftv res, leftv u, leftv v, leftv w)
{
  /* method for generating a cone object from half-lines,
     and lines (any point in the cone being the sum of a point
     in the convex hull of the half-lines and a point in the span
     of the lines), and an integer k;
     valid parametrizations: (intmat, intmat, int);
     Errors will be invoked in the following cases:
     - u and v have different numbers of columns,
     - k not in [0..3];
     if the 2^0-bit of k is set, then the lineality space is known
     to be the span of the provided lines;
     if the 2^1-bit of k is set, then the extreme rays are known:
     each half-line spans a (different) extreme ray */
  intvec* rays = (intvec *)u->CopyD(INTVEC_CMD);
  intvec* linSpace = (intvec *)v->CopyD(INTVEC_CMD);
  if (rays->cols() != linSpace->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           rays->cols(), linSpace->cols());
    return TRUE;
  }
  int k = (int)(long)w->Data();
  if ((k < 0) || (k > 3))
  {
    WerrorS("expected int argument in [0..3]");
    return TRUE;
  }
  gfan::ZMatrix zm1 = intmat2ZMatrix(rays);
  gfan::ZMatrix zm2 = intmat2ZMatrix(linSpace);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(zm1, zm2);
  //k should be passed on to zc; not available yet
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN cone_via_rays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return jjCONERAYS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && (v->Typ() == INTMAT_CMD))
  {
    if (v->next == NULL) return jjCONERAYS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return jjCONERAYS3(res, u, v, w);
  }
  WerrorS("cone_via_rays: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
  intvec* inequs = (intvec *)v->CopyD(INTVEC_CMD);
  gfan::ZMatrix zm = intmat2ZMatrix(inequs);
  gfan::ZCone* zc = new gfan::ZCone(zm, gfan::ZMatrix(0, zm.getWidth()));
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN jjCONENORMALS2(leftv res, leftv u, leftv v)
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
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN jjCONENORMALS3(leftv res, leftv u, leftv v, leftv w)
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
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN cone_via_normals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
  {
    if (u->next == NULL) return jjCONENORMALS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && (v->Typ() == INTMAT_CMD))
  {
    if (v->next == NULL) return jjCONENORMALS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return jjCONENORMALS3(res, u, v, w);
  }
  WerrorS("cone_via_normals: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjGETPROPC(leftv res, leftv u, leftv v)
{
  /* method for retrieving cone properties;
     valid parametrizations: (cone, string),
     Errors will be invoked in the following cases:
     - invalid property string (see below for valid ones) */
  gfan::ZCone* zc = (gfan::ZCone*)u->Data();
  char* prop = (char*)v->Data();
  gfan::ZMatrix retMat;
  gfan::ZCone retCone;
  int retInt;
  gfan::ZVector retVec;
  int typeInfo;

  /* ################ properties with return type intmat: ################## */
  if      (strcmp(prop, "INEQUALITIES") == 0)
  {
    retMat = zc->getInequalities();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "EQUATIONS") == 0)
  {
    retMat = zc->getEquations();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "FACETS") == 0)
  {
    retMat = zc->getFacets();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "IMPLIED_EQUATIONS") == 0)
  {
    retMat = zc->getImpliedEquations();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "GENERATORS_OF_SPAN") == 0)
  {
    retMat = zc->generatorsOfSpan();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "GENERATORS_OF_LINEALITY_SPACE") == 0)
  {
    retMat = zc->generatorsOfLinealitySpace();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "RAYS") == 0)
  {
    retMat = zc->extremeRays();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "QUOTIENT_LATTICE_BASIS") == 0)
  {
    retMat = zc->quotientLatticeBasis();
    typeInfo = INTMAT_CMD;
  }
  else if (strcmp(prop, "LINEAR_FORMS") == 0)
  {
    retMat = zc->getLinearForms();
    typeInfo = INTMAT_CMD;
  }
  /* ################ properties with return type int: ################## */
  else if (strcmp(prop, "AMBIENT_DIM") == 0)
  {
    retInt = zc->ambientDimension();
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "DIM") == 0)
  {
    retInt = zc->dimension();
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "LINEALITY_DIM") == 0)
  {
    retInt = zc->dimensionOfLinealitySpace();
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "MULTIPLICITY") == 0)
  {
    bool ok = true;
    retInt = integerToInt(zc->getMultiplicity(), ok);
    if (!ok)
      WerrorS("overflow while converting a gfan::Integer to an int");
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "IS_ORIGIN") == 0)
  {
    retInt = zc->isOrigin() ? 1 : 0;
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "IS_FULL_SPACE") == 0)
  {
    retInt = zc->isFullSpace() ? 1 : 0;
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "SIMPLICIAL") == 0)
  {
    retInt = zc->isSimplicial() ? 1 : 0;
    typeInfo = INT_CMD;
  }
  else if (strcmp(prop, "CONTAINS_POSITIVE_VECTOR") == 0)
  {
    retInt = zc->containsPositiveVector() ? 1 : 0;
    typeInfo = INT_CMD;
  }
  /* ################ properties with return type ZCone: ################## */
  else if (strcmp(prop, "LINEALITY_SPACE") == 0)
  {
    retCone = zc->linealitySpace();
    typeInfo = coneID;
  }
  else if (strcmp(prop, "DUAL_CONE") == 0)
  {
    retCone = zc->dualCone();
    typeInfo = coneID;
  }
  else if (strcmp(prop, "NEGATED") == 0)
  {
    retCone = zc->negated();
    typeInfo = coneID;
  }
  /* ################ properties with return type intvec: ################## */
  else if (strcmp(prop, "SEMI_GROUP_GENERATOR") == 0)
  {
    /* test whether the cone's dim = dim of lin space + 1: */
    int d = zc->dimension();
    int dLS = zc->dimensionOfLinealitySpace();
    if (d == dLS + 1)
      retVec = zc->semiGroupGeneratorOfRay();
    else
    {
      Werror("expected dim of cone one larger than dim of lin space\n"
             "but got dimensions %d and %d", d, dLS);
    }
    typeInfo = INTVEC_CMD;
  }
  else if (strcmp(prop, "RELATIVE_INTERIOR_POINT") == 0)
  {
    retVec = zc->getRelativeInteriorPoint();
    typeInfo = INTVEC_CMD;
  }
  else if (strcmp(prop, "UNIQUE_POINT") == 0)
  {
    retVec = zc->getUniquePoint();
    typeInfo = INTVEC_CMD;
  }
  else
  {
    Werror("unexpected cone property '%s'", prop);
    return TRUE;
  }

  res->rtyp = typeInfo;
  if (typeInfo == INT_CMD)
    res->data = (void*)retInt;
  else if (typeInfo == INTMAT_CMD)
    res->data = (void*)zMatrix2Intvec(retMat);
  else if (typeInfo == coneID)
    res->data = (void*)new gfan::ZCone(retCone);
  else if (typeInfo == INTVEC_CMD)
    res->data = (void*)zVector2Intvec(retVec);
  else
  {
    WerrorS("implementation error in bbcone.cc::jjGETPROPC");
    return TRUE;
  }
  return FALSE;
}

BOOLEAN getprop(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == STRING_CMD))
    {
      if (v->next == NULL) return jjGETPROPC(res, u, v);
    }
  }
  WerrorS("getprop: unexpected parameters");
  return TRUE;
}

BOOLEAN cone_intersect(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
        Werror("expected ambient dims of both cones to coincide\n"
               "but got %d and %d", d1, d2);
      gfan::ZCone zc3 = gfan::intersection(*zc1, *zc2);
      res->rtyp = coneID;
      res->data = (void *)new gfan::ZCone(zc3);
      return FALSE;
    }
  }
  WerrorS("cone_intersect: unexpected parameters");
  return TRUE;
}

BOOLEAN cone_link(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      intvec* iv = (intvec*)v->Data();
      gfan::ZVector zv= intvec2ZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv.size();
      if (d1 != d2)
        Werror("expected ambient dim of cone and size of vector\n"
               "to be equal but got %d and %d", d1, d2);
      if(!zc->contains(zv))
      {
        WerrorS("the provided intvec does not lie in the cone");
      }
      res->rtyp = coneID;
      res->data = (void *)new gfan::ZCone(zc->link(zv));
      return FALSE;
    }
  }
  WerrorS("cone_link: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjCONTAINS2(leftv res, leftv u, leftv v)
{
  gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
  gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
  int d1 = zc1->ambientDimension();
  int d2 = zc2->ambientDimension();
  if (d1 != d2)
    Werror("expected cones with same ambient dimensions\n but got"
           " dimensions %d and %d", d1, d2);
  res->rtyp = INT_CMD;
  res->data = (void *)(zc1->contains(*zc2) ? 1 : 0);
  return FALSE;
}

static BOOLEAN jjCONTAINS3(leftv res, leftv u, leftv v, leftv w)
{
  gfan::ZCone* zc = (gfan::ZCone*)u->Data();
  intvec* vec = (intvec*)v->Data();
  int flag = (int)(long)w->Data();
  gfan::ZVector zv = intvec2ZVector(vec);
  int d1 = zc->ambientDimension();
  int d2 = zv.size();
  if (d1 != d2)
    Werror("expected ambient dim of cone and size of vector\n"
           "to be equal but got %d and %d", d1, d2);
  res->rtyp = INT_CMD;
  if (flag)
    res->data = (void *)(zc->containsRelatively(zv) ? 1 : 0);
  else
    res->data = (void *)(zc->contains(zv) ? 1 : 0);;
  return FALSE;
}

BOOLEAN contains(leftv res, leftv args)
{

  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID)) return jjCONTAINS2(res, u, v); 
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD)) return jjCONTAINS3(res, u, v, w);
    }
  }
  WerrorS("contains: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjSETPROPC3A(leftv res, leftv u, leftv v, leftv w)
{
  gfan::ZCone* zc = (gfan::ZCone*)u->Data();
  char* prop = (char*)v->Data();
  int val = (int)(long)w->Data();

  if (strcmp(prop, "MULTIPLICITY") == 0)
  {
    zc->setMultiplicity(gfan::Integer(val));
  }
  else
  {
    Werror("unexpected cone property '%s'", prop);
    return TRUE;
  }
  res->rtyp = NONE;
  res->data = NULL;
  return FALSE;
}
static BOOLEAN jjSETPROPC3B(leftv res, leftv u, leftv v, leftv w)
{
  gfan::ZCone* zc = (gfan::ZCone*)u->Data();
  char* prop = (char*)v->Data();
  intvec* mat = (intvec*)w->Data();
  gfan::ZMatrix zm = intmat2ZMatrix(mat);

  if (strcmp(prop, "LINEAR_FORMS") == 0)
  {
    zc->setLinearForms(zm);
  }
  else
  {
    Werror("unexpected cone property '%s'", prop);
    return TRUE;
  }
  res->rtyp = NONE;
  res->data = NULL;
  return FALSE;
}

BOOLEAN setprop(leftv res, leftv args)
{

  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == STRING_CMD))
    {
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD)) return jjSETPROPC3A(res, u, v, w);
      if ((w != NULL) && (w->Typ() == INTMAT_CMD)) return jjSETPROPC3B(res, u, v, w);
    } 
  }
  WerrorS("setprop: unexpected parameters");
  return TRUE;
}

void bbcone_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefule,
  // all other are simply error messages
  b->blackbox_destroy=bbcone_destroy;
  b->blackbox_String=bbcone_String;
  //b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=bbcone_Init;
  b->blackbox_Copy=bbcone_Copy;
  b->blackbox_Assign=bbcone_Assign;
  iiAddCproc("","cone_via_rays",FALSE,cone_via_rays);
  iiAddCproc("","cone_via_normals",FALSE,cone_via_normals);
  iiAddCproc("","getprop",FALSE,getprop);
  iiAddCproc("","cone_intersect",FALSE,cone_intersect);
  iiAddCproc("","cone_link",FALSE,cone_link);
  iiAddCproc("","contains",FALSE,contains);
  iiAddCproc("","setprop",FALSE,setprop);
  coneID=setBlackboxStuff(b,"cone");
  Print("created type %d (cone)\n",coneID); 
}

#endif
/* HAVE_FANS */
