#include <kernel/mod2.h>

#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_q.h>

#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/blackbox.h>
#include <misc/intvec.h>
#include <misc/sirandom.h>
#include <coeffs/longrat.h>
#include <coeffs/bigintmat.h>

#include <bbfan.h>
#include <bbpolytope.h>
#include <sstream>

#include "Singular/ipid.h"

// #include <omalloc/omalloc.h>
// #include <kernel/intvec.h>
// #include <kernel/longrat.h>
// #include <Singular/lists.h>
// #include <Singular/subexpr.h>

int coneID;

number integerToNumber(const gfan::Integer &I)
{
  mpz_t i;
  mpz_init(i);
  I.setGmp(i);
  long m = 268435456;
  if(mpz_cmp_si(i,m))
  {
    int temp = (int) mpz_get_si(i);
    return n_Init(temp,coeffs_BIGINT);
  }
  else
    return n_InitMPZ(i,coeffs_BIGINT);
}

bigintmat* zVectorToBigintmat(const gfan::ZVector &zv)
{
  int d=zv.size();
  bigintmat* bim = new bigintmat(1,d,coeffs_BIGINT);
  for(int i=1;i<=d;i++)
  {
    number temp = integerToNumber(zv[i-1]);
    bim->set(1,i,temp);
    n_Delete(&temp,coeffs_BIGINT);
  }
  return bim;
}

bigintmat* zMatrixToBigintmat(const gfan::ZMatrix &zm)
{
  int d=zm.getHeight();
  int n=zm.getWidth();
  bigintmat* bim = new bigintmat(d,n,coeffs_BIGINT);
  for(int i=1;i<=d;i++)
    for(int j=1; j<=n; j++)
    {
      number temp = integerToNumber(zm[i-1][j-1]);
      bim->set(i,j,temp);
      n_Delete(&temp,coeffs_BIGINT);
    }
  return bim;
}

gfan::Integer* numberToInteger(const number &n)
{
  if (SR_HDL(n) & SR_INT)
    return new gfan::Integer(SR_TO_INT(n));
  else
    return new gfan::Integer(n->z);
}

gfan::ZMatrix* bigintmatToZMatrix(const bigintmat &bim)
{
  int d=bim.rows();
  int n=bim.cols();
  gfan::ZMatrix* zm = new gfan::ZMatrix(d,n);
  for(int i=0;i<d;i++)
    for(int j=0;j<n;j++)
    {
      number temp = BIMATELEM(bim, i+1, j+1);
      gfan::Integer* gi = numberToInteger(temp);
      (*zm)[i][j] = *gi;
      delete gi;
    }
  return zm;
}

gfan::ZVector* bigintmatToZVector(const bigintmat &bim)
{
  gfan::ZVector* zv=new gfan::ZVector(bim.cols());
  for(int j=0; j<bim.cols(); j++)
  {
    number temp = BIMATELEM(bim, 1, j+1);
    gfan::Integer* gi = numberToInteger(temp);
    (*zv)[j] = *gi;
    n_Delete(&temp,coeffs_BIGINT);
    delete gi;
  }
  return zv;
}

char* toString(gfan::ZMatrix const &zm)
{
  bigintmat* bim = zMatrixToBigintmat(zm);
  char* s = bim->StringAsPrinted();
  if (s==NULL)
    s = (char*) omAlloc0(sizeof(char));
  delete bim;
  return s;
}

std::string toString(const gfan::ZCone* const c)
{
  std::stringstream s;
  s<<"AMBIENT_DIM"<<std::endl;
  s<<c->ambientDimension()<<std::endl;

  gfan::ZMatrix i=c->getInequalities();
  char* ineqs = toString(i);
  if (c->areFacetsKnown())
    s<<"FACETS"<<std::endl;
  else
    s<<"INEQUALITIES"<<std::endl;
  if (ineqs!=NULL)
  {
    s<<ineqs<<std::endl;
    omFree(ineqs);
  }

  gfan::ZMatrix e=c->getEquations();
  char* eqs = toString(e);
  if (c->areImpliedEquationsKnown())
    s<<"LINEAR_SPAN"<<std::endl;
  else
    s<<"EQUATIONS"<<std::endl;
  if (eqs!=NULL)
  {
    s<<eqs<<std::endl;
    omFree(eqs);
  }

  if (c->areExtremeRaysKnown())
  {
    gfan::ZMatrix r=c->extremeRays();
    char* rs = toString(r);
    s<<"RAYS"<<std::endl;
    if (rs!=NULL)
    {
      s<<rs<<std::endl;
      omFree(rs);
    }
    gfan::ZMatrix l=c->generatorsOfLinealitySpace();
    char* ls = toString(l);
    s<<"LINEALITY_SPACE"<<std::endl;
    if (ls!=NULL)
    {
      s<<ls<<std::endl;
      omFree(ls);
    }
  }

  return s.str();
}

