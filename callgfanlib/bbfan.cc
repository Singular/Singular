#include <Singular/mod2.h>
#ifdef HAVE_FANS

#include <Singular/ipid.h>
#include <kernel/bigintmat.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/lists.h>
#include <sstream>
#include <gfanlib/gfanlib.h>
#include <gfanlib/gfanlib_zfan.h>

#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbcone.h>
#include <callgfanlib/gfan.h>

int fanID;

void *bbfan_Init(blackbox *b)
{
  return (void*)(new gfan::ZFan(0));
}

void bbfan_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    gfan::ZFan* zf = (gfan::ZFan*) d;
    delete zf;
  }
}

char * bbfan_String(blackbox *b, void *d)
{
  if (d==NULL) return omStrDup("invalid object");
  else
  {
    gfan::ZFan* zf = (gfan::ZFan*)d;
    std::string s = zf->toString();
    return omStrDup(s.c_str());
  }
}

void * bbfan_Copy(blackbox*b, void *d)
{
  gfan::ZFan* zf = (gfan::ZFan*)d;
  gfan::ZFan* newZf = new gfan::ZFan(*zf);
  return newZf;
}

BOOLEAN bbfan_Assign(leftv l, leftv r)
{
  gfan::ZFan* newZf;
  if (r==NULL)
  {
    if (l->Data()!=NULL)
    {
      gfan::ZFan* zd = (gfan::ZFan*)l->Data();
      delete zd;
    }
    newZf = new gfan::ZFan(0);
  }
  else if (r->Typ()==l->Typ())
  {
    if (l->Data()!=NULL)
    {
      gfan::ZFan* zd = (gfan::ZFan*)l->Data();
      delete zd;
    }
    gfan::ZFan* zf = (gfan::ZFan*)r->Data();
    newZf = new gfan::ZFan(*zf);
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
      gfan::ZFan* zd = (gfan::ZFan*)l->Data();
      delete zd;
    }
    newZf = new gfan::ZFan(ambientDim);
  }
  else
  {
    Werror("assign Type(%d) = Type(%d) not implemented",l->Typ(),r->Typ());
    return TRUE;
  }

  if (l->rtyp==IDHDL)
  {
    IDDATA((idhdl)l->data)=(char *)newZf;
  }
  else
  {
    l->data=(void *)newZf;
  }
  return FALSE;
}

/* returns 1 iff all rows consist of entries 1..n,
   where n is the number of columns of the provided
   bigintmat; 0 otherwise */
static gfan::IntMatrix permutationIntMatrix(const bigintmat* iv)
{
        int cc = iv->cols();
        int rr = iv->rows();
        bigintmat* ivCopy = new bigintmat(rr, cc);
        for (int r = 1; r <= rr; r++)
          for (int c = 1; c <= cc; c++)
	  {
	    number temp1 = nlInit(1,NULL);
	    number temp2 = nlSub(IMATELEM(*iv, r, c),temp1);
	    ivCopy->set(r,c,temp2);
	    nlDelete(&temp1,NULL); 
	    nlDelete(&temp2,NULL);
	  }
        gfan::ZMatrix* zm = bigintmatToZMatrix(ivCopy);
        gfan::IntMatrix* im = new gfan::IntMatrix(gfan::ZToIntMatrix(*zm));
        delete zm;
        return *im;
}
static BOOLEAN jjFANEMPTY_I(leftv res, leftv v)
{
  int ambientDim = (int)(long)v->Data();
  if (ambientDim < 0)
  {
    Werror("expected non-negative ambient dim but got %d", ambientDim);
    return TRUE;
  }
  res->rtyp = fanID;
  res->data = (char*)(new gfan::ZFan(ambientDim));
  return FALSE;
}

static BOOLEAN jjFANEMPTY_IM(leftv res, leftv v)
{
  bigintmat* permutations = (bigintmat*)v->Data();
  int ambientDim = permutations->cols();
  gfan::IntMatrix im = permutationIntMatrix(permutations);
  if (!gfan::Permutation::arePermutations(im))
  {
    Werror("provided bigintmat contains invalid permutations of {1, ..., %d}", ambientDim);
    return TRUE;
  }
  gfan::SymmetryGroup sg = gfan::SymmetryGroup(ambientDim);
  sg.computeClosure(im);
  res->rtyp = fanID;
  res->data = (char*)(new gfan::ZFan(sg));
  return FALSE;
}

BOOLEAN emptyFan(leftv res, leftv args)
{
  leftv u = args;
  if (u == NULL)
  {
    res->rtyp = fanID;
    res->data = (void*)(new gfan::ZFan(0));
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    if (u->next == NULL) return jjFANEMPTY_I(res, u);
  }
  if ((u != NULL) && (u->Typ() == BIGINTMAT_CMD))
  {
    if (u->next == NULL) return jjFANEMPTY_IM(res, u);
  }
  WerrorS("emptyFan: unexpected parameters");
  return TRUE;
}

static BOOLEAN jjFANFULL_I(leftv res, leftv v)
{
  int ambientDim = (int)(long)v->Data();
  if (ambientDim < 0)
  {
    Werror("expected non-negative ambient dim but got %d", ambientDim);
    return TRUE;
  }
  gfan::ZFan* zf = new gfan::ZFan(gfan::ZFan::fullFan(ambientDim));
  res->rtyp = fanID;
  res->data = (char*)zf;
  return FALSE;
}
static BOOLEAN jjFANFULL_IM(leftv res, leftv v)
{
  bigintmat* permutations = (bigintmat*)v->Data();
  int ambientDim = permutations->cols();
  gfan::IntMatrix im = permutationIntMatrix(permutations);
  if (!gfan::Permutation::arePermutations(im))
  {
    Werror("provided bigintmat contains invalid permutations of {1, ..., %d}", ambientDim);
    return TRUE;
  }
  gfan::SymmetryGroup sg = gfan::SymmetryGroup(ambientDim);
  sg.computeClosure(im);
  gfan::ZFan* zf = new gfan::ZFan(gfan::ZFan::fullFan(sg));
  res->rtyp = fanID;
  res->data = (char*)zf;
  return FALSE;
}

