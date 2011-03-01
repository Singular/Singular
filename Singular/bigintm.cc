#include <Singular/mod2.h>
#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>
#include <Singular/bigintm.h>


#define HAVE_BIGINTM 1

#ifdef HAVE_BIGINTM
static int bigintm_type_id = -1;
#endif


#ifdef HAVE_BIGINTM
static char * bigintm_String(blackbox *b, void *d)
{ if (d==NULL) return omStrDup("oo");
   else
   {
     StringSetS("");
     number n=(number)d; nlWrite(n,NULL); d=(void*)n;
     return omStrDup(StringAppendS(""));
    }
}
static void * bigintm_Copy(blackbox*b, void *d)
{  number n=(number)d; return nlCopy(n); }

static BOOLEAN bigintm_Assign(leftv l, leftv r)
{
  assume( l->Typ() == bigintm_type_id );
  
  blackbox *ll=getBlackboxStuff(l->Typ());
  
  if (r->Typ()>MAX_TOK)
  {
    if (bigintm_type_id == r->Typ())
    {
      blackbox *rr=getBlackboxStuff(r->Typ());
      
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
      Werror("bigintm_Assign: assign %s (%d) = %s (%d)",
             getBlackboxName(l->Typ()), l->Typ(),
             getBlackboxName(r->Typ()), r->Typ());
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
static BOOLEAN bigintm_OpM(int op, leftv res, leftv args);


static BOOLEAN bigintm_Op2(int op, leftv res, leftv a1, leftv a2)
{
  // interpreter: a1 is ist bigintm
  assume( a1->Typ() == bigintm_type_id );
  
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
      Werror("bigintm_Op2: op %d not implemented for type %s (%d)", op, getBlackboxName(a1->Typ()), a1->Typ());
      break;
  }
  return blackboxDefaultOp2(op,res,a1,a2);
}
// BOOLEAN opM(int op, leftv res, leftv args)
static BOOLEAN bigintm_OpM(int op, leftv res, leftv args)
{
  // interpreter: args->1. arg is ist bigintm
  assume( args->Typ() == bigintm_type_id );
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
      Werror("bigintm_OpM: op %d not implemented for type %s (%d)", op, getBlackboxName(args->Typ()), args->Typ());
      break;
  }
  return TRUE;
}
static void bigintm_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    number n=(number)d;
    nlDelete(&n,NULL);
  }
}

#endif




// this is only a demo
BOOLEAN bigintm_setup()
{
#ifndef HAVE_BIGINTM
  Werror("bigintm_setup: Sorry BIGINTM was not compiled in!");
  return TRUE; // ok, TRUE = error!
#else

  if( bigintm_type_id == -1 )
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
    //b->blackbox_Op1=blackboxDefaultOp1;
    b->blackbox_Op2=bigintm_Op2;
    //b->blackbox_Op3=blackbox_default_Op3;
    b->blackbox_OpM=bigintm_OpM;

    bigintm_type_id = setBlackboxStuff(b,"bigintm");

    Print("bigintm_setup: create type %d (%s)",bigintm_type_id, getBlackboxName(bigintm_type_id));
    PrintLn();

    return FALSE; // ok, TRUE = error!
  } else
  {
    Werror("bigintm_setup: Sorry should NOT be run twice!");
    return TRUE; // ok, TRUE = error!
  }

#endif
}



