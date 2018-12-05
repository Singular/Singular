#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "misc/intvec.h"
#include "misc/sirandom.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"

#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/blackbox.h"

#include "Singular/links/ssiLink.h"

#include "callgfanlib_conversion.h"

#include "gfanlib/gfanlib.h"
#include "gfanlib/gfanlib_q.h"

#include "bbfan.h"
#include "bbpolytope.h"

int coneID;

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
    newZc = (gfan::ZCone*)r->CopyD();
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
  gfan::initializeCddlibIfRequired();
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL)
    {
      BOOLEAN bo = jjCONENORMALS1(res, u);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
  }
  leftv v = u->next;
  if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
  {
    if (v->next == NULL)
    {
      BOOLEAN bo = jjCONENORMALS2(res, u, v);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL)
    {
      BOOLEAN bo = jjCONENORMALS3(res, u, v, w);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
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
  gfan::initializeCddlibIfRequired();
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL)
    {
      BOOLEAN bo = jjCONERAYS1(res, u);
      gfan::deinitializeCddlibIfRequired();
      return bo;
    }
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
    {
      if (v->next == NULL)
      {
        BOOLEAN bo = jjCONERAYS2(res, u, v);
        gfan::deinitializeCddlibIfRequired();
        return bo;
      }
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        if (w->next == NULL)
        {
          BOOLEAN bo = jjCONERAYS3(res, u, v, w);
          gfan::deinitializeCddlibIfRequired();
          return bo;
        }
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getInequalities();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zm = zc->getFacets();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zm);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getImpliedEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfSpan();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfLinealitySpace();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zm = zc->extremeRays();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zm);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    gfan::ZMatrix zmat = rays(zf);
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->quotientLatticeBasis();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getLinearForms();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zMatrixToBigintmat(zmat);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->ambientDimension();
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getAmbientDimension(zf);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getAmbientDimension(zc);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->dimension();
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getDimension(zf);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getDimension(zc);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->codimension();
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getCodimension(zf);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getCodimension(zc);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) zc->dimensionOfLinealitySpace();
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) getLinealityDimension(zf);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    number i = integerToNumber(zc->getMultiplicity());
    res->rtyp = BIGINT_CMD;
    res->data = (void*) i;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->isOrigin();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->isFullSpace();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    int b = zc->isSimplicial();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) b;
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    bool b = isSimplicial(zf);
    res->rtyp = INT_CMD;
    res->data = (void*) (long) b;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->containsPositiveVector();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) i;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->linealitySpace());
    res->rtyp = coneID;
    res->data = (void*) zd;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->dualCone());
    res->rtyp = coneID;
    res->data = (void*) zd;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(zc->negated());
    res->rtyp = coneID;
    res->data = (void*) zd;
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int d = zc->dimension();
    int dLS = zc->dimensionOfLinealitySpace();
    if (d == dLS + 1)
    {
      gfan::ZVector zv = zc->semiGroupGeneratorOfRay();
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(zv);
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = zc->getRelativeInteriorPoint();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = zc->getUniquePoint();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("uniquePoint: unexpected parameters");
  return TRUE;
}

int siRandBound(const int b)
{
  int n = 0;
  while (n==0)
  {
    n = siRand();
    if (b>1)
    {
      n = n % b;
    }
  }
  return n;
}

gfan::ZVector randomPoint(const gfan::ZCone* zc, const int b)
{
  gfan::ZVector rp = gfan::ZVector(zc->ambientDimension());

  gfan::ZMatrix rays = zc->extremeRays();
  for (int i=0; i<rays.getHeight(); i++)
    rp += siRandBound(b) * rays[i].toVector();

  // gfan::ZMatrix lins = zc->generatorsOfLinealitySpace();
  // for (int i=0; i<lins.getHeight(); i++)
  // {
  //   int n = siRandBound(b);
  //   rp = rp + n * lins[i].toVector();
  // }

  return rp;
}