BOOLEAN fullFan(leftv res, leftv args)
{
  /*  {
    gfan::ZFan f(2);
    std::cout<<f.toString();
    f.insert(gfan::ZCone::positiveOrthant(2));
    std::cout<<f.toString();
    }*/


  leftv u = args;
  if (u == NULL)
  {
    res->rtyp = fanID;
    res->data = (void*)(new gfan::ZFan(0));
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    if (u->next == NULL) return jjFANFULL_I(res, u);
  }
  if ((u != NULL) && (u->Typ() == BIGINTMAT_CMD))
  {
    if (u->next == NULL) return jjFANFULL_IM(res, u);
  }
  WerrorS("fullFan: unexpected parameters");
  return TRUE;
}

int getAmbientDimension(gfan::ZFan* zf)
{
  return zf->getAmbientDimension();
}

int getCodimension(gfan::ZFan* zf)
{
  return zf->getCodimension();
}

int getDimension(gfan::ZFan* zf)
{
  return zf->getDimension();
}

int getLinealityDimension(gfan::ZFan* zf)
{
  return zf->getLinealityDimension();
}

BOOLEAN numberOfConesOfDimension(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      leftv v=u->next;
      if ((v != NULL) && (v->Typ() == INT_CMD))
        {
          leftv w=v->next;
          if ((w != NULL) && (w->Typ() == INT_CMD))
            {
              leftv x=w->next;
              if ((x != NULL) && (x->Typ() == INT_CMD))
                {
                  gfan::ZFan* zf = (gfan::ZFan*) u->Data();
                  int d = (int)(long)v->Data();
                  int o = (int)(long)w->Data();
                  int m = (int)(long)x->Data();
                  if ( (0<=d) && (d <= zf->getAmbientDimension()) && ((o == 0) || (o == 1)) && ((m == 0) || (m == 1)))
                    {
                      bool oo = (bool) o;
                      bool mm = (bool) m;
                      int ld = zf->getLinealityDimension();
                      if (d-ld>=0)
                      {
                        int n = zf->numberOfConesOfDimension(d,oo,mm);
                        res->rtyp = INT_CMD;
                        res->data = (void*) n;
                        return FALSE;
                      }
                      else
                      {
                        res->rtyp = INT_CMD;
                        res->data = (char*) 0;
                        return FALSE;
                      }
                    }
                }
            }
        }
    }
  WerrorS("numberOfConesOfDimension: unexpected parameters");
  return TRUE;
}

BOOLEAN ncones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      int d = zf->getAmbientDimension();
      int n = 0;

      for (int i=0; i<=d; i++)
        { n = n + zf->numberOfConesOfDimension(i,0,0); }

      res->rtyp = INT_CMD;
      res->data = (char*) n;
      return FALSE;
    }
  else
    {
      WerrorS("check_compatibility: unexpected parameters");
      return TRUE;
    }
}

BOOLEAN nmaxcones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();

      int n = 0;
      for (int d=0; d<=zf->getAmbientDimension(); d++)
        { n = n + zf->numberOfConesOfDimension(d,0,1); }

      res->rtyp = INT_CMD;
      res->data = (char*) n;
      return FALSE;
    }
  else
    {
      WerrorS("nmaxcones: unexpected parameters");
      return TRUE;
    }
}

bool iscompatible(gfan::ZFan* zf, gfan::ZCone* zc)
{
  bool b = (zf->getAmbientDimension() == zc->ambientDimension());
  if(b)
  {
    for (int d=0; d<=zf->getAmbientDimension(); d++)
    {
      for (int i=0; i<zf->numberOfConesOfDimension(d,0,1); i++)
      {
        gfan::ZCone zd = zf->getCone(d,i,0,1);
        gfan::ZCone zt = gfan::intersection(*zc,zd);
        zt.canonicalize();
        b = b && zd.hasFace(zt);
      }
    }
  }
  return b;
}

BOOLEAN isCompatible(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      bool b = iscompatible(zf,zc);
      int bb = (int) b;
      res->rtyp = INT_CMD;
      res->data = (char*) bb;
      return FALSE;
      }
    }
  WerrorS("isCompatible: unexpected parameters");
  return TRUE;
}

BOOLEAN insertCone(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->rtyp==IDHDL) && (u->e==NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      zc->canonicalize();

      leftv w=v->next;
      int n = 1;
      if ((w != NULL) && (w->Typ() == INT_CMD))
        int n = (int)(long) w;

      if (n != 0)
      {
        if (!iscompatible(zf,zc))
        {          
          WerrorS("insertCone: cone and fan not compatible");
          return TRUE;
        }
      }

      zf->insert(*zc);
      res->rtyp = NONE;
      res->data = NULL;
      IDDATA((idhdl)u->data)=(char *)zf;
      return FALSE;
    }
  }
  else
  {
    WerrorS("insertCone: unexpected parameters");
    return TRUE;
  }
}

bool containsInCollection(gfan::ZFan* zf, gfan::ZCone* zc)
{
  if((zf->getAmbientDimension() == zc->ambientDimension()))
  {
    gfan::ZVector zv=zc->getRelativeInteriorPoint();
    for (int d=0; d<=zf->getAmbientDimension(); d++)
    {
      for (int i=0; i<zf->numberOfConesOfDimension(d,0,1); i++)
      {
        gfan::ZCone zd = zf->getCone(d,i,0,1);
        zd.canonicalize();
        if (zd.containsRelatively(zv))
        {
          gfan::ZCone temp = *zc;
          temp.canonicalize();
          return (!(zd != temp));
        }
      }
    }
  }
}