void* bbcone_Init(blackbox* /*b*/)
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
    IDDATA((idhdl)l->data)=(char*) newZc;
  }
  else
  {
    l->data=(void *)newZc;
  }
  return FALSE;
}

char* bbcone_String(blackbox* /*b*/, void *d)
{
  if (d==NULL) return omStrDup("invalid object");
  else
  {
    std::string s=toString((gfan::ZCone*) d);
    return omStrDup(s.c_str());
  }
}

void bbcone_destroy(blackbox* /*b*/, void *d)
{
  if (d!=NULL)
  {
    gfan::ZCone* zc = (gfan::ZCone*) d;
    delete zc;
  }
}

void* bbcone_Copy(blackbox* /*b*/, void *d)
{
  gfan::ZCone* zc = (gfan::ZCone*)d;
  gfan::ZCone* newZc = new gfan::ZCone(*zc);
  return newZc;
}

static BOOLEAN bbcone_Op2(int op, leftv res, leftv i1, leftv i2)
{
  gfan::ZCone* zp = (gfan::ZCone*) i1->Data();
  switch(op)
  {
    case '&':
    {
      if (i2->Typ()==coneID)
      {
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
          WerrorS("mismatching ambient dimensions");
          return TRUE;
        }
        gfan::ZCone* zs = new gfan::ZCone();
        *zs = gfan::intersection(*zp, *zq);
        zs->canonicalize();
        res->rtyp = coneID;
        res->data = (void*) zs;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    case '|':
    {
      if(i2->Typ()==coneID)
      {
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        int d1 = zp->ambientDimension();
        int d2 = zq->ambientDimension();
        if (d1 != d2)
        {
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
        res->rtyp = coneID;
        res->data = (void*) zs;
        return FALSE;
      }
    return blackboxDefaultOp2(op,res,i1,i2);
    }
    case EQUAL_EQUAL:
    {
      if(i2->Typ()==coneID)
      {
        gfan::ZCone* zq = (gfan::ZCone*) i2->Data();
        zp->canonicalize();
        zq->canonicalize();
        bool b = !((*zp)!=(*zq));
        res->rtyp = INT_CMD;
        res->data = (void*) b;
        return FALSE;
      }
      return blackboxDefaultOp2(op,res,i1,i2);
    }
    default:
      return blackboxDefaultOp2(op,res,i1,i2);
  }
  return blackboxDefaultOp2(op,res,i1,i2);
}


/* singular wrapper for gfanlib functions */
static BOOLEAN jjCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
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
  res->rtyp = coneID;
  res->data = (void*) zc;
  return FALSE;
}

static BOOLEAN jjCONENORMALS2(leftv res, leftv u, leftv v)
{
  /* method for generating a cone object from iequalities,
     and equations (...)
     valid parametrizations: (intmat, intmat)
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
  res->rtyp = coneID;
  res->data = (void*) zc;
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
  res->rtyp = coneID;
  res->data = (void*) zc;
  return FALSE;
}

BOOLEAN coneViaNormals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL) return jjCONENORMALS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
  {
    if (v->next == NULL) return jjCONENORMALS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return jjCONENORMALS3(res, u, v, w);
  }
  WerrorS("coneViaInequalities: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjCONERAYS1(leftv res, leftv v)
{
  /* method for generating a cone object from half-lines
     (cone = convex hull of the half-lines; note: there may be
     entire lines in the cone);
     valid parametrizations: (intmat) */
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
  res->rtyp = coneID;
  res->data = (void*) zc;

  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete rays;
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
  bigintmat* rays = NULL; bigintmat* linSpace = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* rays0 = (intvec*) u->Data();
    rays = iv2bim(rays0,coeffs_BIGINT);
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0,coeffs_BIGINT);
  }
  else
    linSpace = (bigintmat*) v->Data();

  if (rays->cols() != linSpace->cols())
  {
    Werror("expected same number of columns but got %d vs. %d",
           rays->cols(), linSpace->cols());
    return TRUE;
  }
  gfan::ZMatrix* zm1 = bigintmatToZMatrix(rays);
  gfan::ZMatrix* zm2 = bigintmatToZMatrix(linSpace);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm1, *zm2);
  res->rtyp = coneID;
  res->data = (void*) zc;

  delete zm1;
  delete zm2;
  if (u->Typ() == INTMAT_CMD)
    delete rays;
  if (v->Typ() == INTMAT_CMD)
    delete linSpace;
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
  bigintmat* rays = NULL; bigintmat* linSpace = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* rays0 = (intvec*) u->Data();
    rays = iv2bim(rays0,coeffs_BIGINT);
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0,coeffs_BIGINT);
  }
  else
    linSpace = (bigintmat*) v->Data();

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
  gfan::ZMatrix* zm1 = bigintmatToZMatrix(rays);
  gfan::ZMatrix* zm2 = bigintmatToZMatrix(linSpace);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm1, *zm2);
  //k should be passed on to zc; not available yet
  res->rtyp = coneID;
  res->data = (void*) zc;

  delete zm1;
  delete zm2;
  if (u->Typ() == INTMAT_CMD)
    delete rays;
  if (v->Typ() == INTMAT_CMD)
    delete linSpace;
  return FALSE;
}

