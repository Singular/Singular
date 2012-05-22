#include <Singular/mod2.h>

#ifdef HAVE_FANS

#include <omalloc/omalloc.h>
#include <kernel/bigintmat.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>
#include <kernel/longrat.h>
#include <Singular/blackbox.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>
#include <sstream>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

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
    return nlInit(temp,NULL);
  }
  else
    return nlInitgmp(i);
}

bigintmat* zVectorToBigintmat(const gfan::ZVector &zv)
{
  int d=zv.size();
  bigintmat* bm = new bigintmat(d,1);
  for(int i=1;i<=d;i++)
  {
    number temp = integerToNumber(zv[i-1]); 
    bm->set(i,1,temp);
    nlDelete(&temp,NULL);
  }
  return bm;
}

bigintmat* zMatrixToBigintmat(const gfan::ZMatrix &zm)
{
  int d=zm.getHeight();
  int n=zm.getWidth();
  bigintmat* bm = new bigintmat(d,n);
  for(int i=1;i<=d;i++)
    for(int j=1; j<=n; j++)
    {
      number temp = integerToNumber(zm[i-1][j-1]); 
      bm->set(i,j,temp);
      nlDelete(&temp,NULL);
    }
  return bm;
}

// gfan::ZMatrix* intmat2ZMatrix(const intvec* iMat)
// {
//   int d=iMat->rows();
//   int n=iMat->cols();
//   gfan::ZMatrix* ret = new gfan::ZMatrix(d,n);
//   for(int i=0;i<d;i++)
//     for(int j=0;j<n;j++)
//       (*ret)[i][j]=IMATELEM(*iMat, i+1, j+1);
//   return ret;
// }

gfan::Integer numberToInteger(const number &n)
{
  if (SR_HDL(n) & SR_INT)
    return SR_TO_INT(n);
  else
    return n->z;
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
      (*zm)[i][j] = numberToInteger(temp);
      nlDelete(&temp,NULL);
    }
  return zm;
}

gfan::ZVector* bigintmatToZVector(const bigintmat &bim)
{
  gfan::ZVector* zv=new gfan::ZVector(bim.rows());
  for(int j=0; j<bim.rows(); j++)
  {
    number temp = BIMATELEM(bim, j+1, 1);
    (*zv)[j] = numberToInteger(temp);
    nlDelete(&temp,NULL);
  }
  return zv;
}

/* expects iMat to have just one row */
// gfan::ZVector* intvec2ZVector(const intvec* iVec)
// {
//   int n =iVec->rows();
//   gfan::ZVector* ret = new gfan::ZVector(n);
//   for(int j=0;j<n;j++)
//     (*ret)[j]=IMATELEM(*iVec, j+1, 1);
//   return ret;
// }

std::string toString(gfan::ZMatrix const &m, char *tab)
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
     return omStrDup(s.c_str());
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
          Werror("mismatching ambient dimensions");
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


/* singular wrapper for gfanlib functions */


static BOOLEAN jjCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
  bigintmat* ineq = NULL;
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) v->Data();
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) v->Data();
  gfan::ZMatrix* zm = bigintmatToZMatrix(ineq);
  gfan::ZCone* zc = new gfan::ZCone(*zm, gfan::ZMatrix(0, zm->getWidth()));
  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete ineq;
  zc->canonicalize();
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
  bigintmat* ineq = NULL; bigintmat* eq = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) u->Data();
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0);     
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
  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;
  zc->canonicalize();
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
  bigintmat* ineq = NULL; bigintmat* eq = NULL;
  if (u->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) u->Data();
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0);     
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
  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;
  zc->canonicalize();
  res->rtyp = coneID;
  res->data = (char *)zc;
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
  WerrorS("coneViaNormals: unexpected parameters");
  return TRUE;
}

static BOOLEAN qqCONENORMALS1(leftv res, leftv v)
{
  /* method for generating a cone object from inequalities;
     valid parametrizations: (intmat) */
  bigintmat* ineq = NULL;
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* ineq0 = (intvec*) v->Data();
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) v->Data();

  gfan::ZMatrix* zm = bigintmatToZMatrix(ineq);
  gfan::ZCone* zc = new gfan::ZCone(*zm, gfan::ZMatrix(0, zm->getWidth()));
  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete ineq;
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN qqCONENORMALS2(leftv res, leftv u, leftv v)
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
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0);     
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
  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

static BOOLEAN qqCONENORMALS3(leftv res, leftv u, leftv v, leftv w)
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
    ineq = iv2bim(ineq0);     
  }
  else
    ineq = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* eq0 = (intvec*) v->Data();
    eq = iv2bim(eq0);     
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
  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete ineq;
  if (v->Typ() == INTMAT_CMD)
    delete eq;
  res->rtyp = coneID;
  res->data = (char *)zc;
  return FALSE;
}