BOOLEAN containsInCollection(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      res->rtyp = INT_CMD;
      res->data = (char*) (int) containsInCollection(zf,zc);
      return FALSE;
    }
  }
  // if ((u != NULL) && (u->Typ() == coneID))
  // {
  //   leftv v=u->next;
  //   if ((v != NULL) && (v->Typ() == coneID))
  //   {
  //     gfan::ZCone* zc = (gfan::ZCone*)u->Data();
  //     gfan::ZCone* zd = (gfan::ZCone*)v->Data();
  //     res->rtyp = INT_CMD;
  //     res->data = (char*) (int) hasFace(zc,zd);
  //     return FALSE;
  //   }
  // }
  WerrorS("containsInCollection: unexpected parameters");
  return TRUE;
}

// BOOLEAN coneContaining(leftv res, leftv args)
// {
//   leftv u=args;
//   if ((u != NULL) && (u->Typ() == fanID))
//   {
//     if ((v != NULL) && (v->Typ() == BIGINTMAT_CMD))
//     {
//       gfan::ZFan* zf = (gfan::ZFan*)u->Data();
//       bigintmat* vec = (bigintmat*)v->Data();
//     }
//   }
//   WerrorS("coneContaining: unexpected parameters");
//   return TRUE;
// }

BOOLEAN removeCone(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      zc->canonicalize();

      leftv w=v->next; int n = 1;
      if ((w != NULL) && (w->Typ() == INT_CMD))
        int n = (int)(long) w;

      if (n != 0)
      {
        if (!containsInCollection(zf,zc))
        {          
          WerrorS("removeCone: cone not contained in fan");
          return TRUE;
        }
      }

      zf->remove(*zc);
      res->rtyp = NONE;
      res->data = NULL;
      IDDATA((idhdl)u->data)=(char *)zf;
      return FALSE;
    }
  }
  WerrorS("removeCone: unexpected parameters");
  return TRUE;
}

BOOLEAN getCone(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      leftv w=v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        gfan::ZFan* zf = (gfan::ZFan*) u->Data();
        int d = (int)(long)v->Data();
        int i = (int)(long)w->Data();
        int o = -1;
        int m = -1;
        leftv x=w->next;
        if ((x != NULL) && (x->Typ() == INT_CMD))
        {
          o = (int)(long)x->Data();
          leftv y=x->next;
          if ((y != NULL) && (y->Typ() == INT_CMD))
          {
            m = (int)(long)y->Data();
          }
        }
        if (o == -1) o = 0;
        if (m == -1) m = 0;
        if (((o == 0) || (o == 1)) && ((m == 0) || (m == 1)))
        {
          bool oo = (bool) o;
          bool mm = (bool) m;
          if (0<=d && d<=zf->getAmbientDimension())
          {
            if (0<i && i<=zf->numberOfConesOfDimension(d,oo,mm))
            {
              i=i-1;
              int ld = zf->getLinealityDimension();
              if (d-ld>=0)
              {
                gfan::ZCone zc = zf->getCone(d-ld,i,oo,mm);
                res->rtyp = coneID;
                res->data = (char*)new gfan::ZCone(zc);
                return FALSE;
              }
              else
              {
                WerrorS("getCone: invalid dimension; no cones in this dimension");
                return TRUE;
              }
            }
            else
            {
              WerrorS("getCone: invalid index");
              return TRUE;
            }
          }
          else
          {
            WerrorS("getCone: invalid dimension");
            return TRUE;
          }
        }
        else
        {
          WerrorS("getCone: invalid specifier for orbit or maximal");
          return TRUE;
        }
      }
    }
  }
  else
  {
    WerrorS("getCone: unexpected parameters");
    return TRUE;
  }
}

BOOLEAN getCones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      int d = (int)(long)v->Data();
      int o = -1;
      int m = -1;
      leftv w=v->next;
      if ((w != NULL) && (w->Typ() == INT_CMD))
      {
        o = (int)(long)w->Data();
        leftv x=w->next;
        if ((x != NULL) && (x->Typ() == INT_CMD))
        {
          m = (int)(long)x->Data();
        }
      }
      if (o == -1) o = 0;
      if (m == -1) m = 0;
      if (((o == 0) || (o == 1)) && ((m == 0) || (m == 1)))
      {
        bool oo = (bool) o;
        bool mm = (bool) m;
        if (0<=d && d<=zf->getAmbientDimension())
        {
          int ld = zf->getLinealityDimension();
          if (d-ld>=0)
          {
            lists L = (lists)omAllocBin(slists_bin);
            int n = zf->numberOfConesOfDimension(d,oo,mm);
            L->Init(n);
            for (int i=0; i<n; i++)
            {
              gfan::ZCone zc = zf->getCone(d-ld,i,oo,mm);
              L->m[i].rtyp = coneID; L->m[i].data=(void*) new gfan::ZCone(zc);
            }
            res->rtyp = LIST_CMD;
            res->data = (char*) L;
            return FALSE;
          }
          else
          {
            WerrorS("getCones: invalid dimension; no cones in this dimension");
            return TRUE;
          }
        }
        else
        {
          WerrorS("getCones: invalid dimension");
          return TRUE;
        }
      }
      else
      {
        WerrorS("getCones: invalid specifier for orbit or maximal");
        return TRUE;
      }
    }
  }
  else
  {
    WerrorS("getCones: unexpected parameters");
    return TRUE;
  }
}

