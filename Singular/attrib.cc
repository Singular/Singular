/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*!
!

* ABSTRACT: attributes to leftv and idhdl


*/

#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "polys/matpol.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/attrib.h"

static omBin sattr_bin = omGetSpecBin(sizeof(sattr));

void sattr::Print()
{
  omCheckAddrSize(this,sizeof(sattr));
  ::Print("attr:%s, type %s \n",name,Tok2Cmdname(atyp));
  if (next!=NULL) next->Print();
}

attr sattr::Copy()
{
  assume (this!=NULL);

  omCheckAddrSize(this,sizeof(sattr));
  attr n=(attr)omAlloc0Bin(sattr_bin);
  n->atyp=atyp;
  if (name!=NULL) n->name=omStrDup(name);
  n->data=CopyA();
  if (next!=NULL)
  {
    n->next=next->Copy();
  }
  return n;
}

// in subexr.cc:
//void * sattr::CopyA()
//{
//  omCheckAddrSize(this,sizeof(sattr));
//  return s_internalCopy(atyp,data);
//}

static void attr_free(attr h, const ring r=currRing)
{
  if (h->data!=NULL) /*!
!
avoid assume failure 

*/
  {
    s_internalDelete(h->atyp,h->data,r);
    h->data=NULL;
    omFree(h->name);
    h->name=NULL;
  }
}

attr sattr::set(char * s, void * d, int t)
{
  attr h = get(s);
  attr result=this;
  if (h!=NULL)
  {
    attr_free(h);
  }
  else
  {
    h = (attr)omAlloc0Bin(sattr_bin);
    h->next = this;
    result=h;
  }
  h->name = s;
  h->data = d;
  h->atyp = t;
#ifdef TEST
  //::Print("set attr >>%s<< of type %s\n",h->name, Tok2Cmdname(t));
#endif
  return  result;
}

attr sattr::get(const char * s)
{
  attr h = this;
  while (h!=NULL)
  {
    if (0 == strcmp(s,h->name))
    {
#ifdef TEST
      //::Print("get attr >>%s<< of type %s\n",h->name, Tok2Cmdname(h->atyp));
#endif
      return h;
    }
    h = h->next;
  }
  return NULL;
}

#if 0
void * atGet(idhdl root,const char * name)
{
  attr temp = root->attribute->get(name);
  if (temp!=NULL)
    return temp->data;
  else
    return NULL;
}

void * atGet(leftv root,const char * name)
{
  attr temp;
  attr a=*(root->Attribute());
  temp = a->get(name);
  if (temp!=NULL)
    return temp->data;
  else
    return NULL;
}
#endif

void * atGet(idhdl root,const char * name, int t, void *defaultReturnValue)
{
  attr temp = root->attribute->get(name);
  if ((temp!=NULL) && (temp->atyp==t))
    return temp->data;
  else
    return defaultReturnValue;
}

void * atGet(leftv root,const char * name, int t)
{
  attr *a=(root->Attribute());
  if (a!=NULL)
  {
    attr temp = (*a)->get(name);
    if ((temp!=NULL) && (temp->atyp==t))
      return temp->data;
  }
  return NULL;
}

void atSet(idhdl root, char * name,void * data,int typ)
{
  if (root!=NULL)
  {
    if ((IDTYP(root)!=RING_CMD)
    && (!RingDependend(IDTYP(root)))&&(RingDependend(typ)))
      WerrorS("cannot set ring-dependend objects at this type");
    else
      root->attribute=root->attribute->set(name,data,typ);
  }
}

void atSet(leftv root, char * name,void * data,int typ)
{
  if (root!=NULL)
  {
    attr *a=root->Attribute();
    int rt=root->Typ();
    if (a==NULL)
      WerrorS("cannot set attributes of this object");
    else if ((rt!=RING_CMD)
    && (!RingDependend(rt))&&(RingDependend(typ)))
      WerrorS("cannot set ring-dependend objects at this type");
    else
    {
      *a=(*a)->set(name,data,typ);
    }
  }
}

void sattr::kill(const ring r)
{
  attr_free(this,r);
  omFreeBin((ADDRESS)this, sattr_bin);
}

