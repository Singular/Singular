/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: attrib.cc,v 1.11 1999-03-16 15:33:04 Singular Exp $ */

/*
* ABSTRACT: attributes to leftv and idhdl
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "polys.h"
#include "ideals.h"
#include "matpol.h"
#include "ipshell.h"
#include "attrib.h"

void sattr::Print()
{
  mmTestP(this,sizeof(sattr));
  ::Print("attr:%s, type %s \n",name,Tok2Cmdname(atyp));
  if (next!=NULL) next->Print();
}

attr sattr::Copy()
{
  mmTestP(this,sizeof(sattr));
  attr n=(attr)Alloc0(sizeof(sattr));
  n->atyp=atyp;
  if (name!=NULL) n->name=mstrdup(name);
  n->data=CopyA();
  if (next!=NULL)
  {
    n->next=next->Copy();
  }
  return n;
}

void * sattr::CopyA()
{
  mmTestP(this,sizeof(sattr));
  switch (atyp)
  {
    case INTVEC_CMD:
      return (void *)ivCopy((intvec *)data);
    case MATRIX_CMD:
      return (void *)mpCopy((matrix)data);
    case IDEAL_CMD:
    case MODUL_CMD:
      return (void *)idCopy((ideal)data);
    case POLY_CMD:
    case VECTOR_CMD:
      return (void *)pCopy((poly)data);
    case INT_CMD:
      return (void *)data;
    case STRING_CMD:
      return (void *)mstrdup((char *)data);
#ifdef TEST
    default:
      ::Print("CopyA: unknown type %d\n",atyp);  /* DEBUG */
#endif
  }
  return NULL;
}

attr sattr::set(char * s, void * data, int t)
{
  attr h = get(s);
  if (h!=NULL)
  {
    switch (h->atyp)
    {
    case INTVEC_CMD:
      delete (intvec *)h->data;
      break;
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
      idDelete((ideal *)&h->data);
      break;
    case POLY_CMD:
    case VECTOR_CMD:
      pDelete((poly *)&h->data);
      break;
    case INT_CMD:
      break;
    case STRING_CMD:
      FreeL((ADDRESS)h->data);
      break;
#ifdef TEST
    default:
      ::Print("at-set: unknown type\n",atyp);  /* DEBUG */
#endif
    } /* end switch: (atyp) */
    FreeL((ADDRESS)s);
  }
  else
  {
     h = (attr)Alloc0(sizeof(sattr));
     h->name = s;
     h->next = this;
     h->data = data;
     h->atyp = t;
     return  h;
  }
  //::Print("set attr >>%s<< of type %d\n",h->name,t);
  h->data = data;
  h->atyp = t;
  return  this;
}

attr sattr::get(char * s)
{
  attr h = this;
  while (h!=NULL)
  {
    if (0 == strcmp(s,h->name)) return h;
    h = h->next;
  }
  return NULL;
}

void * atGet(idhdl root,char * name)
{
  attr temp = root->attribute->get(name);
  if (temp!=NULL)
    return temp->data;
  else
    return NULL;
}

void * atGet(leftv root,char * name)
{
  attr temp = root->attribute->get(name);
  if ((temp==NULL) && (root->rtyp==IDHDL))
  {
    idhdl h=(idhdl)root->data;
    temp=h->attribute->get(name);
  }
  if (temp!=NULL)
    return temp->data;
  else
    return NULL;
}

void atSet(idhdl root,char * name,void * data,int typ)
{
  if (root!=NULL)
  {
    root->attribute=root->attribute->set(name,data,typ);
  }
}

void atSet(leftv root,char * name,void * data,int typ)
{
  if (root!=NULL)
  {
    if (root->e!=NULL)
    {
      Werror("object must have a name for attrib %s",name);
    }
    else
    {
      if (root->rtyp==IDHDL)
      {
        idhdl h=(idhdl)root->data;
        h->attribute=h->attribute->set(name,data,typ);
        root->attribute=h->attribute;
      }
      else
      {
        root->attribute=root->attribute->set(name,data,typ);
      }
    }
  }
}

void sattr::kill()
{
  FreeL((ADDRESS)name);
  name=NULL;
  switch (atyp)
  {
  case INTVEC_CMD:
    delete (intvec *)data;
    break;
  case IDEAL_CMD:
  case MODUL_CMD:
  case MATRIX_CMD:
    idDelete((ideal *)&data);
    break;
  case POLY_CMD:
  case VECTOR_CMD:
    pDelete((poly *)&data);
    break;
  case INT_CMD:
    break;
  case STRING_CMD:
    FreeL((ADDRESS)data);
    break;
#ifdef TEST
  default:
    ::Print("atKill: unknown type\n",atyp);  /* DEBUG */
#endif
  } /* end switch: (atyp) */
  data=NULL;
  Free((ADDRESS)this,sizeof(sattr));
}

void sattr::killAll()
{
  attr temp = this,temp1;

  while (temp!=NULL)
  {
    temp1 = temp->next;
    temp->kill();
    temp = temp1;
  }
}