BOOLEAN quickConeViaNormals(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL) return qqCONENORMALS1(res, u);
  }
  leftv v = u->next;
  if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
  {
    if (v->next == NULL) return qqCONENORMALS2(res, u, v);
  }
  leftv w = v->next;
  if ((w != NULL) && (w->Typ() == INT_CMD))
  {
    if (w->next == NULL) return qqCONENORMALS3(res, u, v, w);
  }
  WerrorS("quickConeViaNormals: unexpected parameters");
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
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) v->Data();

  gfan::ZMatrix* zm = bigintmatToZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
  zc->canonicalize();
  res->rtyp = coneID;
  res->data = (char *)zc;

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
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0);     
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
  zc->canonicalize();
  res->rtyp = coneID;
  res->data = (char *)zc;

  delete zm1, zm2;
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
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0);     
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
  zc->canonicalize();
  res->rtyp = coneID;
  res->data = (char *)zc;

  delete zm1, zm2;
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
  WerrorS("coneViaRays: unexpected parameters");
  return TRUE;
}

static BOOLEAN qqCONERAYS1(leftv res, leftv v)
{
  /* method for generating a cone object from half-lines
     (cone = convex hull of the half-lines; note: there may be
     entire lines in the cone);
     valid parametrizations: (intmat) */
  bigintmat* rays = NULL;
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* rays0 = (intvec*) v->Data();
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) v->Data();
 
  gfan::ZMatrix* zm = bigintmatToZMatrix(rays);
  gfan::ZCone* zc = new gfan::ZCone();
  *zc = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
  res->rtyp = coneID;
  res->data = (char *)zc;

  delete zm;
  if (v->Typ() == INTMAT_CMD)
    delete rays;
  return FALSE;
}

static BOOLEAN qqCONERAYS2(leftv res, leftv u, leftv v)
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
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0);     
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
  res->data = (char *)zc;

  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete rays;
  if (v->Typ() == INTMAT_CMD)
    delete linSpace;
  return FALSE;
}

static BOOLEAN qqCONERAYS3(leftv res, leftv u, leftv v, leftv w)
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
    rays = iv2bim(rays0);     
  }
  else
    rays = (bigintmat*) u->Data();
  if (v->Typ() == INTMAT_CMD)
  {
    intvec* linSpace0 = (intvec*) v->Data();
    linSpace = iv2bim(linSpace0);     
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
  res->data = (char *)zc;

  delete zm1, zm2;
  if (u->Typ() == INTMAT_CMD)
    delete rays;
  if (v->Typ() == INTMAT_CMD)
    delete linSpace;
  return FALSE;
}

BOOLEAN quickConeViaRays(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && ((u->Typ() == BIGINTMAT_CMD) || (u->Typ() == INTMAT_CMD)))
  {
    if (u->next == NULL) return qqCONERAYS1(res, u);
    leftv v = u->next;
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTMAT_CMD)))
    {
      if (v->next == NULL) return qqCONERAYS2(res, u, v);
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        if (w->next == NULL) return qqCONERAYS3(res, u, v, w);
      }
    }
  }
  WerrorS("quickConeViaRays: unexpected parameters");
  return TRUE;
}


BOOLEAN inequalities(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    
    gfan::ZMatrix zmat = zc->getInequalities();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("inequalities: unexpected parameters");
  return TRUE;
}

BOOLEAN equations(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("equations: unexpected parameters");
  return TRUE;
}

BOOLEAN facets(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zm = zc->getFacets();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (char*)zMatrixToBigintmat(zm);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*)getFacetNormals(zc);
      return FALSE;
    }
  WerrorS("facets: unexpected parameters");
  return TRUE;
}

BOOLEAN impliedEquations(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getImpliedEquations();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("impliedEquations: unexpected parameters");
  return TRUE;
}

BOOLEAN generatorsOfSpan(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfSpan();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("generatorsOfSpan: unexpected parameters");
  return TRUE;
}

BOOLEAN generatorsOfLinealitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->generatorsOfLinealitySpace();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
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
    // res->rtyp = INTMAT_CMD;
    // res->data = (void*)zMatrixToIntmat(zm);
    res->rtyp = BIGINTMAT_CMD;
    res->data = (char*)zMatrixToBigintmat(zm);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    gfan::ZMatrix zmat = rays(zf);
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
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
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("quotientLatticeBasis: unexpected parameters");
  return TRUE;
}