void sattr::killAll(const ring r)
{
  attr temp = this,temp1;

  while (temp!=NULL)
  {
    temp1 = temp->next;
    omCheckAddr(temp);
    temp->kill(r);
    temp = temp1;
  }
}

void at_Kill(idhdl root,const char * name, const ring r)
{
  attr temp = root->attribute->get(name);
  if (temp!=NULL)
  {
    attr N = temp->next;
    attr temp1 = root->attribute;
    if (temp1==temp)
    {
      root->attribute = N;
    }
    else
    {
      while (temp1->next!=temp) temp1 = temp1->next;
      temp1->next = N;
    }
    temp->kill(r);
  }
}

void at_KillAll(idhdl root, const ring r)
{
  root->attribute->killAll(r);
  root->attribute = NULL;
}

void at_KillAll(leftv root, const ring r)
{
  root->attribute->killAll(r);
  root->attribute = NULL;
}

BOOLEAN atATTRIB1(leftv res,leftv v)
{
  attr *aa=(v->Attribute());
  if (aa==NULL)
  {
    WerrorS("this object cannot have attributes");
    return TRUE;
  }
  attr a=*aa;
  BOOLEAN haveNoAttribute=TRUE;
  if (v->e==NULL)
  {
    if (hasFlag(v,FLAG_STD))
    {
      PrintS("attr:isSB, type int\n");
      haveNoAttribute=FALSE;
    }
    if (hasFlag(v,FLAG_QRING))
    {
      PrintS("attr:qringNF, type int\n");
      haveNoAttribute=FALSE;
    }
    if (v->Typ()==RING_CMD)
    {
      PrintS("attr:cf_class, type int\n");
      PrintS("attr:global, type int\n");
      PrintS("attr:maxExp, type int\n");
      PrintS("attr:ring_cf, type int\n");
      #ifdef HAVE_SHIFTBBA
      PrintS("attr:isLetterplaceRing, type int\n");
      #endif

      haveNoAttribute=FALSE;
    }
  }
  else
  {
    leftv at=v->LData();
    return atATTRIB1(res,at);
  }
  if (a!=NULL)                    a->Print();
  else  if(haveNoAttribute)       PrintS("no attributes\n");
  return FALSE;
}
BOOLEAN atATTRIB2(leftv res,leftv v,leftv b)
{
  char *name=(char *)b->Data();
  int t=v->Typ();
  leftv at=NULL;
  if (v->e!=NULL)
    at=v->LData();
  if (strcmp(name,"isSB")==0)
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)hasFlag(v,FLAG_STD);
    if (at!=NULL) res->data=(void *)(long)(hasFlag(v,FLAG_STD)||(hasFlag(at,FLAG_STD)));
  }
  else if ((strcmp(name,"rank")==0)&&(/*!
!
v->Typ()

*/t==MODUL_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(((ideal)v->Data())->rank);
  }
  else if ((strcmp(name,"global")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(((ring)v->Data())->OrdSgn==1);
  }
  else if ((strcmp(name,"maxExp")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)(((ring)v->Data())->bitmask/2);
  }
  else if ((strcmp(name,"ring_cf")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)(rField_is_Ring((ring)v->Data()));
  }
  else if ((strcmp(name,"cf_class")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    res->rtyp=INT_CMD;
    coeffs cf;
    if (t==RING_CMD) cf=((ring)v->Data())->cf;
    else             cf=(coeffs)v->Data();
    res->data=(void *)(long)(cf->type);
  }
  else if (strcmp(name,"qringNF")==0)
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)hasFlag(v,FLAG_QRING);
    if (at!=NULL) res->data=(void *)(long)(hasFlag(v,FLAG_QRING)||(hasFlag(at,FLAG_QRING)));
  }
#ifdef HAVE_SHIFTBBA
  else if ((strcmp(name,"isLetterplaceRing")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)(((ring)v->Data())->isLPring);
  }
#endif
  else
  {
    attr *aa=v->Attribute();
    if (aa==NULL)
    {
      WerrorS("this object cannot have attributes");
      return TRUE;
    }
    attr a=*aa;
    a=a->get(name);
    if (a!=NULL)
    {
      res->rtyp=a->atyp;
      res->data=a->CopyA();
    }
    else
    {
      res->rtyp=STRING_CMD;
      res->data=omStrDup("");
    }
  }
  return FALSE;
}
BOOLEAN atATTRIB3(leftv /*!
!
res

*/,leftv v,leftv b,leftv c)
{
  idhdl h=(idhdl)v->data;
  if (v->e!=NULL)
  {
    v=v->LData();
    if (v==NULL) return TRUE;
    h=NULL;
  }
  else if (v->rtyp!=IDHDL) h=NULL;
  int t=v->Typ();

  char *name=(char *)b->Data();
  if (strcmp(name,"isSB")==0)
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attribute isSB must be int");
      return TRUE;
    }
    if (((long)c->Data())!=0L)
    {
      if (h!=NULL) setFlag(h,FLAG_STD);
      setFlag(v,FLAG_STD);
    }
    else
    {
      if (h!=NULL) resetFlag(h,FLAG_STD);
      resetFlag(v,FLAG_STD);
    }
  }
  else if (strcmp(name,"qringNF")==0)
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attribute qringNF must be int");
      return TRUE;
    }
    if (((long)c->Data())!=0L)
    {
      if (h!=NULL) setFlag(h,FLAG_QRING);
      setFlag(v,FLAG_QRING);
    }
    else
    {
      if (h!=NULL) resetFlag(h,FLAG_QRING);
      resetFlag(v,FLAG_QRING);
    }
  }
  else if ((strcmp(name,"rank")==0)&&(/*!
!
v->Typ()

*/t==MODUL_CMD))
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attribute `rank` must be int");
      return TRUE;
    }
    ideal I=(ideal)v->Data();
    int rk=id_RankFreeModule(I,currRing);
    I->rank=si_max(rk,(int)((long)c->Data()));
  }
  else if (((strcmp(name,"global")==0)
    || (strcmp(name,"cf_class")==0)
    || (strcmp(name,"ring_cf")==0)
    || (strcmp(name,"maxExp")==0))
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    Werror("can not set attribute `%s`",name);
    return TRUE;
  }