void atKill(idhdl root,char * name)
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
    temp->kill();
  }
}

void atKillAll(idhdl root)
{
  root->attribute->killAll();
  root->attribute = NULL;
}

BOOLEAN atATTRIB1(leftv res,leftv a)
{
  leftv v=a;
  if (a->e!=NULL)
  {
    v=a->LData();
    if (v==NULL) return TRUE;
  }
  attr at=v->attribute;
  if (v->Typ()==PROC_CMD)
  {
    procinfo *p=(procinfo *)v->Data();
    if (p->trace_flag!=0)
      Print("trace:%d\n",p->trace_flag);
  }    
  if (hasFlag(v,FLAG_STD))
  {
    PrintS("attr:isSB, type int\n");
    if (at!=NULL) at->Print();
  }
  else
  {
    if (at!=NULL) at->Print();
    else          PrintS("no attributes\n");
  }
  return FALSE;
}
BOOLEAN atATTRIB2(leftv res,leftv a,leftv b)
{
  char *name=(char *)b->Data();
  leftv v=a;
  if (a->e!=NULL)
  {
    v=a->LData();
    if (v==NULL) return TRUE;
  }
  if (strcmp(name,"isSB")==0)
  {
    res->rtyp=INT_CMD;
    res->data=(void *)hasFlag(v,FLAG_STD);
  }
  else if ((strcmp(name,"rank")==0)&&(v->Typ()==MODUL_CMD))
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(((ideal)v->Data())->rank);
  }
  else if ((v->Typ()==PROC_CMD) && (strcmp(name,"trace")==0))
  {
    procinfo *p=(procinfo *)v->Data();
    res->rtyp=INT_CMD;
    res->data=(void *)p->trace_flag;
  }    
  else
  {
    attr at=v->attribute->get(name);
    if (at!=NULL)
    {
      res->rtyp=at->atyp;
      res->data=at->CopyA();
    }
    else
    {
      res->rtyp=STRING_CMD;
      res->data=mstrdup("");
    }
  }
  return FALSE;
}
BOOLEAN atATTRIB3(leftv res,leftv a,leftv b,leftv c)
{
  idhdl h=(idhdl)a->data;
  leftv v=a;
  if (a->e!=NULL)
  {
    v=a->LData();
    if (v==NULL) return TRUE;
    h=NULL;
  }
  attr *at=&(v->attribute);
  char *name=(char *)b->Data();
  if (strcmp(name,"isSB")==0)
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attrib isSB must be int");
      return TRUE;
    }
    if (((int)c->Data())!=0)
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
  else if ((strcmp(name,"rank")==0)&&(v->Typ()==MODUL_CMD))
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attrib `rank` must be int");
      return TRUE;
    }
    ideal I=(ideal)v->Data();
    I->rank=max(I->rank,(int)c->Data());
  }
  else if ((strcmp(name,"trace")==0)&&(v->Typ()==PROC_CMD))
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attrib `trace` must be int");
      return TRUE;
    }
    procinfo *p=(procinfo *)v->Data();
    p->trace_flag=(int)c->Data();
  }    
#ifdef DRING
  else if (strcmp(name,"D")==0)
  {
    if (c->Typ()!=INT_CMD)
    {
      WerrorS("attrib `D` must be int");
      return TRUE;
    }
    switch (v->Typ())
    {
      case POLY_CMD:
      case VECTOR_CMD:
        pdSetDFlagP((poly)v->Data(),(int)c->Data());
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
        {
          ideal I=(ideal)v->Data();
          int i=IDELEMS(I)-1;
          int cc=(int)c->Data();
          while (i>=0) { pdSetDFlagP(I->m[i],cc); i--; }
          break;
        }
      default:
        WerrorS("cannot set attrib `D` for this type");
    }
  }
#endif
  else
  {
    atSet(v,mstrdup(name),c->CopyD(),c->Typ());
    if (h!=NULL) IDATTR(h)=v->attribute;
  }
  return FALSE;
}

BOOLEAN atKILLATTR1(leftv res,leftv a)
{
  if ((a->rtyp!=IDHDL)||(a->e!=NULL))
  {
    WerrorS("object must have a name");
    return TRUE;
  }
  resetFlag(a,FLAG_STD);
  resetFlag((idhdl)a->data,FLAG_STD);
  resetFlag(a,FLAG_DOPERATOR);
  resetFlag((idhdl)a->data,FLAG_DOPERATOR);
  if (a->attribute!=NULL)
  {
    atKillAll((idhdl)a->data);
    a->attribute=NULL;
  }
  return FALSE;
}
BOOLEAN atKILLATTR2(leftv res,leftv a,leftv b)
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
#ifdef DRING
  else if (strcmp(name,"D")==0)
  {
    resetFlag(a,FLAG_DOPERATOR);
    resetFlag((idhdl)a->data,FLAG_DOPERATOR);
  }
#endif
  else
  {
    atKill((idhdl)a->data,name);
  }
  return FALSE;
}