BOOLEAN isSimplicial(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    bool b=zf->isSimplicial();
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == coneID))
    {
      gfan::ZCone* zc = (gfan::ZCone*) u->Data();
      int b = isSimplicial(zc);
      res->rtyp = INT_CMD;
      res->data = (char*) b;
      return FALSE;
    }
  WerrorS("isSimplicial: unexpected parameters");
  return TRUE;
}

BOOLEAN isPure(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    bool b=zf->isPure();
    res->rtyp = INT_CMD;
    res->data = (char*) (int) b;
    return FALSE;
  }
  WerrorS("isPure: unexpected parameters");
  return TRUE;
}

BOOLEAN isComplete(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      bool b=zf->isComplete();
      res->rtyp = INT_CMD;
      res->data = (char*) (int) b;
      return FALSE;
    }
  WerrorS("isComplete: unexpected parameters");
  return TRUE;
}

BOOLEAN fVector(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      gfan::ZVector zv=zf->getFVector();
      res->rtyp = BIGINTMAT_CMD;
      res->data = (void*) zVectorToBigintmat(zv);
      return FALSE;
    }
  WerrorS("fVector: unexpected parameters");
  return TRUE;
}

gfan::ZMatrix rays(gfan::ZFan* zf)
{
  int linDim = zf->getLinealityDimension();
  gfan::ZMatrix rays(0,zf->getAmbientDimension());
  for (int i=0; i<zf->numberOfConesOfDimension(linDim+1,0,0); i++)
  {
    gfan::ZCone zc = zf->getCone(linDim+1, i, 0, 0);
    rays.append(zc.extremeRays());
  }
  return rays;
}

int numberOfConesWithVector(gfan::ZFan* zf, gfan::ZVector* v)
{
  int count = 0;
  int ambientDim = zf->getAmbientDimension();
  for (int i=0; i<zf->numberOfConesOfDimension(ambientDim, 0, 0); i++)
  {
    gfan::ZCone zc = zf->getCone(ambientDim, i, 0, 0);
    if (zc.contains(*v))
    {
      count = count +1;
      if (count > 1) 
        return count;
    }
  }
  return count;
}

BOOLEAN numberOfConesWithVector(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == BIGINTMAT_CMD))
    {
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      bigintmat* v0 = (bigintmat*) v->Data();
      int ambientDim = zf->getAmbientDimension();
      if (ambientDim != v0->cols())
      {
        WerrorS("numberOfConesWithVector: mismatching dimensions");
        return TRUE;
      }
      gfan::ZVector* v1 = bigintmatToZVector(*v0); 
      int count = numberOfConesWithVector(zf, v1);
      delete v1;
      res->rtyp = INT_CMD;
      res->data = (void*) count;
      return FALSE;
    }
  }
  WerrorS("numberOfConesWithVector: unexpected parameters");
  return TRUE;
}

BOOLEAN fanFromString(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == STRING_CMD))
    {
      std::string fanInString = (char*) u->Data();
      std::istringstream s(fanInString);
      gfan::ZFan* zf = new gfan::ZFan(s);
      res->rtyp = fanID;
      res->data = (void*) zf;
      return FALSE;
    }
  WerrorS("fanFromString: unexpected parameters");
  return TRUE;
}

BOOLEAN fanViaCones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    lists L = (lists) u->Data();
    if (lSize(L)>-1)
    {
      if (L->m[0].Typ() != coneID)
      {
        WerrorS("fanViaCones: list contains entries of wrong type");
        return TRUE;
      }
      gfan::ZCone* zc = (gfan::ZCone*) L->m[0].Data();
      gfan::ZFan* zf = new gfan::ZFan(zc->ambientDimension());
      zf->insert(*zc);
      for (int i=1; i<=lSize(L); i++)
      {
        if (L->m[i].Typ() != coneID)
        {
          WerrorS("fanViaCones: entries of wrong type in list");
          return TRUE;
        }
        gfan::ZCone* zc = (gfan::ZCone*) L->m[i].Data();
        if (zc->ambientDimension() != zf->getAmbientDimension())
        {
          WerrorS("fanViaCones: inconsistent ambient dimensions amongst cones in list");
          return TRUE;
        }
        zf->insert(*zc);
      }
      res->rtyp = fanID;
      res->data = (void*) zf;
      return FALSE;
    }
    res->rtyp = fanID;
    res->data = (void*) new gfan::ZFan(0);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    gfan::ZFan* zf = new gfan::ZFan(zc->ambientDimension());
    zf->insert(*zc);
    while (u->next != NULL)
    {
      u = u->next;
      if (u->Typ() != coneID)
      {
        WerrorS("fanViaCones: arguments of wrong type");
        return TRUE;
      }
      gfan::ZCone* zc = (gfan::ZCone*) u->Data();
      if (zc->ambientDimension() != zf->getAmbientDimension())
      {
        WerrorS("fanViaCones: inconsistent ambient dimensions amongst input cones");
        return TRUE;
      }
      zf->insert(*zc);
    }
    res->rtyp = fanID;
    res->data = (void*) zf;
    return FALSE;
  }
  if (u == NULL)
  {
    res->rtyp = fanID;
    res->data = (void*) new gfan::ZFan(0);
    return FALSE;
  }
  WerrorS("fanViaCones: unexpected parameters");
  return TRUE;
}

static gfan::ZCone subcone(const lists cones, const gfan::ZVector point)
{
  gfan::ZCone sigma = gfan::ZCone(gfan::ZMatrix(1,point.size()), gfan::ZMatrix(1,point.size()));
  gfan::ZCone* zc;
  for (int i=0; i<=lSize(cones); i++)
  {
    zc = (gfan::ZCone*) cones->m[i].Data();
    if (zc->contains(point))
      sigma = gfan::intersection(sigma,*zc);
  }
  return(sigma);
}

