#include <Singular/mod2.h>
#ifdef HAVE_FANS

#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <kernel/intvec.h>
#include <sstream>
#include <gfanlib/gfanlib.h>

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
   intmat; 0 otherwise */
static gfan::IntMatrix permutationIntMatrix(const intvec* iv)
{
        int cc = iv->cols();
        int rr = iv->rows();
        intvec* ivCopy = new intvec(rr, cc, 0);
        for (int r = 1; r <= rr; r++)
          for (int c = 1; c <= cc; c++)
            IMATELEM(*ivCopy, r, c) = IMATELEM(*iv, r, c) - 1;
        gfan::ZMatrix zm = intmat2ZMatrix(ivCopy);
        gfan::IntMatrix* im = new gfan::IntMatrix(gfan::ZToIntMatrix(zm));
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
  intvec* permutations = (intvec*)v->Data();
  int ambientDim = permutations->cols();
  gfan::IntMatrix im = permutationIntMatrix(permutations);
  if (!gfan::Permutation::arePermutations(im))
  {
    Werror("provided intmat contains invalid permutations of {1, ..., %d}", ambientDim);
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
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
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
  intvec* permutations = (intvec*)v->Data();
  int ambientDim = permutations->cols();
  gfan::IntMatrix im = permutationIntMatrix(permutations);
  if (!gfan::Permutation::arePermutations(im))
  {
    Werror("provided intmat contains invalid permutations of {1, ..., %d}", ambientDim);
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
  if ((u != NULL) && (u->Typ() == INTMAT_CMD))
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
                  if ((d <= zf->getAmbientDimension()) && ((o == 0) || (o == 1)) && ((m == 0) || (m == 1)))
                    {
                      bool oo = (bool) o;
                      bool mm = (bool) m;
                      int n = zf->numberOfConesOfDimension(d,oo,mm);
                      res->rtyp = INT_CMD;
                      res->data = (char*) n;
                      return FALSE;
                    }
                }
            }
        }
    }
  WerrorS("getAmbientDimension: unexpected parameters");
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

BOOLEAN quickInsertCone(leftv res, leftv args)
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
      zf->insert(*zc);
      res->rtyp = NONE;
      res->data = NULL;
      return FALSE;
    }
  }
  WerrorS("quickInsertCone: unexpected parameters");
  return TRUE;
}

