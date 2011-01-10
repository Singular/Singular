#include <Singular/mod2.h>
#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <Singular/bigintm.h>

// as this is only a demo,
// we do not included compiled code:
#if 0
char * bigintm_String(blackbox *b, void *d);
void * bigintm_Copy(blackbox*b, void *d);
BOOLEAN bigintm_Assign(leftv l, leftv r);
BOOLEAN bigintm_Op2(int op, leftv res, leftv a1, leftv a2);
BOOLEAN bigintm_OpM(int op, leftv res, leftv args);
void bigintm_destroy(blackbox *b, void *d);

void bigintm_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite usefule,
  // all other are simply error messages
  b->blackbox_destroy=bigintm_destroy;
  b->blackbox_String=bigintm_String;
  //b->blackbox_Print=blackbox_default_Print;
  //b->blackbox_Init=blackbox_default_Init;
  b->blackbox_Copy=bigintm_Copy;
  b->blackbox_Assign=bigintm_Assign;
  //b->blackbox_Op1=blackbox_default_Op1;
  b->blackbox_Op2=bigintm_Op2;
  //b->blackbox_Op3=blackbox_default_Op3;
  b->blackbox_OpM=bigintm_OpM;
  int rt=setBlackboxStuff(b,"bigintm");
  Print("create type %d (bigintm)\n",rt); 
}

char * bigintm_String(blackbox *b, void *d)
{ if (d==NULL) return omStrDup("oo");
   else
   {
     StringSetS("");
     number n=(number)d; nlWrite(n,NULL); d=(void*)n;
     return omStrDup(StringAppendS(""));
    }
}
void * bigintm_Copy(blackbox*b, void *d)
{  number n=(number)d; return nlCopy(n); }

BOOLEAN bigintm_Assign(leftv l, leftv r)
{
  blackbox *ll=getBlackboxStuff(l->Typ());
  if (r->Typ()>MAX_TOK)
  {
    blackbox *rr=getBlackboxStuff(r->Typ());
    if (l->Typ()==r->Typ())
    {
      if (l->Data()!=NULL) { number n1=(number)l->Data(); nlDelete(&n1,NULL); }
      number n2=(number)r->CopyD();
      if (l->rtyp==IDHDL)
      {
        IDDATA((idhdl)l->data)=(char *)n2;
      }
      else
      {
        l->data=(void *)n2;
      }
      return FALSE;
    }
    else
    {
      Werror("assign %d = %d",l->Typ(),r->Typ());
      return TRUE;
    }
  }
  else if (r->Typ()==INT_CMD)
  {
    if (l->Data()!=NULL) { number n1=(number)l->Data(); nlDelete(&n1,NULL); }
    number n2=nlInit((int)(long)r->Data(),NULL);
    if (l->rtyp==IDHDL)
    {
      IDDATA((idhdl)l->data)=(char *)n2;
    }
    else
    {
      l->data=(void *)n2;
    }
    return FALSE;
  }
  else
    Werror("assign %d = %d",l->Typ(),r->Typ());
  return TRUE;
}
BOOLEAN bigintm_OpM(int op, leftv res, leftv args);


BOOLEAN bigintm_Op2(int op, leftv res, leftv a1, leftv a2)
{
  // interpreter: a1 is ist bigintm
  blackbox *a=getBlackboxStuff(a1->Typ());
  number n1=(number)a1->Data(); 
  switch(op)
  {
    case '+':
    {
      if (a2->Typ()==INT_CMD)
      {
        number n2=nlInit((int)(long)a2->Data(),NULL);
        number n=nlAdd(n1,n2);
        res->data=(void *)n;
        res->rtyp=a1->Typ();
        return FALSE;
      }
      else if (a2->Typ()==a1->Typ())
      {
        number n2=(number)a2->Data(); 
        number n=nlAdd(n1,n2);
        res->data=(void *)n;
        res->rtyp=a1->Typ();
        return FALSE;
      }
      return TRUE;
    }
    default:
      Werror("op %d not implemented for type %d",op,a1->Typ());
      break;
  }
  return TRUE;
}
// BOOLEAN opM(int op, leftv res, leftv args)
BOOLEAN bigintm_OpM(int op, leftv res, leftv args)
{
  // interpreter: args->1. arg is ist bigintm
  blackbox *a=getBlackboxStuff(args->Typ());
  switch(op)
  {
    case STRING_CMD:
    {
      res->data=(void *)a->blackbox_String(a,args->Data());
      res->rtyp=STRING_CMD;
      return FALSE;
    }
    default:
      Werror("op %d not implemented for type %d",op,args->Typ());
      break;
  }
  return TRUE;
}
void bigintm_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    number n=(number)d;
    nlDelete(&n,NULL);
  }
}
#endif