BOOLEAN coneViaRays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL) return jjCONERAYS1(res, u);
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
    {
      if (v->next == NULL) return jjCONERAYS2(res, u, v);
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        if (w->next == NULL) return jjCONERAYS3(res, u, v, w);
      }
    }
  }
  WerrorS("coneViaPoints: unexpected parameters");
  return TRUE;
}

BOOLEAN inequalities(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();

    gfan::ZMatrix zmat = zc->getInequalities();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("inequalities: unexpected parameters");
  return TRUE;
}

BOOLEAN equations(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("equations: unexpected parameters");
  return TRUE;
}

BOOLEAN facets(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zm = zc->getFacets();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zm);
    return FALSE;
  }
  WerrorS("facets: unexpected parameters");
  return TRUE;
}

BOOLEAN impliedEquations(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getImpliedEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("span: unexpected parameters");
  return TRUE;
}

BOOLEAN generatorsOfSpan(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfSpan();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("generatorsOfSpan: unexpected parameters");
  return TRUE;
}

BOOLEAN generatorsOfLinealitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID || u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfLinealitySpace();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("generatorsOfLinealitySpace: unexpected parameters");
  return TRUE;
}

BOOLEAN rays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zm = zc->extremeRays();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zm);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    gfan::ZMatrix zmat = rays(zf);
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("rays: unexpected parameters");
  return TRUE;
}

BOOLEAN quotientLatticeBasis(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->quotientLatticeBasis();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("quotientLatticeBasis: unexpected parameters");
  return TRUE;
}

BOOLEAN getLinearForms(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getLinearForms();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("getLinearForms: unexpected parameters");
  return TRUE;
}

BOOLEAN ambientDimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->ambientDimension();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getAmbientDimension(zf);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getAmbientDimension(zc);
    return FALSE;
  }
  WerrorS("ambientDimension: unexpected parameters");
  return TRUE;
}

BOOLEAN dimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->dimension();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getDimension(zf);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getDimension(zc);
    return FALSE;
  }
  WerrorS("dimension: unexpected parameters");
  return TRUE;
}

