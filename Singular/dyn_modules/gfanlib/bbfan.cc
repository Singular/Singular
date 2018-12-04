#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "misc/intvec.h"
#include "coeffs/coeffs.h"
#include "coeffs/bigintmat.h"

#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/blackbox.h"

#include "Singular/links/ssiLink.h"

#include "callgfanlib_conversion.h"
#include "bbfan.h"
#include "gfan.h"

int fanID;

void* bbfan_Init(blackbox* /*b*/)
{
  return (void*) new gfan::ZFan(0);
}

void bbfan_destroy(blackbox* /*b*/, void *d)
{
  if (d!=NULL)
  {
    gfan::ZFan* zf = (gfan::ZFan*) d;
    delete zf;
  }
}

char* bbfan_String(blackbox* /*b*/, void *d)
{
  if (d==NULL) return omStrDup("invalid object");
  else
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)d;
    std::string s = zf->toString(2+4+8+128);
    gfan::deinitializeCddlibIfRequired();
    return omStrDup(s.c_str());
// =======
//     std::stringstream s;
//     std::string raysAndCones = zf->toStringJustRaysAndMaximalCones();
//     s << raysAndCones;
//     if (zf->getDimension() >= 0) // <=> zf is not empty
//     {
//       assert(zf->numberOfConesOfDimension(zf->getDimension()-zf->getLinealityDimension(),0,0));
//       gfan::ZCone zc = zf->getCone(zf->getDimension()-zf->getLinealityDimension(),0,0,0);
//       gfan::ZMatrix genLinSpace = zc.generatorsOfLinealitySpace();
//       char* gens = toString(genLinSpace);
//       s << std::endl << "GENERATORS_LINEALITY_SPACE:" << std::endl;
//       s << gens;
//     }
//     std::string sstring = s.str();
//     return omStrDup(sstring.c_str());
// >>>>>>> status updated 11.03.
  }
}

void* bbfan_Copy(blackbox* /*b*/, void *d)
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
      gfan::ZFan* zd = (gfan::ZFan*) l->Data();
      delete zd;
    }
    newZf = new gfan::ZFan(0);
  }
  else if (r->Typ()==l->Typ())
  {
    if (l->Data()!=NULL)
    {
      gfan::ZFan* zd = (gfan::ZFan*) l->Data();
      delete zd;
    }
    newZf = (gfan::ZFan*) r->CopyD();
  }
  else if (r->Typ()==INT_CMD)
  {
    int ambientDim = (int) (long) r->Data();
    if (ambientDim < 0)
    {
      Werror("expected an int >= 0, but got %d", ambientDim);
      return TRUE;
    }
    if (l->Data()!=NULL)
    {
      gfan::ZFan* zd = (gfan::ZFan*) l->Data();
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
    IDDATA((idhdl)l->data) = (char*) newZf;
  }
  else
  {
    l->data = (void*) newZf;
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
  bigintmat* ivCopy = new bigintmat(rr, cc, coeffs_BIGINT);
  number temp1 = n_Init(1,coeffs_BIGINT);
  for (int r = 1; r <= rr; r++)
    for (int c = 1; c <= cc; c++)
    {
      number temp2 = n_Sub(IMATELEM(*iv, r, c),temp1,coeffs_BIGINT);
      ivCopy->set(r,c,temp2);
      n_Delete(&temp2,coeffs_BIGINT);
    }
  n_Delete(&temp1,coeffs_BIGINT);
  gfan::ZMatrix* zm = bigintmatToZMatrix(ivCopy);
  gfan::IntMatrix im = gfan::IntMatrix(gfan::ZToIntMatrix(*zm));
  delete zm;
  return im;
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
  res->data = (void*)(new gfan::ZFan(ambientDim));
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
  res->data = (void*)(new gfan::ZFan(sg));
  return FALSE;
}

BOOLEAN emptyFan(leftv res, leftv args)
{
  leftv u = args;
  if (u == NULL)
  {
    res->rtyp = fanID;
    res->data = (void*) new gfan::ZFan(0);
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
  res->data = (void*) zf;
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
  res->data = (void*) zf;
  return FALSE;
}

BOOLEAN fullFan(leftv res, leftv args)
{
  gfan::initializeCddlibIfRequired();
  leftv u = args;
  if (u == NULL)
  {
    res->rtyp = fanID;
    res->data = (void*) new gfan::ZFan(0);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == INT_CMD))
    if (u->next == NULL) return jjFANFULL_I(res, u);
  if ((u != NULL) && (u->Typ() == BIGINTMAT_CMD))
    if (u->next == NULL) return jjFANFULL_IM(res, u);
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
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      int d = (int)(long)v->Data();
      d -= zf->getLinealityDimension(); // shifting by dimension of lineality space
      // since gfan always regards the lineality space as zero-dimensional
      int m = 0;
      leftv w=v->next;
      if ((w != NULL) && (w->Typ() != INT_CMD))
      {
        WerrorS("numberOfConesOfDimension: invalid maximality flag");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      if ((w != NULL) && (w->Typ() == INT_CMD))
        m = (int)(long) w->Data();
      bool mm = (bool) m;

      if (d<0 || (d>zf->getAmbientDimension()-zf->getLinealityDimension()))
      {
        WerrorS("numberOfConesOfDimension: invalid dimension");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }

      int n = zf->numberOfConesOfDimension(d,0,mm);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) n;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
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
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();
    int d = zf->getAmbientDimension();
    int n = 0;

    for (int i=0; i<=d; i++)
      n = n + zf->numberOfConesOfDimension(i,0,0);

    res->rtyp = INT_CMD;
    res->data = (void*) (long) n;
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("ncones: unexpected parameters");
  return TRUE;
}

BOOLEAN nmaxcones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*)u->Data();

    int n = 0;
    for (int d=0; d<=zf->getAmbientDimension(); d++)
      n = n + zf->numberOfConesOfDimension(d,0,1);

    res->rtyp = INT_CMD;
    res->data = (void*) (long) n;
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("nmaxcones: unexpected parameters");
  return TRUE;
}

