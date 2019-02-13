#include "Singular/libsingular.h"
#include <sstream>
#include <unordered_set>

typedef std::unordered_set<int> si_intset; /* intset is already defined in std*/

STATIC_VAR int si_intset_type_id = -1;

void* si_intset_Init(blackbox *b)
{
  void *d=new si_intset();
  return (void*)d;
}

void si_intset_destroy(blackbox *b, void *d)
{
  if(d!=NULL)
  {
     si_intset* tobedeleted=(si_intset*) d;
     delete tobedeleted;
  }
}

char* si_intset_String(blackbox *b, void *d)
{
  if(d!=NULL)
  {
    si_intset *myset;
    myset=(si_intset*)d;
    std::stringstream out;
    StringSetS("{");
    for (si_intset::iterator it = myset->begin(); it != myset->end(); ++it)
    {
      StringAppend("%d,", *it);
    }
    StringAppendS("}");
    return(omStrDup(StringEndS()));
  }
  else
  {
    return omStrDup("invalid object");
  }
}

void* si_intset_Copy(blackbox *b, void *d)
{
  si_intset *tobecopied= (si_intset*)d;
  si_intset *dd=new si_intset(*tobecopied);
  return (void*)dd;
}

BOOLEAN si_intset_Assign(leftv l, leftv r)
{
  if (r->Typ()==l->Typ())
  {
    if (l->Data()!=NULL)
    {
      //si_intset* tobedeleted=(si_intset*) l->Data();
      //delete tobedeleted;
      si_intset* tobecleared=(si_intset*) l->Data();
      tobecleared->clear();
    }
    si_intset *tobeassigned= (si_intset*)r->CopyD();
    if (l->rtyp==IDHDL)
    {
      IDDATA((idhdl)l->data)=(char*) tobeassigned;
    }
    else
    {
      l->data = (void*) tobeassigned;
    }
  }
  else
  {
    Werror("assign Type(%d) = Type(%d) not implemented",l->Typ(),r->Typ());
    return TRUE;
  }
  return FALSE;
}

BOOLEAN unite_set (leftv result, leftv arg)
{
  if ((arg==NULL)
  ||(arg->next==NULL)
  ||(arg->Typ()!=si_intset_type_id)
  ||(arg->next->Typ()!=si_intset_type_id)
  ||(arg->next->next->Typ()!=si_intset_type_id))
  {
    WerrorS("syntax: unite_set(<intset>,<intset>,<intset>)");
    return TRUE;
  }
  si_intset *a=(si_intset*)arg->Data();
  si_intset *b=(si_intset*)arg->next->Data();
  si_intset *c=(si_intset*)arg->next->next->Data();
  c->clear();
  for (si_intset::iterator it = a->begin(); it != a->end(); ++it)
  {
    c->insert(*it);
  }
  for (si_intset::iterator it = b->begin(); it != b->end(); ++it)
  {
    c->insert(*it);
  }
  result->rtyp= NONE;
  result->data= NULL;
  return FALSE;
}

BOOLEAN intersect_set (leftv result, leftv arg)
{
  if ((arg==NULL)
  ||(arg->next==NULL)
  ||(arg->Typ()!=si_intset_type_id)
  ||(arg->next->Typ()!=si_intset_type_id)
  ||(arg->next->next->Typ()!=si_intset_type_id))
  {
    WerrorS("syntax: intersect_set(<intset>,<intset>,<intset>)");
    return TRUE;
  }
  si_intset *a=(si_intset*)arg->Data();
  si_intset *b=(si_intset*)arg->next->Data();
  si_intset *c=(si_intset*)arg->next->next->Data();
  c->clear();
  if (b->size() < a->size())
  {
    for (si_intset::const_iterator it = b->begin(); it != b->end(); it++)
    {
      if (a->find(*it) != a->end())
        c->insert(*it);
    }
  }
  else
  {
    for (si_intset::const_iterator it = a->begin(); it != a->end(); it++)
    {
      if (b->find(*it) != b->end())
        c->insert(*it);
    }
  }
  result->rtyp=NONE;
  result->data= NULL;
  return FALSE;
}

BOOLEAN insert_set (leftv result, leftv arg)
{
  if ((arg==NULL)
  ||(arg->next==NULL)
  ||(arg->Typ()!=si_intset_type_id)
  ||(arg->next->Typ()!=INT_CMD))
  {
    WerrorS("syntax: insert_set(<intset>,<int>)");
    return TRUE;
  }
  si_intset *a=(si_intset*)arg->Data();
  int b=(int)(long)arg->next->Data();
  a->insert(b);
  result->rtyp=NONE;
  result->data=NULL;
  return FALSE;
}

BOOLEAN equal_set (leftv result, leftv arg)
{
  if ((arg==NULL)
  ||(arg->next==NULL)
  ||(arg->Typ()!=si_intset_type_id)
  ||(arg->next->Typ()!=si_intset_type_id))
  {
    WerrorS("syntax: equal_set(<intset>,<intset>)");
    return TRUE;
  }
  si_intset *a=(si_intset*)arg->Data();
  si_intset *b=(si_intset*)arg->next->Data();
  for (si_intset::const_iterator it = b->begin(); it != b->end(); it++)
  {
    if (a->find(*it) != a->end())
    {
      result->data= (void *) (FALSE);
      result->rtyp= INT_CMD;
      return FALSE;
    }
  }
  for (si_intset::const_iterator it = a->begin(); it != a->end(); it++)
  {
    if (b->find(*it) != b->end())
    {
      result->data= (void *) (FALSE);
      result->rtyp= INT_CMD;
      return FALSE;
    }
  }
  result->data= (void *) (TRUE);
  result->rtyp= INT_CMD;
  return FALSE;
}

extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
  VAR blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  b->blackbox_Init=si_intset_Init;
  b->blackbox_destroy=si_intset_destroy;
  b->blackbox_Copy=si_intset_Copy;
  b->blackbox_String=si_intset_String;
  b->blackbox_Assign=si_intset_Assign;
  si_intset_type_id=setBlackboxStuff(b,"intset");
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"unite_set",FALSE,unite_set);
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"intersect_set",FALSE,intersect_set);
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"insert_set",FALSE,insert_set);
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""),"equal_set",FALSE,equal_set);
  return MAX_TOK;
}