BOOLEAN linearForms(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    gfan::ZMatrix zmat = zc->getLinearForms();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*)zMatrixToBigintmat(zmat);
    return FALSE;
  }
  WerrorS("linearForms: unexpected parameters");
  return TRUE;
}

BOOLEAN ambientDimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) zc->ambientDimension();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) getAmbientDimension(zf);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) getAmbientDimension(zc);
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
    res->data = (char*) zc->dimension();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) getDimension(zf);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == polytopeID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) getDimension(zc);
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
      res->data = (char*) zc->codimension();
      return FALSE;
    }
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      res->rtyp = INT_CMD;
      res->data = (char*) getCodimension(zf);
      return FALSE;
    }
  if ((u != NULL) && (u->Typ() == polytopeID))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      res->rtyp = INT_CMD;
      res->data = (char*) getCodimension(zc);
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
    res->data = (char*) zc->dimensionOfLinealitySpace();
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    res->rtyp = INT_CMD;
    res->data = (char*) getLinealityDimension(zf);
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
    bool ok = true;
    number i = integerToNumber(zc->getMultiplicity());
    if (!ok)
      WerrorS("overflow while converting a gfan::Integer to an int");
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
    int i = zc->isOrigin() ? 1 : 0;
    res->rtyp = INT_CMD;
    res->data = (void*) i;
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
    int i = zc->isFullSpace() ? 1 : 0;
    res->rtyp = INT_CMD;
    res->data = (void*) i;
    return FALSE;
  }
  WerrorS("isFullSpace: unexpected parameters");
  return TRUE;
}

int isSimplicial(gfan::ZCone* zc)
{
  int i = zc->isSimplicial() ? 1 : 0;
  return i;
}

BOOLEAN containsPositiveVector(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*)u->Data();
    int i = zc->containsPositiveVector() ? 1 : 0;
    res->rtyp = INT_CMD;
    res->data = (void*) i;
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
  WerrorS("dualCone: unexpected parameters");
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
    else
    {
      Werror("expected dim of cone one larger than dim of lin space\n"
             "but got dimensions %d and %d", d, dLS);
    }
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
        mat = iv2bim(mat0);
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
  }
  WerrorS("intersectCones: unexpected parameters");
  return TRUE;
}

BOOLEAN takeUnion(leftv res, leftv args)
{
  leftv u = args;
  std::cout << u->Typ() << (u != NULL) << std::endl;
  if ((u != NULL) && (u->Typ() == coneID))
  {
    leftv v = u->next;
    std::cout << v->Typ() << (v != NULL) << std::endl;
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
      zc3.canonicalize();
      if (zc1->hasFace(zc3) && zc2->hasFace(zc3))
      {
	gfan::ZMatrix zm1 = zc1->extremeRays();
	gfan::ZMatrix zm2 = zc2->extremeRays();
	gfan::ZMatrix zm11 = zc1->generatorsOfLinealitySpace();
	gfan::ZMatrix zm22 = zc2->generatorsOfLinealitySpace();
	gfan::ZMatrix zm = combineOnTop(combineOnTop(combineOnTop(zm1,zm2),zm11),zm22);
        gfan::ZCone* zc = new gfan::ZCone();
        *zc = gfan::ZCone::givenByRays(zm, gfan::ZMatrix(0, zm.getWidth()));
        res->rtyp = coneID;
        res->data = (char*) zc;
        return FALSE;
      }
      WerrorS("takeUnion: cones do not share common face");
    }
  }
  WerrorS("takeUnion: unexpected parameters");
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
        iv = iv2bim(iv0);
      }
      else
        iv = (bigintmat*)v->Data();
      gfan::ZVector* zv = bigintmatToZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv->size();
      if (d1 != d2)
        Werror("expected ambient dim of cone and size of vector\n"
               "to be equal but got %d and %d", d1, d2);
      if(!zc->contains(*zv))
      {
        WerrorS("the provided intvec does not lie in the cone");
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
  // if ((u != NULL) && (u->Typ() == fanID))          // TODO
  // {
  //   leftv v=u->next;
  //   if ((v != NULL) && (v->Typ() == coneID))
  //   {
  //     gfan::ZFan* zf = (gfan::ZFan*)u->Data();
  //     gfan::ZCone* zc = (gfan::ZCone*)v->Data();
  //     gfan::ZMatrix zm = zc->extremeRays();
  //     bool b = 1;
  //     for(int i=1; i<zm->getHeight(); i++)
  //     {

  //     }
  //     res->rtyp = INT_CMD;
  //     res->data = (char*) (int) contains(zf,zc);
  //     return FALSE;
  //   }
  // }
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
        Werror("expected cones with same ambient dimensions\n but got"
	       " dimensions %d and %d", d1, d2);
      bool b = (zc->contains(*zd) ? 1 : 0);
      res->rtyp = INT_CMD;
      res->data = (char*) (int) b;
      return FALSE;
    }
    if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      bigintmat* iv = NULL;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* iv0 = (intvec*) v->Data();
        iv = iv2bim(iv0);
      }
      else
        iv = (bigintmat*)v->Data();
      gfan::ZVector* zv = bigintmatToZVector(iv);
      int d1 = zc->ambientDimension();
      int d2 = zv->size();
      if (d1 != d2)
	Werror("expected ambient dim of cone and size of vector\n"
               "to be equal but got %d and %d", d1, d2);
      bool b = (zc->contains(*zv) ? 1 : 0);
      res->rtyp = INT_CMD;
      res->data = (char*) (int) b;

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
        iv = iv2bim(iv0);
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
      int bb = (int) b;
      res->rtyp = INT_CMD;
      res->data = (char*) bb;
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
      int bb = (int) b;
      res->rtyp = INT_CMD;
      res->data = (char*) bb;
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
    res->data = (char*) zd;
    return FALSE;
  }
  WerrorS("canonicalizeCone: unexpected parameters");
  return TRUE;
}