BOOLEAN codimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      res->rtyp = INT_CMD;
      res->data = (void*) (long) zc->codimension();
      return FALSE;
    }
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      res->rtyp = INT_CMD;
      res->data = (void*) (long) getCodimension(zf);
      return FALSE;
    }
  if ((u != NULL) && (u->Typ() == polytopeID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      res->rtyp = INT_CMD;
      res->data = (void*) (long) getCodimension(zc);
      return FALSE;
    }
  WerrorS("getCodimension: unexpected parameters");
  return TRUE;
}

BOOLEAN linealityDimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->dimensionOfLinealitySpace();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getLinealityDimension(zf);
    return FALSE;
  }
  WerrorS("linealityDimension: unexpected parameters");
  return TRUE;
}

BOOLEAN getMultiplicity(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    number i = integerToNumber(zc->getMultiplicity());
    res->rtyp = BIGINT_CMD;
    res->data = (void*) i;
    return FALSE;
  }
  WerrorS("getMultiplicity: unexpected parameters");
  return TRUE;
}

BOOLEAN isOrigin(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->isOrigin();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    return FALSE;
  }
  WerrorS("isOrigin: unexpected parameters");
  return TRUE;
}

BOOLEAN isFullSpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->isFullSpace();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    return FALSE;
  }
  WerrorS("isFullSpace: unexpected parameters");
  return TRUE;
}

BOOLEAN isSimplicial(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    int b = zc->isSimplicial();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    bool b = isSimplicial(zf);
    res->rtyp = INT_CMD;
    res->data = (void*) (long) b;
    return FALSE;
  }
  WerrorS("isSimplicial: unexpected parameters");
  return TRUE;
}

BOOLEAN containsPositiveVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->containsPositiveVector();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    return FALSE;
  }
  WerrorS("containsPositiveVector: unexpected parameters");
  return TRUE;
}

BOOLEAN linealitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->linealitySpace());
    res->rtyp = coneID;
    res->data = (void*) zd;
    return FALSE;
  }
  WerrorS("linealitySpace: unexpected parameters");
  return TRUE;
}

BOOLEAN dualCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->dualCone());
    res->rtyp = coneID;
    res->data = (void*) zd;
    return FALSE;
  }
  WerrorS("dual: unexpected parameters");
  return TRUE;
}

BOOLEAN negatedCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->negated());
    res->rtyp = coneID;
    res->data = (void*) zd;
    return FALSE;
  }
  WerrorS("negatedCone: unexpected parameters");
  return TRUE;
}

BOOLEAN semigroupGenerator(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int d = zc->dimension();
    int dLS = zc->dimensionOfLinealitySpace();
    if (d == dLS + 1)
    {
      gfan::ZVector zv = zc->semiGroupGeneratorOfRay();
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(zv);
      return FALSE;
    }
    Werror("expected dim of cone one larger than dim of lin space\n"
            "but got dimensions %d and %d", d, dLS);
  }
  WerrorS("semigroupGenerator: unexpected parameters");
  return TRUE;
}

BOOLEAN relativeInteriorPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = zc->getRelativeInteriorPoint();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    return FALSE;
  }
  WerrorS("relativeInteriorPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN uniquePoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = zc->getUniquePoint();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    return FALSE;
  }
  WerrorS("uniquePoint: unexpected parameters");
  return TRUE;
}

gfan::ZVector randomPoint(const gfan::ZCone* zc)
{
  gfan::ZMatrix rays = zc->extremeRays();
  gfan::ZVector rp = gfan::ZVector(zc->ambientDimension());
  for (int i=0; i<rays.getHeight(); i++)
  {
    int n = siRand();
    rp = rp + n * rays[i];
  }
  return rp;
}

BOOLEAN randomPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = randomPoint(zc);
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    return FALSE;
  }
  WerrorS("randomPoint: unexpected parameters");
  return TRUE;
}

BOOLEAN setMultiplicity(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      int val = (int)(long)v->Data();
      zc->setMultiplicity(gfan::Integer(val));
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }
  }
  WerrorS("setMultiplicity: unexpected parameters");
  return TRUE;
}