BOOLEAN randomPoint(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::initializeCddlibIfRequired();

    int b = 0;
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      b = (int) (long) v->Data();
      b = b+1;
    }

    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZVector zv = randomPoint(zc,b);

    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      int val = (int)(long)v->Data();
      zc->setMultiplicity(gfan::Integer(val));
      res->rtyp = NONE;
      res->data = NULL;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
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
      gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    gfan::ZMatrix ineq=zc->getInequalities();
    gfan::ZMatrix eq=zc->getEquations();
    gfan::ZCone* zd = new gfan::ZCone(liftUp(ineq),liftUp(eq));
    res->rtyp = polytopeID;
    res->data = (void*) zd;
    gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = coneID;
      res->data = (void *)new gfan::ZCone(zc3);
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc11 = (gfan::ZCone*)u->Data();
      gfan::ZCone zc1 = liftUp(*zc11);
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1.ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    if (u->next == NULL)
    {
      lists l = (lists) u->Data();

      // find the total number of inequalities and equations
      int r1=0; // total number of inequalities
      int r2=0; // total number of equations
      int c=0;  // ambient dimension
      for (int i=0; i<=lSize(l); i++)
      {
        if (l->m[i].Typ() != coneID)
        {
          WerrorS("convexIntersection: entries of wrong type in list");
          return TRUE;
        }
        gfan::ZCone* ll = (gfan::ZCone*) l->m[i].Data();
        r1 = r1 + ll->getInequalities().getHeight();
        r2 = r2 + ll->getEquations().getHeight();
      }
      if (lSize(l)>=0)
      {
        gfan::ZCone* ll = (gfan::ZCone*) l->m[0].Data();
        c = ll->getInequalities().getWidth();
      }
      gfan::ZMatrix totalIneqs(r1,c);
      gfan::ZMatrix totalEqs(r2,c);

      // concat all inequalities and equations
      r1=0; // counter for inequalities
      r2=0; // counter for equations
      for (int i=0; i<=lSize(l); i++)
      {
        gfan::ZCone* ll = (gfan::ZCone*) l->m[i].Data();
        gfan::ZMatrix ineqs = ll->getInequalities();
        for (int j=0; j<ineqs.getHeight(); j++)
        {
          totalIneqs[r1]=ineqs[j];
          r1 = r1+1;
        }
        gfan::ZMatrix eqs = ll->getEquations();
        for (int j=0; j<eqs.getHeight(); j++)
        {
          totalEqs[r2]=eqs[j];
          r2 = r2+1;
        }
      }

      gfan::ZCone* zc = new gfan::ZCone(totalIneqs,totalEqs);
      zc->canonicalize();
      res->rtyp = coneID;
      res->data = (void *) zc;
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc22 = (gfan::ZCone*)v->Data();
      gfan::ZCone zc2 = liftUp(*zc22);
      int d1 = zc1->ambientDimension();
      int d2 = zc2.ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZCone zc3 = gfan::intersection(*zc1, *zc2);
      zc3.canonicalize();
      res->rtyp = polytopeID;
      res->data = (void *)new gfan::ZCone(zc3);
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension();
      int d2 = zc2->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
       gfan::deinitializeCddlibIfRequired();
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
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc11 = (gfan::ZCone*)u->Data();
      gfan::ZCone zc1 = liftUp(*zc11);
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1.ambientDimension()-1;
      int d2 = zc2->ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
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
      gfan::deinitializeCddlibIfRequired();
     return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc22 = (gfan::ZCone*)v->Data();
      gfan::ZCone zc2 = liftUp(*zc22);
      int d1 = zc1->ambientDimension()-1;
      int d2 = zc2.ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
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
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc1 = (gfan::ZCone*)u->Data();
      gfan::ZCone* zc2 = (gfan::ZCone*)v->Data();
      int d1 = zc1->ambientDimension()-1;
      int d2 = zc2->ambientDimension()-1;
      if (d1 != d2)
      {
        Werror("expected ambient dims of both cones to coincide\n"
                "but got %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZMatrix zm1 = zc1->extremeRays();
      gfan::ZMatrix zm2 = zc2->extremeRays();
      gfan::ZMatrix zm = combineOnTop(zm1,zm2);
      gfan::ZCone* zc = new gfan::ZCone();
      *zc = gfan::ZCone::givenByRays(zm,gfan::ZMatrix(0, zm.getWidth()));
      res->rtyp = polytopeID;
      res->data = (void*) zc;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
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
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      if(!zc->contains(*zv))
      {
        WerrorS("the provided intvec does not lie in the cone");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZCone* zd = new gfan::ZCone(zc->link(*zv));
      res->rtyp = coneID;
      res->data = (void *) zd;

      delete zv;
      if (v->Typ() == INTVEC_CMD)
        delete iv;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      int d1 = zc->ambientDimension();
      int d2 = zd->ambientDimension();
      if (d1 != d2)
      {
        Werror("expected cones with same ambient dimensions\n but got"
               " dimensions %d and %d", d1, d2);
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      bool b = (zc->contains(*zd) ? 1 : 0);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::initializeCddlibIfRequired();
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
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      int b = zc->contains(*zv);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;

      delete zv;
      if (v->Typ() == INTVEC_CMD)
        delete iv;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
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
        if (v->Typ() == INTVEC_CMD)
          delete iv;
        gfan::deinitializeCddlibIfRequired();
        return FALSE;
      }
      delete zv;
      if (v->Typ() == INTVEC_CMD)
        delete iv;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      bool b = zc->hasFace(*zd);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == polytopeID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      gfan::ZCone* zd = (gfan::ZCone*)v->Data();
      bool b = zc->hasFace(*zd);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZCone* zd = new gfan::ZCone(*zc);
    zd->canonicalize();
    res->rtyp = coneID;
    res->data = (void*) zd;
    gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      lists l = (lists) u->Data();
      gfan::ZCone* zc = (gfan::ZCone*) v->Data();
      zc->canonicalize();
      int b = 0;
      for (int i=0; i<=lSize(l); i++)
      {
        if (l->m[i].Typ() != coneID)
        {
          WerrorS("containsCone: entries of wrong type in list");
          gfan::deinitializeCddlibIfRequired();
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
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  WerrorS("containsCone: unexpected parameters");
  return TRUE;
}

BOOLEAN faceContaining(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::initializeCddlibIfRequired();
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

      if (!zc->contains(*point))
      {
        WerrorS("faceContaining: point not in cone");
        return TRUE;
      }
      res->rtyp = coneID;
      res->data = (void*) new gfan::ZCone(zc->faceContaining(*point));

      delete point;
      if (v->Typ() == INTVEC_CMD)
        delete point1;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  WerrorS("faceContaining: unexpected parameters");
  return TRUE;
}


BOOLEAN onesVector(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    int n = (int) (long) u->Data();
    if (n>0)
    {
      intvec* v = new intvec(n);
      for (int i=0; i<n; i++)
        (*v)[i]=1;
      res->rtyp = INTVEC_CMD;
      res->data = (void*) v;
      return FALSE;
    }
  }
  WerrorS("onesVector: unexpected parameters");
  return TRUE;
}

/***
 * Computes a relative interior point for each facet of zc
 **/
gfan::ZMatrix interiorPointsOfFacets(const gfan::ZCone &zc, const std::set<gfan::ZVector> &exceptThese)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  /* our cone has r facets, if r==0 return empty matrices */
  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  if (r==0) return relativeInteriorPoints;

  /* next we iterate over each of the r facets,
   * build the respective cone and add it to the list
   * this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  facet.canonicalize();
  gfan::ZVector interiorPoint = facet.getRelativeInteriorPoint();
  if (exceptThese.count(interiorPoint)==0)
    relativeInteriorPoints.appendRow(interiorPoint);

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    facet.canonicalize();
    interiorPoint = facet.getRelativeInteriorPoint();
    if (exceptThese.count(interiorPoint)==0)
      relativeInteriorPoints.appendRow(interiorPoint);
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  facet = gfan::ZCone(newInequalities,newEquations);
  facet.canonicalize();
  interiorPoint = facet.getRelativeInteriorPoint();
  if (exceptThese.count(interiorPoint)==0)
    relativeInteriorPoints.appendRow(interiorPoint);

  return relativeInteriorPoints;
}


/***
 * Computes a relative interior point and an outer normal vector for each facet of zc
 **/
std::pair<gfan::ZMatrix,gfan::ZMatrix> interiorPointsAndNormalsOfFacets(const gfan::ZCone zc, const std::set<gfan::ZVector> &exceptThesePoints, const bool onlyLowerHalfSpace)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();

  /* our cone has r facets, if r==0 return empty matrices */
  gfan::ZMatrix relativeInteriorPoints = gfan::ZMatrix(0,c);
  gfan::ZMatrix outerFacetNormals = gfan::ZMatrix(0,c);
  if (r==0)
    return std::make_pair(relativeInteriorPoints,outerFacetNormals);

  /* next we iterate over each of the r facets,
   * build the respective cone and add it to the list
   * this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations;
  newEquations.appendRow(inequalities[0]);
  gfan::ZCone facet = gfan::ZCone(newInequalities,newEquations);
  gfan::ZVector interiorPoint = facet.getRelativeInteriorPoint();
  if (onlyLowerHalfSpace==false || interiorPoint[0].sign()<0)
  {
    if (exceptThesePoints.count(interiorPoint)==0)
    {
      relativeInteriorPoints.appendRow(interiorPoint);
      outerFacetNormals.appendRow(-inequalities[0].toVector());
    }
  }

  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    facet = gfan::ZCone(newInequalities,newEquations);
    interiorPoint = facet.getRelativeInteriorPoint();
    if (onlyLowerHalfSpace==false || interiorPoint[0].sign()<0)
    {
      if (exceptThesePoints.count(interiorPoint)==0)
      {
        relativeInteriorPoints.appendRow(interiorPoint);
        outerFacetNormals.appendRow(-inequalities[i].toVector());
      }
    }
  }

  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  facet = gfan::ZCone(newInequalities,newEquations);
  interiorPoint = facet.getRelativeInteriorPoint();
  if (onlyLowerHalfSpace==false || interiorPoint[0].sign()<0)
  {
    if (exceptThesePoints.count(interiorPoint)==0)
    {
      relativeInteriorPoints.appendRow(interiorPoint);
      outerFacetNormals.appendRow(-inequalities[r-1].toVector());
    }
  }

  return std::make_pair(relativeInteriorPoints,outerFacetNormals);
}


static void gfanIntegerWriteFd(gfan::Integer n, ssiInfo* dd)
{
  mpz_t tmp;
  mpz_init(tmp);
  n.setGmp(tmp);
  mpz_out_str (dd->f_write,SSI_BASE, tmp);
  mpz_clear(tmp);
  fputc(' ',dd->f_write);
}

static void gfanZMatrixWriteFd(gfan::ZMatrix M, ssiInfo* dd)
{
  fprintf(dd->f_write,"%d %d ",M.getHeight(),M.getWidth());

  for (int i=0; i<M.getHeight(); i++)
  {
    for (int j=0; j<M.getWidth(); j++)
    {
      gfanIntegerWriteFd(M[i][j],dd);
    }
  }
}

BOOLEAN bbcone_serialize(blackbox *b, void *d, si_link f)
{
  ssiInfo *dd = (ssiInfo *)f->data;

  sleftv l;
  memset(&l,0,sizeof(l));
  l.rtyp=STRING_CMD;
  l.data=(void*)"cone";
  f->m->Write(f, &l);

  gfan::ZCone *Z = (gfan::ZCone*) d;
  fprintf(dd->f_write,"%d ",Z->areImpliedEquationsKnown()+Z->areFacetsKnown()*2);

  gfan::ZMatrix i=Z->getInequalities();
  gfanZMatrixWriteFd(i,dd);

  gfan::ZMatrix e=Z->getEquations();
  gfanZMatrixWriteFd(e,dd);

  // assert(i.getWidth() == e.getWidth());
  return FALSE;
}

static gfan::Integer gfanIntegerReadFd(ssiInfo* dd)
{
  mpz_t tmp;
  mpz_init(tmp);
  s_readmpz_base(dd->f_read,tmp,SSI_BASE);
  gfan::Integer n(tmp);
  mpz_clear(tmp);
  return n;
}

static gfan::ZMatrix gfanZMatrixReadFd(ssiInfo* dd)
{
  int r=s_readint(dd->f_read);
  int c=s_readint(dd->f_read);

  gfan::ZMatrix M(r,c);
  for (int i=0; i<r; i++)
  {
    for (int j=0; j<c; j++)
    {
      M[i][j] = gfanIntegerReadFd(dd);
    }
  }
  return M;
}

BOOLEAN bbcone_deserialize(blackbox **b, void **d, si_link f)
{
  ssiInfo *dd = (ssiInfo *)f->data;
  int preassumptions = s_readint(dd->f_read);

  gfan::ZMatrix i = gfanZMatrixReadFd(dd);
  gfan::ZMatrix e = gfanZMatrixReadFd(dd);

  // if (e.getHeight()==0) // why is e sometimes 0x0 and sometimex 0xn???
  //   e = gfan::ZMatrix(0,i.getWidth());

  gfan::ZCone* Z = new gfan::ZCone(i,e,preassumptions);

  *d=Z;
  return FALSE;
}

BOOLEAN convexIntersectionOld(leftv res, leftv args)
{
  gfan::initializeCddlibIfRequired();
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
  WerrorS("convexIntersectionOld: unexpected parameters");
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
  b->blackbox_serialize=bbcone_serialize;
  b->blackbox_deserialize=bbcone_deserialize;
  p->iiAddCproc("gfan.lib","coneViaInequalities",FALSE,coneViaNormals);
  p->iiAddCproc("gfan.lib","coneViaPoints",FALSE,coneViaRays);
  p->iiAddCproc("","listContainsCone",FALSE,containsCone);
  // iiAddCproc("gfan.lib","makePolytope",FALSE,coneToPolytope);
  p->iiAddCproc("gfan.lib","ambientDimension",FALSE,ambientDimension);
  p->iiAddCproc("gfan.lib","canonicalizeCone",FALSE,canonicalizeCone);
  p->iiAddCproc("gfan.lib","codimension",FALSE,codimension);
  p->iiAddCproc("gfan.lib","coneLink",FALSE,coneLink);
  p->iiAddCproc("gfan.lib","containsAsFace",FALSE,hasFace);
  p->iiAddCproc("gfan.lib","containsInSupport",FALSE,containsInSupport);
  p->iiAddCproc("gfan.lib","containsPositiveVector",FALSE,containsPositiveVector);
  p->iiAddCproc("gfan.lib","containsRelatively",FALSE,containsRelatively);
  p->iiAddCproc("gfan.lib","convexHull",FALSE,convexHull);
  p->iiAddCproc("gfan.lib","convexIntersection",FALSE,intersectCones);
  p->iiAddCproc("gfan.lib","dimension",FALSE,dimension);
  p->iiAddCproc("gfan.lib","dualCone",FALSE,dualCone);
  p->iiAddCproc("gfan.lib","equations",FALSE,equations);
  p->iiAddCproc("gfan.lib","facets",FALSE,facets);
  p->iiAddCproc("gfan.lib","generatorsOfLinealitySpace",FALSE,generatorsOfLinealitySpace);
  p->iiAddCproc("gfan.lib","generatorsOfSpan",FALSE,generatorsOfSpan);
  p->iiAddCproc("gfan.lib","getLinearForms",FALSE,getLinearForms);
  p->iiAddCproc("gfan.lib","getMultiplicity",FALSE,getMultiplicity);
  p->iiAddCproc("gfan.lib","inequalities",FALSE,inequalities);
  p->iiAddCproc("gfan.lib","isFullSpace",FALSE,isFullSpace);
  p->iiAddCproc("gfan.lib","isOrigin",FALSE,isOrigin);
  p->iiAddCproc("gfan.lib","isSimplicial",FALSE,isSimplicial);
  p->iiAddCproc("gfan.lib","linealityDimension",FALSE,linealityDimension);
  p->iiAddCproc("gfan.lib","linealitySpace",FALSE,linealitySpace);
  p->iiAddCproc("gfan.lib","negatedCone",FALSE,negatedCone);
  p->iiAddCproc("gfan.lib","quotientLatticeBasis",FALSE,quotientLatticeBasis);
  p->iiAddCproc("gfan.lib","randomPoint",FALSE,randomPoint);
  p->iiAddCproc("gfan.lib","rays",FALSE,rays);
  p->iiAddCproc("gfan.lib","relativeInteriorPoint",FALSE,relativeInteriorPoint);
  p->iiAddCproc("gfan.lib","semigroupGenerator",FALSE,semigroupGenerator);
  p->iiAddCproc("gfan.lib","setLinearForms",FALSE,setLinearForms);
  p->iiAddCproc("gfan.lib","setMultiplicity",FALSE,setMultiplicity);
  p->iiAddCproc("gfan.lib","span",FALSE,impliedEquations);
  p->iiAddCproc("gfan.lib","uniquePoint",FALSE,uniquePoint);
  p->iiAddCproc("gfan.lib","faceContaining",FALSE,faceContaining);
  p->iiAddCproc("gfan.lib","onesVector",FALSE,onesVector);
  p->iiAddCproc("gfan.lib","convexIntersectionOld",FALSE,convexIntersectionOld);
  coneID=setBlackboxStuff(b,"cone");
}

#endif