#ifdef HAVE_SHIFTBBA
  else if ((strcmp(name,"isLetterplaceRing")==0)
  &&(/*!
!
v->Typ()

*/t==RING_CMD))
  {
    if (c->Typ()==INT_CMD)
      ((ring)v->Data())->isLPring=(int)(long)c->Data();
    else
    {
      WerrorS("attribute `isLetterplaceRing` must be int");
      return TRUE;
    }
  }
#endif
  else
  {
    int typ=c->Typ();
    if (h!=NULL) atSet(h,omStrDup(name),c->CopyD(typ),typ/*!
!
c->T(yp()

*/);
    else         atSet(v,omStrDup(name),c->CopyD(typ),typ/*!
!
c->T(yp()

*/);
  }
  return FALSE;
}

BOOLEAN atKILLATTR1(leftv /*!
!
res

*/,leftv a)
{
  idhdl h=NULL;
  if ((a->rtyp==IDHDL)&&(a->e==NULL))
  {
    h=(idhdl)a->data;
    resetFlag((idhdl)a->data,FLAG_STD);
  }
  resetFlag(a,FLAG_STD);
  if (h->attribute!=NULL)
  {
    atKillAll(h);
    a->attribute=NULL;
  }
  else atKillAll(a);
  return FALSE;
}
BOOLEAN atKILLATTR2(leftv /*!
!
res

*/,leftv a,leftv b)
{
  if ((a->rtyp!=IDHDL)||(a->e!=NULL))
  {
    WerrorS("object must have a name");
    return TRUE;
  }
  char *name=(char *)b->Data();
  if (strcmp(name,"isSB")==0)
  {
    resetFlag(a,FLAG_STD);
    resetFlag((idhdl)a->data,FLAG_STD);
  }
  else if (strcmp(name,"global")==0)
  {
    WerrorS("can not set attribut `global`");
    return TRUE;
  }
  else
  {
    atKill((idhdl)a->data,name);
  }
  return FALSE;
}