BOOLEAN setLinearForms(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      bigintmat* mat=NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* mat0 = (intvec*) v->Data();
        mat = iv2bim(mat0,coeffs_BIGINT)->transpose();
      }
      else
        mat = (bigintmat*) v->Data();
      gfan::ZMatrix* zm = bigintmatToZMatrix(mat);
      zc->setLinearForms(*zm);
      res->rtyp = NONE;
      res->data = NULL;

      delete zm;
      if (v->Typ() == INTVEC_CMD)
        delete mat;
     return FALSE;
    }
  }
  WerrorS("setLinearForms: unexpected parameters");
  return TRUE;
}

gfan::ZMatrix liftUp(const gfan::ZMatrix &zm)
{
  int r=zm.getHeight();
  int c=zm.getWidth();
  gfan::ZMatrix zn(r+1,c+1);
  zn[1][1]=1;
  for (int i=0; i<r; i++)
    for (int j=0; j<c; j++)
      zn[i+1][j+1]=zm[i][j];
  return zn;
}

gfan::ZCone liftUp(const gfan::ZCone &zc)
{
  gfan::ZMatrix ineq=zc.getInequalities();
  gfan::ZMatrix eq=zc.getEquations();
  gfan::ZCone zd(liftUp(ineq),liftUp(eq));
  return zd;
}

BOOLEAN coneToPolytope(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    gfan::ZMatrix ineq=zc->getInequalities();
    gfan::ZMatrix eq=zc->getEquations();
    gfan::ZCone* zd = new gfan::ZCone(liftUp(ineq),liftUp(eq));
    res->rtyp = polytopeID;
    res->data = (void*) zd;
    return FALSE;
  }
  WerrorS("makePolytope: unexpected parameters");
  return TRUE;
}

BOOLEAN intersectCones(leftv res, leftv args)
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
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = coneID;
      res->data = (void *)new gfan::ZCone(zc3);
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zc11 = (gfan::ZCone*)u->Data();
      gfan::ZCone zc1 = liftUp(*zc11);
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1.ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc22 = (gfan::ZCone*)v->Data();
      gfan::ZCone zc2 = liftUp(*zc22);
      int d1 = zc1->ambientDimension();
      int d2 = zc2.ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      return FALSE;
    }
  }
  WerrorS("convexIntersection: unexpected parameters");
  return TRUE;
}

BOOLEAN convexHull(leftv res, leftv args)
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
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZMatrix zm1 = zc1->extremeRays();
      gfan::ZMatrix zm2 = zc2->extremeRays();
      gfan::ZMatrix zn1 = zc1->generatorsOfLinealitySpace();
      gfan::ZMatrix zn2 = zc2->generatorsOfLinealitySpace();
      gfan::ZMatrix zm = combineOnTop(zm1,zm2);
      gfan::ZMatrix zn = combineOnTop(zn1,zn2);
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm, zn);
      res->rtyp = coneID;
      res->data = (void*) zc;
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zc11 = (gfan::ZCone*)u->Data();
      gfan::ZCone zc1 = liftUp(*zc11);
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1.ambientDimension()-1;
      int d2 = zc2->ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZMatrix zm1 = zc1.extremeRays();
      gfan::ZMatrix zm2 = zc2->extremeRays();
      gfan::ZMatrix zn = zc1.generatorsOfLinealitySpace();
      gfan::ZMatrix zm = combineOnTop(zm1,zm2);
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm, zn);
      res->rtyp = polytopeID;
      res->data = (void*) zc;
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc22 = (gfan::ZCone*)v->Data();
      gfan::ZCone zc2 = liftUp(*zc22);
      int d1 = zc1->ambientDimension()-1;
      int d2 = zc2.ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZMatrix zm1 = zc1->extremeRays();
      gfan::ZMatrix zm2 = zc2.extremeRays();
      gfan::ZMatrix zn = zc2.generatorsOfLinealitySpace();
      gfan::ZMatrix zm = combineOnTop(zm1,zm2);
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = polytopeID;
      res->data = (void*) zc;
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension()-1;
      int d2 = zc2->ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        return TRUE;
      }
      gfan::ZMatrix zm1 = zc1->extremeRays();
      gfan::ZMatrix zm2 = zc2->extremeRays();
      gfan::ZMatrix zm = combineOnTop(zm1,zm2);
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = polytopeID;
      res->data = (void*) zc;
      return FALSE;
    }
  }
  WerrorS("convexHull: unexpected parameters");
  return TRUE;
}