static gfan::ZVector randomPoint(const gfan::ZCone &zc)
{
  gfan::ZMatrix rays = zc.extremeRays();
  gfan::ZVector rp = gfan::ZVector(zc.ambientDimension());
  for (int i=0; i<rays.getHeight(); i++)
  {
    rp = rp + siRand() * rays[i];
  }
  return rp;
}

static lists listOfInteriorFacets(const gfan::ZCone &zc, const gfan::ZCone &bound)
{
  gfan::ZMatrix inequalities = zc.getFacets();
  gfan::ZMatrix equations = zc.getImpliedEquations();
  lists L = (lists) omAllocBin(slists_bin);
  int r = inequalities.getHeight();
  int c = inequalities.getWidth();
  L->Init(r);

  int index = -1;
  /* next we iterate over each of the r facets, build the respective cone and add it to the list */
  /* this is the i=0 case */
  gfan::ZMatrix newInequalities = inequalities.submatrix(1,0,r,c);
  gfan::ZMatrix newEquations = equations; 
  newEquations.appendRow(inequalities[0]);

  gfan::ZCone* eta = new gfan::ZCone(newInequalities,newEquations);
  eta->canonicalize();
  gfan::ZVector* v = new gfan::ZVector(eta->getRelativeInteriorPoint());
  gfan::ZVector* w = new gfan::ZVector(inequalities[0]);

  if (bound.containsRelatively(*v))
  {
    // std::cout << "interior facet: " << std::endl;
    // std::cout << eta->getInequalities() << std::endl;
    // std::cout << eta->getEquations() << std::endl;
    index++;
    lists l = (lists) omAllocBin(slists_bin);
    l->Init(3);
    l->m[0].rtyp = coneID; 
    l->m[0].data = (void*) eta;
    l->m[1].rtyp = coneID; 
    l->m[1].data = (void*) v;
    l->m[2].rtyp = coneID; 
    l->m[2].data = (void*) w;
    // std::cout << "storing interiorPoint: " << *v << std::endl;
    L->m[index].rtyp = LIST_CMD; 
    L->m[index].data = (void*) l;
    // l->nr = -1; l->m = NULL;
    eta = NULL;
    v = NULL;
  }
  else
  {
    delete eta, v;
  }
  
  /* these are the cases i=1,...,r-2 */
  for (int i=1; i<r-1; i++)
  {
    newInequalities = inequalities.submatrix(0,0,i,c);
    newInequalities.append(inequalities.submatrix(i+1,0,r,c));
    newEquations = equations;
    newEquations.appendRow(inequalities[i]);
    eta = new gfan::ZCone(newInequalities,newEquations);
    eta->canonicalize();
    v = new gfan::ZVector(eta->getRelativeInteriorPoint());
    // v = new gfan::ZVector(eta->getUniquePoint());
    w = new gfan::ZVector(inequalities[i]);
    if (bound.containsRelatively(*v))
    {    
      // std::cout << "contained in relative interior" << std::endl;
      // std::cout << "interior facet: " << std::endl;
      // std::cout << eta->getInequalities() << std::endl;
      // std::cout << eta->getEquations() << std::endl;
      index++;
      lists l = (lists) omAllocBin(slists_bin);
      l->Init(3);
      l->m[0].rtyp = coneID; 
      l->m[0].data = (void*) eta;
      l->m[1].rtyp = coneID; 
      l->m[1].data = (void*) v;
      l->m[2].rtyp = coneID; 
      l->m[2].data = (void*) w;
      // std::cout << "storing interiorPoint: " << *v << std::endl;
      L->m[index].rtyp = LIST_CMD; 
      L->m[index].data = (void*) l;
      // l->nr = -1; l->m = NULL;
      eta = NULL;
      v = NULL;
    }
    else
    {
      delete eta, v, w;
    }
  }
  
  /* this is the i=r-1 case */
  newInequalities = inequalities.submatrix(0,0,r-1,c);
  newEquations = equations;
  newEquations.appendRow(inequalities[r-1]);
  eta = new gfan::ZCone(newInequalities,newEquations);
  eta->canonicalize();

  v = new gfan::ZVector(eta->getRelativeInteriorPoint());
  // v = new gfan::ZVector(eta->getUniquePoint());
  w = new gfan::ZVector(inequalities[r-1]);
  if (bound.containsRelatively(*v))
  {    
    // std::cout << "interior facet: " << std::endl;
    // std::cout << eta->getInequalities() << std::endl;
    // std::cout << eta->getEquations() << std::endl;
    // std::cout << "contained in relative interior" << std::endl;
    index++;
    lists l = (lists) omAllocBin(slists_bin);
    l->Init(3);
    l->m[0].rtyp = coneID; 
    l->m[0].data = (void*) eta;
    l->m[1].rtyp = coneID; 
    l->m[1].data = (void*) v;
    l->m[2].rtyp = coneID; 
    l->m[2].data = (void*) w;
    // std::cout << "storing interiorPoint: " << *v << std::endl;
    L->m[index].rtyp = LIST_CMD; 
    L->m[index].data = (void*) l;
    // l->nr = -1; l->m = NULL;
    eta = NULL;
    v = NULL;
  }
  else
  {
    delete eta, v;
  }

  if (index==-1)
  {
    L->m[0].rtyp = INT_CMD;         // when there are no interior facets
    L->m[0].data = (void*) (int) 0; // we just write an integer as zeroth entry
    return L;
  }
  if (index!=L->nr)
  {
    L->m = (leftv)omRealloc(L->m,(index+1)*sizeof(sleftv));
    L->nr = index;
  }
  return L;
}