bool isCompatible(const gfan::ZFan* zf, const gfan::ZCone* zc)
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
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      int b = isCompatible(zf,zc);
      res->rtyp = INT_CMD;
      res->data = (void*) (long) b;
      gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      zc->canonicalize();

      leftv w=v->next;
      int n=1;
      if ((w != NULL) && (w->Typ() == INT_CMD))
        n = (int)(long) w->Data();

      if (n != 0)
      {
        if (!isCompatible(zf,zc))
        {
          WerrorS("insertCone: cone and fan not compatible");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
      }

      zf->insert(*zc);
      res->rtyp = NONE;
      res->data = NULL;
      IDDATA((idhdl)u->data) = (char*) zf;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  WerrorS("insertCone: unexpected parameters");
  return TRUE;
}

bool containsInCollection(gfan::ZFan* zf, gfan::ZCone* zc)
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
  return 0;
}

BOOLEAN containsInCollection(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == coneID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      if((zf->getAmbientDimension() == zc->ambientDimension()))
      {
        res->rtyp = INT_CMD;
        res->data = (void*) (long) (int) containsInCollection(zf,zc);
        gfan::deinitializeCddlibIfRequired();
        return FALSE;
      }
      gfan::deinitializeCddlibIfRequired();
      WerrorS("containsInCollection: mismatching ambient dimensions");
      return TRUE;
    }
  }
  WerrorS("containsInCollection: unexpected parameters");
  return TRUE;
}