BOOLEAN coneLink(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      bigintmat* iv = NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* iv0 = (intvec*) v->Data();
        iv = iv2bim(iv0,coeffs_BIGINT)->transpose();
      }
      else
        iv = (bigintmat*)v->Data();
      gfan::ZVector* zv = bigintmatToZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv->size();
      if (d1 != d2)
      {
        Werror("expected ambient dim of cone and size of vector\n"
               " to be equal but got %d and %d", d1, d2);
        return TRUE;
      }
      if(!zc->contains(*zv))
      {
        WerrorS("the provided intvec does not lie in the cone");
        return TRUE;
      }
      gfan::ZCone* zd = new gfan::ZCone(zc->link(*zv));
      res->rtyp = coneID;
      res->data = (void *) zd;

      delete zv;
      if (v->Typ() == INTMAT_CMD)
        delete iv;
      return FALSE;
    }
  }
  WerrorS("coneLink: unexpected parameters");
  return TRUE;
}

BOOLEAN containsInSupport(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      int d1 = zc->ambientDimension();
      int d2 = zd->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected cones with same ambient dimensions\n but got"
               " dimensions %d and %d", d1, d2);
        return TRUE;
      }
      bool b = (zc->contains(*zd) ? 1 : 0);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      return FALSE;
    }
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      bigintmat* iv = NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* iv0 = (intvec*) v->Data();
        iv = iv2bim(iv0,coeffs_BIGINT)->transpose();
      }
      else
        iv = (bigintmat*)v->Data();

      gfan::ZVector* zv = bigintmatToZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv->size();
      if (d1 != d2)
      {
        Werror("expected cones with same ambient dimensions\n but got"
               " dimensions %d and %d", d1, d2);
        return TRUE;
      }
      int b = zc->contains(*zv);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;

      delete zv;
      if (v->Typ() == INTMAT_CMD)
        delete iv;
      return FALSE;
    }
  }
  WerrorS("containsInSupport: unexpected parameters");
  return TRUE;
}

BOOLEAN containsRelatively(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      bigintmat* iv = NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* iv0 = (intvec*) v->Data();
        iv = iv2bim(iv0,coeffs_BIGINT)->transpose();
      }
      else
        iv = (bigintmat*)v->Data();
      gfan::ZVector* zv = bigintmatToZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv->size();
      if (d1 == d2)
      {
        bool b = (zc->containsRelatively(*zv) ? 1 : 0);
        res->rtyp = INT_CMD;
        res->data = (void *) b;
        delete zv;
        if (v->Typ() == INTMAT_CMD)
          delete iv;
        return FALSE;
      }
      delete zv;
      if (v->Typ() == INTMAT_CMD)
        delete iv;
      Werror("expected ambient dim of cone and size of vector\n"
             "to be equal but got %d and %d", d1, d2);
    }
  }
  WerrorS("containsRelatively: unexpected parameters");
  return TRUE;
}

BOOLEAN hasFace(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      bool b = zc->hasFace(*zd);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      bool b = zc->hasFace(*zd);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      return FALSE;
    }
  }
  WerrorS("containsAsFace: unexpected parameters");
  return TRUE;
}

BOOLEAN canonicalizeCone(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(*zc);
    zd->canonicalize();
    res->rtyp = coneID;
    res->data = (void*) zd;
    return FALSE;
  }
  WerrorS("canonicalizeCone: unexpected parameters");
  return TRUE;
}

BOOLEAN containsCone(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      lists l = (lists) u->Data();
      gfan::ZCone* zc = (gfan::ZCone*) v->Data();
      zc->canonicalize();
      int b = 0;
      for (int i=0; i<=lSize(l); i++)
      {
        if (l->m[i].Typ() != coneID)
        {
          WerrorS("containsCone: entries of wrong type in list");
          return TRUE;
        }
        gfan::ZCone* ll = (gfan::ZCone*) l->m[i].Data();
        ll->canonicalize();
        if (!((*ll) != (*zc)))
        {
          b = 1;
          break;
        }
      }
      res->rtyp = INT_CMD;
      res->data = (char*) (long) b;
      return FALSE;
    }
  }
  WerrorS("containsCone: unexpected parameters");
  return TRUE;
}