/***
 * delete the i-th entry of ul
 **/
static void deleteEntry(lists ul, int i)
{
  lists cache = (lists) ul->m[i].Data();
  gfan::ZVector* v = (gfan::ZVector*) cache->m[1].Data();
  delete v;
  cache->m[1].rtyp = 0;
  cache->m[1].data = NULL;
  gfan::ZVector* w = (gfan::ZVector*) cache->m[2].Data();
  delete w;
  cache->m[2].rtyp = 0;
  cache->m[2].data = NULL;
  cache->nr = 0;
  cache = NULL;

  ul->m[i].CleanUp();
}

static void swap(intvec* v, int i, int j)
{
  int k = (*v)[j];
  (*v)[j] = (*v)[i];
  (*v)[i] = k;
}

static void gnomeSort(intvec* v)
{
  int i=1;
  while (i<v->length())
  {
    if ((*v)[i-1] <= (*v)[i])
      ++i;
    else
    {
      swap(v,i-1,i);
 
      if (i > 1)
        --i;
    }
  }
}

static int findLastUsableEntry(lists ul, int start)
{
  int i = start;
  while (ul->m[i].data == NULL)
  {
    --i;
  }
  return i;
}

/***
 * Here, ul and vl ar lists of lists of facets and their relative interior point.
 * The list ul might be empty sometimes, however vl always contains at least one element.
 * This functions appends vl to ul with the following constraint:
 * Should a facet in vl already exist in ul, then both are to be deleted.
 * The merged list will be stored in ul.
 **/
static void mergeListsOfFacets(lists ul, lists vl)
{
  int ur = ul->nr;
  int vr = vl->nr;
  // std::cout << "ul->nr: " << ur << std::endl;
  // std::cout << "vl->nr: " << vr << std::endl;

  /***
   * First we cover the trivial cases with ul being empty
   * Here we can delete ul entirely and make it point to the content of vl
   * vl can be set to MULL.
   **/
  if (ur == -1)
  {
    ul->Clean();
    ul = vl;
    vl = NULL;
  }
  else
  {
    /***
     * We will know go through all vectors q in vl and compare them to all vectors p in ul.
     * If p and q coincide, we will delete q and p right away.
     * If q does not coincide with any p, we will mark it as to be preserved.
     * Two intvecs will keep track of entries in ul that have been deleted 
     * and all entries of vl that must be preserved.
     * countUl and countVl keep track of the number of elements to be deleted or preserved.
     * The reason why we count j up and i down is because the first entry of vl
     * tends to be equal the last entry of ul, due to the way vl and ul are created.
     **/ 
    intvec* entriesOfUlToBeFilled = new intvec(vr+1);
    intvec* entriesOfVlToBePreserved = new intvec(vr+1);
    int countUl = 0;
    int countVl = 0;

    gfan::ZVector* q = NULL;
    gfan::ZVector* p = NULL;
    bool toBePreserved = 1;
    lists cache0;
    lists cache1;
    for (int j=0; j<=vr; j++)
    {
      toBePreserved = 1;
      cache0 = (lists) vl->m[j].Data();
      q = (gfan::ZVector*) cache0->m[1].Data();
      // std::cout << "checking for: " << *q << std::endl;
      for (int i=ur; i>=0; i--)
      {
        if (ul->m[i].data != NULL) // in case that entry of ul was deleted earlier
        {
          cache1 = (lists) ul->m[i].Data();
          p = (gfan::ZVector*) cache1->m[1].Data();
          // std::cout << "checking: " << *p << std::endl;
          if ((*q) == (*p))
          {
            // std::cout << "                          duplicate: " << *q << std::endl;
            // std::cout << "deleting vl entry " << j << " and ul entry " << i << std::endl;
            deleteEntry(vl,j);
            deleteEntry(ul,i);
            // std::cout << "writing index " << i << " in entry " << countUl << " of toBeFilled" 
            //           << std::endl;
            (*entriesOfUlToBeFilled)[countUl] = i;
            countUl++; 
            toBePreserved = 0; 
            break;
          }
          cache1 = NULL;
        }
      }
      if (toBePreserved)
      {
        // std::cout << "new facet: " << *q << std::endl;
        // std::cout << "preserving vl entry " << j << std::endl;
        (*entriesOfVlToBePreserved)[countVl] = j;
        countVl++;
      }
      cache0 = NULL;
    }
 
    /***
     * In case the resulting merged list is empty,
     * we are finished now.
     **/
    if (ur+1-countUl+countVl == 0)
    {
      vl->nr = -1;
      vl->Clean();
      ul->nr = -1;
      ul->Clean();
      ul = NULL;
      return;
    }

    /***
     * We fill the holes in ul, the algorithm works as follows:
     * - repeat the following until it breaks or all holes are fixed:
     *   - determine the first empty entry to be filled
     *   - determine the last usable entry
     *   - if firstEmptyEntry < lastUsableEntry, fill
     *   - else break
     * (If it breaks that means that the remaining empty slots need not to be filled)
     * Note that entriesOfUlToBeFilled is unordered.
     **/
    entriesOfUlToBeFilled->resize(countUl);
    gnomeSort(entriesOfUlToBeFilled);
    int i = 0;
    int firstEmptyEntry = (*entriesOfUlToBeFilled)[i];
    int lastUsableEntry = findLastUsableEntry(ul,ur); 
    while (1)
    {
      if (firstEmptyEntry < lastUsableEntry)
      {
        // std::cout << "filling ul entry " << firstEmptyEntry << " with ul entry " << lastUsableEntry 
        //           << std::endl;
        ul->m[firstEmptyEntry] = ul->m[lastUsableEntry];
        ul->m[lastUsableEntry].rtyp = 0;
        ul->m[lastUsableEntry].data = NULL; 
        lastUsableEntry = findLastUsableEntry(ul,lastUsableEntry);
      }
      else
        break;

      if (i < countUl-1)
      {
        ++i; firstEmptyEntry = (*entriesOfUlToBeFilled)[i];
      }
      else
        break;
    }
 
    /***
     * We resize ul accordingly
     * and append the to be preserved entries of vl to it.
     **/
    int k;
    // std::cout << "resizing ul to: " << ur+1-countUl+countVl << std::endl;
    ul->m = (leftv) omRealloc0(ul->m, (ur+1-countUl+countVl)*sizeof(sleftv));  
    ul->nr = ur-countUl+countVl;
    for (int j=0; j<countVl; j++)
    {
      k = (*entriesOfVlToBePreserved)[j];
      // std::cout << "writing vl entry " << k << " into ul entry: " << lastUsableEntry+j+1 << std::endl;
      ul->m[lastUsableEntry+j+1] = vl->m[k];
      vl->m[k].rtyp = 0;
      vl->m[k].data = NULL;

      // cache0 = (lists) ul->m[lastEntry+1].Data();
      // gfan::ZCone* eta = (gfan::ZVector*) cache0->m[0].Data();
      // std::cout << "new facet: " << std::endl;
      // std::cout << eta->getInequalities() << std::endl;
    }

    /***
     * Now we delete of vl which is empty by now,
     * because its elements have either been deleted or moved to ul.
     **/
    vl->nr = -1;
    vl->Clean();
    
    delete entriesOfUlToBeFilled, entriesOfVlToBePreserved;
  }
}