// BOOLEAN coneContaining(leftv res, leftv args)
// {
//   leftv u=args;
//   if ((u != NULL) && (u->Typ() == fanID))
//   {
//     leftv v=u->next;
//     if ((v != NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
//     {
//       gfan::ZFan* zf = (gfan::ZFan*)u->Data();
//       gfan::ZVector* point;
//       if (v->Typ() == INTVEC_CMD)
//       {
//         intvec* w0 = (intvec*) v->Data();
//         bigintmat* w1 = iv2bim(w0,coeffs_BIGINT);
//         w1->inpTranspose();
//         point = bigintmatToZVector(*w1);
//         delete w1;
//       }
//       else
//       {
//         bigintmat* w1 = (bigintmat*) v->Data();
//         point = bigintmatToZVector(*w1);
//       }
//       lists L = (lists)omAllocBin(slists_bin);
//       res->rtyp = LIST_CMD;
//       res->data = (void*) L;
//       delete point;
//       return FALSE;
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
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*)u->Data();
      gfan::ZCone* zc = (gfan::ZCone*)v->Data();
      zc->canonicalize();

      leftv w=v->next; int n = 1;
      if ((w != NULL) && (w->Typ() == INT_CMD))
        n = (int)(long) w;

      if (n != 0)
      {
        if (!containsInCollection(zf,zc))
        {
          WerrorS("removeCone: cone not contained in fan");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
      }

      zf->remove(*zc);
      res->rtyp = NONE;
      res->data = NULL;
      IDDATA((idhdl)u->data) = (char*) zf;
      gfan::deinitializeCddlibIfRequired();
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
        gfan::initializeCddlibIfRequired();
        gfan::ZFan* zf = (gfan::ZFan*) u->Data();

        int d = (int)(long)v->Data();
        d -= zf->getLinealityDimension(); // shifting by dimension of lineality space
        // since gfan always regards the lineality space as zero-dimensional
        int i = (int)(long)w->Data();
        i -= 1; // shifting by one, since gfan starts counting at 0
        int m = 0;
        leftv x=w->next;
        if ((x != NULL) && (x->Typ() != INT_CMD))
        {
          WerrorS("getCone: invalid maximality flag");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        if ((x != NULL) && (x->Typ() == INT_CMD))
          m = (int)(long) x->Data();
        bool mm = (bool) m;

        if (d<0 || (d>zf->getAmbientDimension()-zf->getLinealityDimension()))
        {
          WerrorS("getCone: invalid dimension");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        if (i<0 || i>=zf->numberOfConesOfDimension(d,0,mm))
        {
          WerrorS("getCone: invalid index");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }

        gfan::ZCone zc = zf->getCone(d,i,0,mm);
        res->rtyp = coneID;
        res->data = (void*)new gfan::ZCone(zc);
        gfan::deinitializeCddlibIfRequired();
        return FALSE;
      }
    }
  }
  WerrorS("getCone: unexpected parameters");
  return TRUE;
}

BOOLEAN getCones(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == INT_CMD))
    {
      gfan::initializeCddlibIfRequired();
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
            int n = zf->numberOfConesOfDimension(d-ld,oo,mm);
            L->Init(n);
            for (int i=0; i<n; i++)
            {
              gfan::ZCone zc = zf->getCone(d-ld,i,oo,mm);
              L->m[i].rtyp = coneID; L->m[i].data=(void*) new gfan::ZCone(zc);
            }
            res->rtyp = LIST_CMD;
            res->data = (void*) L;
            gfan::deinitializeCddlibIfRequired();
            return FALSE;
          }
          else
          {
            WerrorS("getCones: invalid dimension; no cones in this dimension");
            gfan::deinitializeCddlibIfRequired();
            return TRUE;
          }
        }
        else
        {
          WerrorS("getCones: invalid dimension");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
      }
      else
      {
        WerrorS("getCones: invalid specifier for orbit or maximal");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
    }
  }
  WerrorS("getCones: unexpected parameters");
  return TRUE;
}

int isSimplicial(gfan::ZFan* zf)
{
  int i = zf->isSimplicial() ? 1 : 0;
  return i;
}

BOOLEAN isPure(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    int b = zf->isPure();
    res->rtyp = INT_CMD;
    res->data = (void*) (long) b;
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("isPure: unexpected parameters");
  return TRUE;
}

// BOOLEAN isComplete(leftv res, leftv args)
// {
//   leftv u=args;
//   if ((u != NULL) && (u->Typ() == fanID))
//   {
//     gfan::ZFan* zf = (gfan::ZFan*) u->Data();
//     int b = zf->isComplete();
//     res->rtyp = INT_CMD;
//     res->data = (void*) (long) b;
//     return FALSE;
//   }
//   WerrorS("isComplete: unexpected parameters");
//   return TRUE;
// }