gfan::ZVector intStar2ZVector(const int d, const int* i)
{
  gfan::ZVector zv(d);
  for(int j=0; j<d; j++)
    zv[j]=i[j+1];
  return zv;
}

BOOLEAN maximalGroebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g = NULL;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        while (g != NULL)
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          inequalities.appendRow(leadexpw-tailexpw);
          pIter(g);
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("maximalGroebnerCone: unexpected parameters");
  return TRUE;
}


lists listOfFacets(const gfan::ZCone &zc)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  lists L = (lists)omAllocBin(slists_bin);
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();
  L->Init(r);

  /* next we iterate over each of the r facets, build the respective cone and add it to the list */
  /* this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  L->m[0].rtyp = coneID; L->m[0].data=(void*) new gfan::ZCone(newInequalities,newEquations);

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i-1,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    L->m[i].rtyp = coneID; L->m[i].data=(void*) new gfan::ZCone(newInequalities,newEquations);
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r]);
  L->m[r-1].rtyp = coneID; L->m[r-1].data=(void*) new gfan::ZCone(newInequalities,newEquations);

  return L;
}


BOOLEAN listOfFacets(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    lists L = listOfFacets(*zc);
    res->rtyp = LIST_CMD;
    res->data = (void*) L;
    return FALSE;
  }
  WerrorS("listOfFacets: unexpected parameters");
  return TRUE;
}


poly initial(poly p)
{
  poly g = p;
  poly h = p_Head(g,currRing);
  poly f = h;
  long d = p_Deg(g,currRing);
  pIter(g);
  while ((g != NULL) && (p_Deg(g,currRing) == d))
  {
    pNext(h) = p_Head(g,currRing);
    pIter(h);
    pIter(g);
  }
  return(f);
}


BOOLEAN initial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      poly p = (poly) u->Data();
      res->rtyp = POLY_CMD;
      res->data = (void*) initial(p);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      ideal inI = idInit(IDELEMS(I));
      poly g;
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i];
        inI->m[i]=initial(g);
      }
      res->rtyp = IDEAL_CMD;
      res->data = (void*) inI;
      return FALSE;
    }
  }
  WerrorS("initial: unexpected parameters");
  return TRUE;
}


BOOLEAN homogeneitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        while (g != NULL)
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(gfan::ZMatrix(0, equations.getWidth()),equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("homogeneitySpace: unexpected parameters");
  return TRUE;
}


BOOLEAN groebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g = NULL;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      long d;
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        d = p_Deg(g,currRing);
        while ((g != NULL) && (p_Deg(g,currRing) == d))
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }

        if (g != NULL)
        {
          while (g != NULL)
          {
            pGetExpV(g,tailexpv);
            tailexpw = intStar2ZVector(n, tailexpv);
            inequalities.appendRow(leadexpw-tailexpw);
            pIter(g);
          }
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(inequalities,equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("groebnerCone: unexpected parameters");
  return TRUE;
}

/***
 * Given a cone and a point in its boundary,
 * returns the inner normal vector of a facet
 * containing the point.
 * In case no facet contains the point,
 * then 0 is returned.
 **/
gfan::ZVector* facetContaining(gfan::ZCone* zc, gfan::ZVector* zv)
{
  gfan::ZMatrix facets = zc->getFacets();
  for (int i=0; i<facets.getHeight(); i++)
  {
    gfan::ZVector facet = facets[i];
    if (dot(facet,*zv) == (long) 0)
      return new gfan::ZVector(facet);
  }
  return new gfan::ZVector(zc->ambientDimension());
}


BOOLEAN facetContaining(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::ZCone* zc = (gfan::ZCone*) u->Data();

      bigintmat* point1;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* point0 = (intvec*) v->Data();
        point1 = iv2bim(point0,coeffs_BIGINT)->transpose();
      }
      else
        point1 = (bigintmat*) v->Data();

      gfan::ZVector* point = bigintmatToZVector(*point1);
      gfan::ZVector* facet = facetContaining(zc, point);

      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(*facet);

      delete facet;
      delete point;
      if (v->Typ() == INTVEC_CMD)
        delete point1;
      return FALSE;
    }
  }
  WerrorS("facetContaining: unexpected parameters");
  return TRUE;
}