/***
 * delete last entry of ul
 **/
static void deleteLast(lists ul)
{
  int ur = ul->nr;
  
  lists cache = (lists) ul->m[ur].Data();
  gfan::ZVector* v = (gfan::ZVector*) cache->m[1].Data();
  delete v;
  cache->m[1].rtyp = 0;
  cache->m[1].data = NULL;
  cache->nr = 0;
  cache = NULL;

  ul->m[ur].CleanUp();
  ul->nr = ur-1;
  if (ur > 0)
    ul->m = (leftv) omRealloc(ul->m, ur*sizeof(sleftv));
  else
    ul->m = NULL;
}

BOOLEAN refineCones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == LIST_CMD))
  {
    leftv v=u->next;
    if ((u != NULL) && (v->Typ() == BIGINTMAT_CMD))
    {
      lists cones = (lists) u->Data();
      bigintmat* bim = (bigintmat*) v->Data();
      bim = bim->transpose();
      gfan::ZMatrix* zm = bigintmatToZMatrix(bim);
      gfan::ZCone support = gfan::ZCone::givenByRays(*zm, gfan::ZMatrix(0, zm->getWidth()));
      delete zm;

      /***
       * Randomly compute a first full-dimensional cone and insert it into the fan.
       * Compute a list of facets and relative interior points.
       * The relative interior points are unique, assuming the cone is stored in canonical form,
       * which is the case in our algorithm, as we supply no redundant inequalities.
       * Hence we can decide whether a facet need to be traversed by crosschecking
       * its relative interior point with this list.
       **/
      gfan::ZCone lambda; gfan::ZVector point;
      // std::cout << "starting to compute initial cone!" << std::endl;
      do
      {
        // std::cout << "computing randomPoint!" << std::endl;
        point = randomPoint(support);
        // std::cout << "randomPoint: " << point << std::endl;
        // std::cout << "computing subcone!" << std::endl;
        lambda = subcone(cones, point);
      }
      while (lambda.dimension() < lambda.ambientDimension());

      // std::cout << "initial cone: " << std::endl;
      // std::cout << lambda.getFacets() << std::endl;
      lambda.canonicalize();
      gfan::ZFan* Sigma = new gfan::ZFan(lambda.ambientDimension());
      Sigma->insert(lambda);
      lists interiorFacets = listOfInteriorFacets(lambda, support);
      if (interiorFacets->m[0].rtyp == INT_CMD)
      {
        res->rtyp = fanID;
        res->data = (void*) Sigma;
        return FALSE;
      }
      int mu = 1000; int iterationNumber = 1;

      gfan::ZCone* eta; gfan::ZVector* interiorPoint; gfan::ZVector* facetNormal;
      while (interiorFacets->nr > -1)
      {
        std::cout << "cones found: " << iterationNumber++ << std::endl;
        // if (iterationNumber == 26)
        //   return TRUE;
        // std::cout << "facets left to check: " << interiorFacets->nr + 1 << std::endl;
        /***
         * Extract a facet to traverse and its relative interior point.
         **/
        lists cache = (lists) interiorFacets->m[interiorFacets->nr].Data();
        eta = (gfan::ZCone*) cache->m[0].Data();
        interiorPoint = (gfan::ZVector*) cache->m[1].Data();
        facetNormal = (gfan::ZVector*) cache->m[2].Data(); 
        // std::cout << "going over facet with interiorPoint: " << *interiorPoint << std::endl;
        // std::cout << "and with facetNormal: " << *facetNormal << std::endl;
        // std::cout << "going over facet: " << std::endl;
        // std::cout << eta->getFacets() << std::endl;
        // std::cout << eta->getEquations() << std::endl;

        /***
         * construct a point, which lies on the other side of the facet. 
         * make sure it lies in the known support of our fan
         * and that the cone around the point is maximal, containing eta.
         **/
        point = mu * (*interiorPoint) - (*facetNormal);
        while (!support.containsRelatively(point))
        {
          mu = mu * 10;
          point = mu * (*interiorPoint) - (*facetNormal);
        }

        lambda = subcone(cones,point);
        while ((lambda.dimension() < lambda.ambientDimension()) && !(lambda.contains(*interiorPoint)))
        {
          mu = mu * 10;
          point = mu * (*interiorPoint) - (*facetNormal);
          lambda = subcone(cones,point);
        }

        // std::cout << "maximal cone: " << std::endl;
        // std::cout << lambda.getFacets() << std::endl;
        /*** 
         * insert lambda into Sigma, and create a list of facets of lambda. 
         * merge the two lists of facets
         **/
        lambda.canonicalize();
        Sigma->insert(lambda);
        lists newFacets = listOfInteriorFacets(lambda, support);
        if (newFacets->m[0].rtyp == INT_CMD)  
        { // this means there are no facets and points to be added.
          // we delete newFacets alltogether and the last entry of interiorFacets
          newFacets->Clean();
          deleteLast(interiorFacets);
        }
        else
          mergeListsOfFacets(interiorFacets, newFacets);

        if (interiorFacets == NULL)
          break;

        eta = NULL;
        interiorPoint = NULL;
        facetNormal = NULL;
        cache = NULL;
      }
      std::cout << "cones found: " << iterationNumber++ << std::endl;
      res->rtyp = fanID;
      res->data = (void*) Sigma;
      return FALSE;
    }
  }
  WerrorS("refineCones: unexpected parameters");
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
    res->data = (char*) L;
    return FALSE;
  }
  WerrorS("listOfFacets: unexpected parameters");
  return TRUE;
}