BOOLEAN fVector(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZFan* zf = (gfan::ZFan*) u->Data();
    gfan::ZVector zv=zf->getFVector();
    res->rtyp = BIGINTMAT_CMD;
    res->data = (void*) zVectorToBigintmat(zv);
    gfan::deinitializeCddlibIfRequired();
    return FALSE;
  }
  WerrorS("fVector: unexpected parameters");
  return TRUE;
}

gfan::ZMatrix rays(const gfan::ZFan* const zf)
{
  gfan::ZMatrix rays(0,zf->getAmbientDimension());
  for (int i=0; i<zf->numberOfConesOfDimension(1,0,0); i++)
  {
    gfan::ZCone zc = zf->getCone(1, i, 0, 0);
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
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      bigintmat* v0 = (bigintmat*) v->Data();
      int ambientDim = zf->getAmbientDimension();
      if (ambientDim != v0->cols())
      {
        WerrorS("numberOfConesWithVector: mismatching dimensions");
        gfan::deinitializeCddlibIfRequired();
        return TRUE;
      }
      gfan::ZVector* v1 = bigintmatToZVector(*v0);
      int count = numberOfConesWithVector(zf, v1);
      delete v1;
      res->rtyp = INT_CMD;
      res->data = (void*) (long) count;
      gfan::deinitializeCddlibIfRequired();
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
    gfan::initializeCddlibIfRequired();
    std::string fanInString = (char*) u->Data();
    std::istringstream s(fanInString);
    gfan::ZFan* zf = new gfan::ZFan(s);
    res->rtyp = fanID;
    res->data = (void*) zf;
    gfan::deinitializeCddlibIfRequired();
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
      gfan::initializeCddlibIfRequired();
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
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        gfan::ZCone* zc = (gfan::ZCone*) L->m[i].Data();
        if (zc->ambientDimension() != zf->getAmbientDimension())
        {
          WerrorS("fanViaCones: inconsistent ambient dimensions amongst cones in list");
          gfan::deinitializeCddlibIfRequired();
          return TRUE;
        }
        zf->insert(*zc);
      }
      res->rtyp = fanID;
      res->data = (void*) zf;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
    res->rtyp = fanID;
    res->data = (void*) new gfan::ZFan(0);
    return FALSE;
  }
  if ((u != NULL) && (u->Typ() == coneID))
  {
    gfan::initializeCddlibIfRequired();
    gfan::ZCone* zc = (gfan::ZCone*) u->Data();
    gfan::ZFan* zf = new gfan::ZFan(zc->ambientDimension());
    zf->insert(*zc);
    while (u->next != NULL)
    {
      u = u->next;
      if (u->Typ() != coneID)
      {
        WerrorS("fanViaCones: arguments of wrong type");
        gfan::deinitializeCddlibIfRequired();
        delete zf;
        return TRUE;
      }
      gfan::ZCone* zc = (gfan::ZCone*) u->Data();
      if (zc->ambientDimension() != zf->getAmbientDimension())
      {
        WerrorS("fanViaCones: inconsistent ambient dimensions amongst input cones");
        gfan::deinitializeCddlibIfRequired();
        delete zf;
        return TRUE;
      }
      zf->insert(*zc);
    }
    res->rtyp = fanID;
    res->data = (void*) zf;
    gfan::deinitializeCddlibIfRequired();
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


// BOOLEAN tropicalVariety(leftv res, leftv args)
// {
//   leftv u=args;
//   if ((u != NULL) && (u->Typ() == POLY_CMD))
//   {
//     int n = rVar(currRing);
//     gfan::ZFan* zf = new gfan::ZFan(n);
//     int* expv1 = (int*)omAlloc((n+1)*sizeof(int));
//     int* expv2 = (int*)omAlloc((n+1)*sizeof(int));
//     int* expvr = (int*)omAlloc((n+1)*sizeof(int));
//     gfan::ZVector expw1 = gfan::ZVector(n);
//     gfan::ZVector expw2 = gfan::ZVector(n);
//     gfan::ZVector expwr = gfan::ZVector(n);
//     gfan::ZMatrix eq, ineq;
//     for (poly s1=(poly)u->Data(); s1!=NULL; pIter(s1))
//     {
//       pGetExpV(s1,expv1);
//       expw1 = intStar2ZVector(n,expv1);
//       for (poly s2=pNext(s1); s2!=NULL; pIter(s2))
//       {
//         pGetExpV(s2,expv2);
//         expw2 = intStar2ZVector(n,expv2);
//         eq = gfan::ZMatrix(0,n);
//         eq.appendRow(expw1-expw2);
//         ineq = gfan::ZMatrix(0,n);
//         for (poly r=(poly)u->Data(); r!=NULL; pIter(r))
//         {
//           pGetExpV(r,expvr);
//           expwr = intStar2ZVector(n,expvr);
//           if ((r!=s1) && (r!=s2))
//           {
//             ineq.appendRow(expw1-expwr);
//           }
//         }
//         gfan::ZCone zc = gfan::ZCone(ineq,eq);
//         zf->insert(zc);
//       }
//     }
//     omFreeSize(expv1,(n+1)*sizeof(int));
//     omFreeSize(expv2,(n+1)*sizeof(int));
//     omFreeSize(expvr,(n+1)*sizeof(int));
//     res->rtyp = fanID;
//     res->data = (void*) zf;
//     return FALSE;
//   }
//   WerrorS("tropicalVariety: unexpected parameters");
//   return TRUE;
// }

gfan::ZFan commonRefinement(gfan::ZFan zf, gfan::ZFan zg)
{
  assume(zf.getAmbientDimension() == zg.getAmbientDimension());

  // gather all maximal cones of f and g
  std::list<gfan::ZCone> maximalConesOfF;
  for (int d=0; d<=zf.getAmbientDimension(); d++)
    for (int i=0; i<zf.numberOfConesOfDimension(d,0,1); i++)
      maximalConesOfF.push_back(zf.getCone(d,i,0,1));

  std::list<gfan::ZCone> maximalConesOfG;
  for (int d=0; d<=zg.getAmbientDimension(); d++)
    for (int i=0; i<zg.numberOfConesOfDimension(d,0,1); i++)
      maximalConesOfG.push_back(zg.getCone(d,i,0,1));

  // construct a new fan out of their intersections
  gfan::ZFan zr = gfan::ZFan(zf.getAmbientDimension());
  for (std::list<gfan::ZCone>::iterator itf=maximalConesOfF.begin();
       itf != maximalConesOfF.end(); itf++)
    for (std::list<gfan::ZCone>::iterator itg=maximalConesOfG.begin();
         itg != maximalConesOfG.end(); itg++)
      zr.insert(intersection(*itf,*itg));

  return zr;
}

BOOLEAN commonRefinement(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
  {
    leftv v=u->next;
    if ((v != NULL) && (v->Typ() == fanID))
    {
      gfan::initializeCddlibIfRequired();
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      gfan::ZFan* zg = (gfan::ZFan*) v->Data();
      gfan::ZFan* zr = new gfan::ZFan(commonRefinement(*zf,*zg));
      res->rtyp = fanID;
      res->data = (void*) zr;
      gfan::deinitializeCddlibIfRequired();
      return FALSE;
    }
  }
  WerrorS("commonRefinement: unexpected parameters");
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
//     res->rtyp=LIST_CMD; //res->rtyp=coneID; res->data(void*)zcone;
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


BOOLEAN bbfan_serialize(blackbox *b, void *d, si_link f)
{
  ssiInfo *dd = (ssiInfo *)f->data;

  sleftv l;
  memset(&l,0,sizeof(l));
  l.rtyp=STRING_CMD;
  l.data=(void*)"fan";
  f->m->Write(f, &l);

  gfan::ZFan* zf = (gfan::ZFan*) d;
  std::string s = zf->toString(2+4+8+128);

  fprintf(dd->f_write,"%d %s ",(int)s.size(),s.c_str());

  return FALSE;
}


BOOLEAN bbfan_deserialize(blackbox **b, void **d, si_link f)
{
  ssiInfo *dd = (ssiInfo *)f->data;

  int l = s_readint(dd->f_read);
  char* buf = (char*) omAlloc0(l+1);
  (void) s_getc(dd->f_read); // skip whitespace
  (void) s_readbytes(buf,l,dd->f_read);
  buf[l]='\0';

  std::istringstream fanInString(std::string(buf,l));
  gfan::ZFan* zf = new gfan::ZFan(fanInString);
  *d=zf;

  omFree(buf);
  return FALSE;
}


void bbfan_setup(SModulFunctions* p)
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
  b->blackbox_serialize=bbfan_serialize;
  b->blackbox_deserialize=bbfan_deserialize;
  p->iiAddCproc("gfan.lib","emptyFan",FALSE,emptyFan);
  p->iiAddCproc("gfan.lib","fullFan",FALSE,fullFan);
  /* the following functions are implemented in bbcone.cc */
  // iiAddCproc("gfan.lib","containsInSupport",FALSE,containsInSupport);
  // iiAddCproc("gfan.lib","getAmbientDimension",FALSE,getAmbientDimension);
  // iiAddCproc("gfan.lib","getCodimension",FALSE,getDimension);
  // iiAddCproc("gfan.lib","getDimension",FALSE,getDimension);
  // iiAddCproc("gfan.lib","getLinealityDimension",FALSE,getLinealityDimension);
  // iiAddCproc("gfan.lib","isSimplicial",FALSE,isSimplicial);
  /********************************************************/
  p->iiAddCproc("gfan.lib","isCompatible",FALSE,isCompatible);
  p->iiAddCproc("gfan.lib","numberOfConesOfDimension",FALSE,numberOfConesOfDimension);
  p->iiAddCproc("gfan.lib","ncones",FALSE,ncones);
  p->iiAddCproc("gfan.lib","nmaxcones",FALSE,nmaxcones);
  p->iiAddCproc("gfan.lib","insertCone",FALSE,insertCone);
  p->iiAddCproc("gfan.lib","removeCone",FALSE,removeCone);
  p->iiAddCproc("gfan.lib","getCone",FALSE,getCone);
  p->iiAddCproc("gfan.lib","getCones",FALSE,getCones);
  p->iiAddCproc("gfan.lib","isPure",FALSE,isPure);
  p->iiAddCproc("gfan.lib","fanFromString",FALSE,fanFromString);
  p->iiAddCproc("gfan.lib","fanViaCones",FALSE,fanViaCones);
  p->iiAddCproc("gfan.lib","numberOfConesWithVector",FALSE,numberOfConesWithVector);
  // iiAddCproc("gfan.lib","isComplete",FALSE,isComplete);  not working as expected, should leave this to polymake
  p->iiAddCproc("gfan.lib","fVector",FALSE,fVector);
  p->iiAddCproc("gfan.lib","containsInCollection",FALSE,containsInCollection);
  // p->iiAddCproc("gfan.lib","tropicalVariety",FALSE,tropicalVariety);
  p->iiAddCproc("gfan.lib","commonRefinement",FALSE,commonRefinement);
  // iiAddCproc("gfan.lib","grFan",FALSE,grFan);
  fanID=setBlackboxStuff(b,"fan");
  //Print("created type %d (fan)\n",fanID);
}

#endif
// gfan::ZFan commonRefinementCompleteFans(const gfan::ZFan &zf, const gfan::ZFan &zg)
// {
//   assume(zf->getAmbientDimension() == zg->getAmbientDimension());

//   gfan::ZFan zfg = gfan::ZFan(zf->getAmbientDimension());
//   int d = zf->getAmbientDimension();
//   for (int i=0; i<zf->numberOfConesOfDimension(d,0,1); i++)
//     for (int j=0; j<zg->numberOfConesOfDimension(d,0,1); j++)
//     {
//       gfan::ZCone zc = intersection(zf->getCone(d,i,0,1),zg->getCone(d,j,0,1));
//       if (zc.dimension()==d) zgf.insert(zc);
//     }

//   return zfg;
// }