void bbcone_setup(SModulFunctions* p)
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefull,
  // all other are simply error messages
  b->blackbox_destroy=bbcone_destroy;
  b->blackbox_String=bbcone_String;
  // b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=bbcone_Init;
  b->blackbox_Copy=bbcone_Copy;
  b->blackbox_Assign=bbcone_Assign;
  b->blackbox_Op2=bbcone_Op2;
  p->iiAddCproc("","coneViaInequalities",FALSE,coneViaNormals);
  p->iiAddCproc("","coneViaPoints",FALSE,coneViaRays);

  // iiAddCproc("","makePolytope",FALSE,coneToPolytope);
  p->iiAddCproc("","ambientDimension",FALSE,ambientDimension);
  p->iiAddCproc("","canonicalizeCone",FALSE,canonicalizeCone);
  p->iiAddCproc("","codimension",FALSE,codimension);
  p->iiAddCproc("","coneLink",FALSE,coneLink);
  p->iiAddCproc("","containsAsFace",FALSE,hasFace);
  p->iiAddCproc("","containsInSupport",FALSE,containsInSupport);
  p->iiAddCproc("","containsPositiveVector",FALSE,containsPositiveVector);
  p->iiAddCproc("","containsRelatively",FALSE,containsRelatively);
  p->iiAddCproc("","convexHull",FALSE,convexHull);
  p->iiAddCproc("","convexIntersection",FALSE,intersectCones);
  p->iiAddCproc("","dimension",FALSE,dimension);
  p->iiAddCproc("","dualCone",FALSE,dualCone);
  p->iiAddCproc("","equations",FALSE,equations);
  p->iiAddCproc("","facets",FALSE,facets);
  p->iiAddCproc("","generatorsOfLinealitySpace",FALSE,generatorsOfLinealitySpace);
  p->iiAddCproc("","generatorsOfSpan",FALSE,generatorsOfSpan);
  p->iiAddCproc("","getLinearForms",FALSE,getLinearForms);
  p->iiAddCproc("","getMultiplicity",FALSE,getMultiplicity);
  p->iiAddCproc("","inequalities",FALSE,inequalities);
  p->iiAddCproc("","isFullSpace",FALSE,isFullSpace);
  p->iiAddCproc("","isOrigin",FALSE,isOrigin);
  p->iiAddCproc("","isSimplicial",FALSE,isSimplicial);
  p->iiAddCproc("","linealityDimension",FALSE,linealityDimension);
  p->iiAddCproc("","linealitySpace",FALSE,linealitySpace);
  p->iiAddCproc("","negatedCone",FALSE,negatedCone);
  p->iiAddCproc("","quotientLatticeBasis",FALSE,quotientLatticeBasis);
  p->iiAddCproc("","randomPoint",FALSE,randomPoint);
  p->iiAddCproc("","rays",FALSE,rays);
  p->iiAddCproc("","relativeInteriorPoint",FALSE,relativeInteriorPoint);
  p->iiAddCproc("","semigroupGenerator",FALSE,semigroupGenerator);
  p->iiAddCproc("","setLinearForms",FALSE,setLinearForms);
  p->iiAddCproc("","setMultiplicity",FALSE,setMultiplicity);
  p->iiAddCproc("","span",FALSE,impliedEquations);
  p->iiAddCproc("","uniquePoint",FALSE,uniquePoint);
  p->iiAddCproc("","listContainsCone",FALSE,containsCone);
  p->iiAddCproc("","listOfFacets",FALSE,listOfFacets);
  p->iiAddCproc("","maximalGroebnerCone",FALSE,maximalGroebnerCone);
  p->iiAddCproc("","groebnerCone",FALSE,groebnerCone);
  p->iiAddCproc("","initial",FALSE,initial);
  p->iiAddCproc("","homogeneitySpace",FALSE,homogeneitySpace);
  p->iiAddCproc("","facetContaining",FALSE,facetContaining);
  coneID=setBlackboxStuff(b,"cone");
}