// BOOLEAN grFan(leftv res, leftv h)
// {
//   /*======== GFAN ==============*/
//   /*
//    WILL HAVE TO CHANGE RETURN TYPE TO LIST_CMD
//   */
//   /*
//     heuristic:
//     0 = keep all Groebner bases in memory
//     1 = write all Groebner bases to disk and read whenever necessary
//     2 = use a mixed heuristic, based on length of Groebner bases
//   */
//   if( h!=NULL && h->Typ()==IDEAL_CMD && h->next!=NULL && h->next->Typ()==INT_CMD)
//   {
//     int heuristic;
//     heuristic=(int)(long)h->next->Data();
//     ideal I=((ideal)h->Data());
//     #ifndef USE_ZFAN
//         #define USE_ZFAN
//     #endif
//     #ifndef USE_ZFAN
//     res->rtyp=LIST_CMD; //res->rtyp=coneID; res->data(char*)zcone;
//     res->data=(lists) grfan(I,heuristic,FALSE);
//     #else
//     res->rtyp=fanID;
//     res->data=(void*)(grfan(I,heuristic,FALSE));
//     #endif
//     return FALSE;
//   }
//   else
//   {
//     WerrorS("Usage: grfan(<ideal>,<int>)");
//     return TRUE;
//   }
// }
  //Possibility to have only one Groebner cone computed by specifying a weight vector FROM THE RELATIVE INTERIOR!
  //Needs wp as ordering!
//   if(strcmp(sys_cmd,"grcone")==0)
//   {
//     if(h!=NULL && h->Typ()==IDEAL_CMD && h->next!=NULL && h->next->Typ()==INT_CMD)
//     {
//       ideal I=((ideal)h->Data());
//       res->rtyp=LIST_CMD;
//       res->data=(lists)grcone_by_intvec(I);
//     }
//   }


void bbfan_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefule,
  // all other are simply error messages
  b->blackbox_destroy=bbfan_destroy;
  b->blackbox_String=bbfan_String;
  //b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=bbfan_Init;
  b->blackbox_Copy=bbfan_Copy;
  b->blackbox_Assign=bbfan_Assign;
  iiAddCproc("","emptyFan",FALSE,emptyFan);
  iiAddCproc("","fullFan",FALSE,fullFan);
  /* the following functions are implemented in bbcone.cc */
  // iiAddCproc("","containsInSupport",FALSE,containsInSupport);
  // iiAddCproc("","getAmbientDimension",FALSE,getAmbientDimension);
  // iiAddCproc("","getCodimension",FALSE,getDimension);
  // iiAddCproc("","getDimension",FALSE,getDimension);
  // iiAddCproc("","getLinealityDimension",FALSE,getLinealityDimension);
  /********************************************************/
  iiAddCproc("","isCompatible",FALSE,isCompatible);
  iiAddCproc("","numberOfConesOfDimension",FALSE,numberOfConesOfDimension);
  iiAddCproc("","ncones",FALSE,ncones);
  iiAddCproc("","nmaxcones",FALSE,nmaxcones);
  iiAddCproc("","insertCone",FALSE,insertCone);
  iiAddCproc("","removeCone",FALSE,removeCone);
  iiAddCproc("","getCone",FALSE,getCone);
  iiAddCproc("","getCones",FALSE,getCones);
  iiAddCproc("","isSimplicial",FALSE,isSimplicial);
  iiAddCproc("","isPure",FALSE,isPure);
  iiAddCproc("","fanFromString",FALSE,fanFromString);
  iiAddCproc("","fanViaCones",FALSE,fanViaCones);
  iiAddCproc("","numberOfConesWithVector",FALSE,numberOfConesWithVector);
  // iiAddCproc("","isComplete",FALSE,isComplete);  not working as expected, should leave this to polymake
  iiAddCproc("","fVector",FALSE,fVector);
  iiAddCproc("","containsInCollection",FALSE,containsInCollection);
  iiAddCproc("","refineCones",FALSE,refineCones);
  // iiAddCproc("","grFan",FALSE,grFan);
  fanID=setBlackboxStuff(b,"fan");
  //Print("created type %d (fan)\n",fanID);
}

#endif
/* HAVE_FANS */