BOOLEAN insertCone(leftv res, leftv args)
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
      if (iscompatible(zf,zc))
        {
          zf->insert(*zc);
          res->rtyp = NONE;
          res->data = NULL;
          return FALSE;
        }
      else
        {
          WerrorS("insertCone: cone and fan not compatible");
          return TRUE;
        }
    }
  }
  else
  {
    WerrorS("insertCone: unexpected parameters");
    return TRUE;
  }
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
      res->rtyp = INT_CMD;
      res->data = (char*) (int) containsInSupport(zc,zd);
      return FALSE;
    }
    if ((v != NULL) && (v->Typ() == INTVEC_CMD))
    {
      gfan::ZCone* zc = (gfan::ZCone*)u->Data();
      intvec* vec = (intvec*)v->Data();
      res->rtyp = INT_CMD;
      res->data = (char*) (int) containsInSupport(zc,vec);
      return FALSE;
    }
  }
  WerrorS("containsInSupport: unexpected parameters");
  return TRUE;
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
//     if ((v != NULL) && (v->Typ() == INTVEC_CMD))
//     {
//       gfan::ZFan* zf = (gfan::ZFan*)u->Data();
//       intvec* vec = (intvec*)v->Data();
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
      if(containsInCollection(zf,zc))
      {
        zf->remove(*zc);
        res->rtyp = NONE;
        res->data = NULL;
        return FALSE;
      }
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
        leftv x=w->next;
        if ((x != NULL) && (x->Typ() == INT_CMD))
        {
          leftv y=w->next;
          if ((y != NULL) && (y->Typ() == INT_CMD))
          {
            gfan::ZFan* zf = (gfan::ZFan*) u->Data();
            int d = (int)(long)v->Data();
            int i = (int)(long)w->Data();
            int o = (int)(long)x->Data();
            int m = (int)(long)y->Data();
            if (((o == 0) || (o == 1)) && ((m == 0) || (m == 1)))
            {
              bool oo = (bool) o;
              bool mm = (bool) m;
              if (d<=zf->getAmbientDimension())
              {
                if (i<=zf->numberOfConesOfDimension(d,oo,mm))
                {
                  i=i-1;
                  gfan::ZCone zc = zf->getCone(d,i,oo,mm);
                  res->rtyp = coneID;
                  res->data = (char*)new gfan::ZCone(zc);
                  return FALSE;
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
    }
  }
  else
  {
    WerrorS("getCone: unexpected parameters");
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

BOOLEAN getFVector(leftv res, leftv args)
{
  leftv u=args;
  if ((u != NULL) && (u->Typ() == fanID))
    {
      gfan::ZFan* zf = (gfan::ZFan*) u->Data();
      gfan::ZVector zv=zf->getFVector();
      res->rtyp = INTVEC_CMD;
      res->data = (void*) zVector2Intvec(zv);
      return FALSE;
    }
  WerrorS("getFVector: unexpected parameters");
  return TRUE;
}

BOOLEAN grFan(leftv res, leftv h)
{
  /*======== GFAN ==============*/
  /*
   WILL HAVE TO CHANGE RETURN TYPE TO LIST_CMD
  */
  /*
    heuristic:
    0 = keep all Groebner bases in memory
    1 = write all Groebner bases to disk and read whenever necessary
    2 = use a mixed heuristic, based on length of Groebner bases
  */
  if( h!=NULL && h->Typ()==IDEAL_CMD && h->next!=NULL && h->next->Typ()==INT_CMD)
  {
    int heuristic;
    heuristic=(int)(long)h->next->Data();
    ideal I=((ideal)h->Data());
    #ifndef USE_ZFAN
        #define USE_ZFAN
    #endif
    #ifndef USE_ZFAN
    res->rtyp=LIST_CMD; //res->rtyp=coneID; res->data(char*)zcone;
    res->data=(lists) grfan(I,heuristic,FALSE);
    #else
    res->rtyp=fanID;
    res->data=(void*)(grfan(I,heuristic,FALSE));
    #endif
    return FALSE;
  }
  else
  {
    WerrorS("Usage: grfan(<ideal>,<int>)");
    return TRUE;
  }
}
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
  // iiAddCproc("","getAmbientDimension",FALSE,getAmbientDimension);
  // iiAddCproc("","getCodimension",FALSE,getDimension);
  // iiAddCproc("","getDimension",FALSE,getDimension);
  // iiAddCproc("","getLinealityDimension",FALSE,getLinealityDimension);
  /********************************************************/
  iiAddCproc("","isCompatible",FALSE,isCompatible);
  iiAddCproc("","numberOfConesOfDimension",FALSE,numberOfConesOfDimension);
  iiAddCproc("","ncones",FALSE,ncones);
  iiAddCproc("","nmaxcones",FALSE,nmaxcones);
  iiAddCproc("","quickInsertCone",FALSE,quickInsertCone);
  iiAddCproc("","insertCone",FALSE,insertCone);
  iiAddCproc("","removeCone",FALSE,removeCone);
  iiAddCproc("","getCone",FALSE,getCone);
  iiAddCproc("","isSimplicial",FALSE,isSimplicial);
  iiAddCproc("","isPure",FALSE,isPure);
  iiAddCproc("","isComplete",FALSE,isComplete);
  iiAddCproc("","getFVector",FALSE,getFVector);
  iiAddCproc("","containsInCollection",FALSE,containsInCollection);
  iiAddCproc("","containsInSupport",FALSE,containsInSupport);
  iiAddCproc("","grFan",FALSE,grFan);
  fanID=setBlackboxStuff(b,"fan");
  //Print("created type %d (fan)\n",fanID);
}

#endif
/* HAVE_FANS */