void bbcone_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefull,
  // all other are simply error messages
  b->blackbox_destroy=bbcone_destroy;
  b->blackbox_String=bbcone_String;
  //b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=bbcone_Init;
  b->blackbox_Copy=bbcone_Copy;
  b->blackbox_Assign=bbcone_Assign;
  b->blackbox_Op2=bbcone_Op2;
  iiAddCproc("","coneViaNormals",FALSE,coneViaNormals);
  iiAddCproc("","quickConeViaNormals",FALSE,quickConeViaNormals);
  iiAddCproc("","coneViaRays",FALSE,coneViaRays);
  iiAddCproc("","quickConeViaRays",FALSE,quickConeViaRays);
  iiAddCproc("","canonicalizeCone",FALSE,canonicalizeCone);

  iiAddCproc("","inequalities",FALSE,inequalities);
  iiAddCproc("","impliedInequalities",FALSE,facets);
  iiAddCproc("","facets",FALSE,facets);
  iiAddCproc("","equations",FALSE,equations);
  iiAddCproc("","impliedEquations",FALSE,impliedEquations);
  iiAddCproc("","rays",FALSE,rays);
  iiAddCproc("","generatorsOfLinealitySpace",FALSE,generatorsOfLinealitySpace);
  iiAddCproc("","linealitySpace",FALSE,linealitySpace);
  iiAddCproc("","generatorsOfSpan",FALSE,generatorsOfSpan);

  iiAddCproc("","dimension",FALSE,dimension);
  iiAddCproc("","codimension",FALSE,codimension);
  iiAddCproc("","ambientDimension",FALSE,ambientDimension);
  iiAddCproc("","linealityDimension",FALSE,linealityDimension);

  iiAddCproc("","setMultiplicity",FALSE,setMultiplicity);
  iiAddCproc("","getMultiplicity",FALSE,getMultiplicity);
  iiAddCproc("","setLinearForms",FALSE,setLinearForms);
  iiAddCproc("","linearForms",FALSE,linearForms);

  iiAddCproc("","dualCone",FALSE,dualCone);
  iiAddCproc("","negatedCone",FALSE,negatedCone);
  iiAddCproc("","coneLink",FALSE,coneLink);
  iiAddCproc("","intersectCones",FALSE,intersectCones);
  // iiAddCproc("","uniteCones",FALSE,takeUnion);

  iiAddCproc("","isOrigin",FALSE,isOrigin);
  iiAddCproc("","isFullSpace",FALSE,isFullSpace);

  iiAddCproc("","containsAsFace",FALSE,hasFace);
  iiAddCproc("","containsInSupport",FALSE,containsInSupport);
  iiAddCproc("","containsRelatively",FALSE,containsRelatively);
  iiAddCproc("","containsPositiveVector",FALSE,containsPositiveVector);

  // iiAddCproc("","contains",FALSE,contains);
  iiAddCproc("","relativeInteriorPoint",FALSE,relativeInteriorPoint);
  iiAddCproc("","quotientLatticeBasis",FALSE,quotientLatticeBasis);
  iiAddCproc("","semigroupGenerator",FALSE,semigroupGenerator);
  iiAddCproc("","uniquePoint",FALSE,uniquePoint);
  // iiAddCproc("","faceContaining",FALSE,faceContaining);
  coneID=setBlackboxStuff(b,"cone");
  //Print("created type %d (cone)\n",coneID); 
}

#endif
/* HAVE_FANS */
