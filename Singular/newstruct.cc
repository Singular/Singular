#include "kernel/mod2.h"

#include "Singular/ipid.h"
#include "Singular/blackbox.h"
#include "Singular/lists.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/ipconv.h"
#include "Singular/newstruct.h"

#include <ctype.h>

struct newstruct_member_s;
typedef struct newstruct_member_s *newstruct_member;
struct  newstruct_member_s
{
  newstruct_member next;
  char *         name;
  int            typ;
  int            pos;
};

struct newstruct_proc_s;
typedef struct newstruct_proc_a *newstruct_proc;
struct  newstruct_proc_a
{
  newstruct_proc next;
  int            t; /*tok id */
  int            args; /* number of args */
  procinfov      p;
};

struct newstruct_desc_s
{
  newstruct_member member;
  newstruct_desc   parent;
  newstruct_proc   procs;
  int            size; // number of mebers +1
  int            id;   // the type id assigned to this bb
};

int newstruct_desc_size()
{
  return sizeof(newstruct_desc_s);
}

char * newstruct_String(blackbox *b, void *d)
{
  if (d==NULL) return omStrDup("oo");
  else
  {
    newstruct_desc ad=(newstruct_desc)(b->data);

    newstruct_proc p=ad->procs;
    while((p!=NULL)&&(p->t!=STRING_CMD))
      p=p->next;

    if (p!=NULL)
    {
      sleftv tmp;
      tmp.Init();
      tmp.rtyp=ad->id;
      void * newstruct_Copy(blackbox*, void *); //forward declaration
      tmp.data=(void*)newstruct_Copy(b,d);
      idrec hh;
      hh.Init();
      hh.id=Tok2Cmdname(p->t);
      hh.typ=PROC_CMD;
      hh.data.pinf=p->p;
      BOOLEAN sl=iiMake_proc(&hh,NULL,&tmp);

      if ((!sl)&& (iiRETURNEXPR.Typ() == STRING_CMD))
      {
        char *res = (char*)iiRETURNEXPR.CopyD();
        iiRETURNEXPR.Init();
        return res;
      }
      iiRETURNEXPR.CleanUp();
      iiRETURNEXPR.Init();
    }

    lists l=(lists)d;
    newstruct_member a=ad->member;
    StringSetS("");
    loop
    {
      StringAppendS(a->name);
      StringAppendS("=");
      if ((!RingDependend(a->typ))
      || ((l->m[a->pos-1].data==(void *)currRing)
         && (currRing!=NULL)))
      {
        if (l->m[a->pos].rtyp==LIST_CMD)
        {
          StringAppendS("<list>");
        }
        else if (l->m[a->pos].rtyp==STRING_CMD)
	{
          StringAppendS((char*)l->m[a->pos].Data());
	}
	else
        {
          char *tmp2=omStrDup(l->m[a->pos].String());
          if ((strlen(tmp2)>80)||(strchr(tmp2,'\n')!=NULL))
          {
            StringAppendS("<");
            StringAppendS(Tok2Cmdname(l->m[a->pos].rtyp));
            StringAppendS(">");
          }
          else StringAppendS(tmp2);
          omFree(tmp2);
        }
      }
      else StringAppendS("??");
      if (a->next==NULL) break;
      StringAppendS("\n");
      if(errorreported) break;
      a=a->next;
    }
    return StringEndS();
  }
}
lists lCopy_newstruct(lists L)
{
  lists N=(lists)omAlloc0Bin(slists_bin);
  int n=L->nr;
  ring save_ring=currRing;
  N->Init(n+1);
  for(;n>=0;n--)
  {
    if (RingDependend(L->m[n].rtyp)
    ||((L->m[n].rtyp==LIST_CMD)&&lRingDependend((lists)L->m[n].data)))
    {
      assume((L->m[n-1].rtyp==RING_CMD) || (L->m[n-1].data==NULL));
      if(L->m[n-1].data!=NULL)
      {
        if (L->m[n-1].data!=(void*)currRing)
          rChangeCurrRing((ring)(L->m[n-1].data));
        N->m[n].Copy(&L->m[n]);
      }
      else
      {
        N->m[n].rtyp=L->m[n].rtyp;
        N->m[n].data=idrecDataInit(L->m[n].rtyp);
      }
    }
    else if(L->m[n].rtyp==LIST_CMD)
    {
      N->m[n].rtyp=L->m[n].rtyp;
      N->m[n].data=(void *)lCopy((lists)(L->m[n].data));
    }
    else if(L->m[n].rtyp>MAX_TOK)
    {
      N->m[n].rtyp=L->m[n].rtyp;
      blackbox *b=getBlackboxStuff(N->m[n].rtyp);
      N->m[n].data=(void *)b->blackbox_Copy(b,L->m[n].data);
    }
    else
      N->m[n].Copy(&L->m[n]);
  }
  if (currRing!=save_ring) rChangeCurrRing(save_ring);
  return N;
}
void * newstruct_Copy(blackbox*, void *d)
{
  lists n1=(lists)d;
  return (void*)lCopy_newstruct(n1);
}

// Used by newstruct_Assign for overloaded '='
BOOLEAN newstruct_Assign_user(int op, leftv l, leftv r)
{
  blackbox *ll=getBlackboxStuff(op);
  assume(ll->data != NULL);
  newstruct_desc nt=(newstruct_desc)ll->data;
  newstruct_proc p=nt->procs;

  while( (p!=NULL) && ((p->t!='=')||(p->args!=1)) ) p=p->next;

  if (p!=NULL)
  {
    BOOLEAN sl;
    idrec hh;
    hh.Init();
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    sleftv tmp;
    tmp.Copy(r);
    sl = iiMake_proc(&hh, NULL, &tmp);
    if (!sl)
    {
      if (iiRETURNEXPR.Typ() == op)
      {
        memcpy(l,&iiRETURNEXPR,sizeof(sleftv));
        iiRETURNEXPR.Init();
        return FALSE;
      }
      iiRETURNEXPR.CleanUp();
      iiRETURNEXPR.Init();
    }
  }
  return TRUE;
}

void lClean_newstruct(lists l)
{
  if (l->nr>=0)
  {
    int i;
    ring r=NULL;
    for(i=l->nr;i>=0;i--)
    {
      if ((i>0) && (l->m[i-1].rtyp==RING_CMD))
        r=(ring)(l->m[i-1].data);
      else
        r=NULL;
      l->m[i].CleanUp(r);
    }
    omFreeSize((ADDRESS)l->m, (l->nr+1)*sizeof(sleftv));
    l->nr=-1;
  }
  omFreeBin((ADDRESS)l,slists_bin);
}

static BOOLEAN newstruct_Assign_same(leftv l, leftv r)
{
  assume(l->Typ() == r->Typ());
  if (l->Data()!=NULL)
  {
    lists n1=(lists)l->Data();
    lClean_newstruct(n1);
  }
  lists n2=(lists)r->Data();
  n2=lCopy_newstruct(n2);
  r->CleanUp();
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

BOOLEAN newstruct_Op1(int op, leftv res, leftv arg)
{
  // interpreter: arg is newstruct
  blackbox *a=getBlackboxStuff(arg->Typ());
  newstruct_desc nt=(newstruct_desc)a->data;
  newstruct_proc p=nt->procs;

  while((p!=NULL) &&( (p->t!=op) || (p->args!=1) )) p=p->next;

  if (p!=NULL)
  {
    idrec hh;
    hh.Init();
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    BOOLEAN sl=iiMake_proc(&hh,NULL,arg);
    if (sl) return TRUE;
    else
    {
      memcpy(res,&iiRETURNEXPR,sizeof(sleftv));
      iiRETURNEXPR.Init();
      return FALSE;
    }
  }
  return blackboxDefaultOp1(op,res,arg);
}

BOOLEAN newstruct_Assign(leftv l, leftv r)
{
  assume(l->Typ() > MAX_TOK);
  if (l->Typ()==r->Typ())
  {
    return newstruct_Assign_same(l,r);
  }
  if (r->Typ()>MAX_TOK)
  {
    blackbox *rr=getBlackboxStuff(r->Typ());
    if (l->Typ()!=r->Typ())
    {
      newstruct_desc rrn=(newstruct_desc)rr->data;

      if (rrn==NULL) // this is not a newstruct
      {
        Werror("custom type %s(%d) cannot be assigned to newstruct %s(%d)",
               Tok2Cmdname(r->Typ()), r->Typ(), Tok2Cmdname(l->Typ()), l->Typ());
        return TRUE;
      }

      // try to find a parent newstruct:
      newstruct_desc rrp=rrn->parent;
      while ((rrp!=NULL)&&(rrp->id!=l->Typ())) rrp=rrp->parent;
      if (rrp!=NULL)
      {
        if (l->rtyp==IDHDL)
        {
          IDTYP((idhdl)l->data)=r->Typ();
        }
        else
        {
          l->rtyp=r->Typ();
        }
      }
      else                      // unrelated types - look for custom conversion
      {
        sleftv tmp;
        if (! newstruct_Op1(l->Typ(), &tmp, r))  return newstruct_Assign(l, &tmp);
        if(!newstruct_Assign_user(l->Typ(), &tmp, r)) return newstruct_Assign(l, &tmp);
      }
    }
    if (l->Typ()==r->Typ())
    {
      return  newstruct_Assign_same(l,r);
    }
  }
  else
  {
    sleftv tmp;
    if(!newstruct_Assign_user(l->Typ(), &tmp, r)) return newstruct_Assign(l, &tmp);
  }
  Werror("assign %s(%d) = %s(%d)",
        Tok2Cmdname(l->Typ()),l->Typ(),Tok2Cmdname(r->Typ()),r->Typ());
  return TRUE;
}

BOOLEAN newstruct_Op2(int op, leftv res, leftv a1, leftv a2)
{
  // interpreter: a1 or a2 is newstruct
  blackbox *a=getBlackboxStuff(a1->Typ());
  newstruct_desc nt;
  lists al=(lists)a1->Data();
  if (a!=NULL)
  {
    nt=(newstruct_desc)a->data;
    switch(op)
    {
      case '.':
      {
        if (a2->name!=NULL)
        {
          BOOLEAN search_ring=FALSE;
          newstruct_member nm=nt->member;
          while ((nm!=NULL)&&(strcmp(nm->name,a2->name)!=0)) nm=nm->next;
          if ((nm==NULL) && (strncmp(a2->name,"r_",2)==0))
          {
            nm=nt->member;
            while ((nm!=NULL)&&(strcmp(nm->name,a2->name+2)!=0)) nm=nm->next;
            if ((nm!=NULL)&&(RingDependend(nm->typ)))
              search_ring=TRUE;
            else
              nm=NULL;
          }
          if (nm==NULL)
          {
            Werror("member %s not found", a2->name);
            return TRUE;
          }
          if (search_ring)
          {
            ring r;
            res->rtyp=RING_CMD;
            res->data=al->m[nm->pos-1].data;
            r=(ring)res->data;
            if (r==NULL)
            {
              res->data=(void *)currRing; r=currRing;
              if (r!=NULL) r->ref++;
              else WerrorS("ring of this member is not set and no basering found");
            }
            a1->CleanUp();
            a2->CleanUp();
            return r==NULL;
          }
          else if (RingDependend(nm->typ)
          || (al->m[nm->pos].RingDependend()))
          {
            if (al->m[nm->pos].data==NULL)
            {
              // NULL belongs to any ring
              ring r=(ring)al->m[nm->pos-1].data;
              if (r!=NULL)
              {
                r->ref--;
                al->m[nm->pos-1].data=NULL;
                al->m[nm->pos-1].rtyp=DEF_CMD;
              }
            }
            else
            {
              //Print("checking ring at pos %d for dat at pos %d\n",nm->pos-1,nm->pos);
              #if 0
              if ((al->m[nm->pos-1].data!=(void *)currRing)
              &&(al->m[nm->pos-1].data!=(void*)0L))
              {
                Werror("different ring %lx(data) - %lx(basering)",
                  (long unsigned)(al->m[nm->pos-1].data),(long unsigned)currRing);
                Werror("name of basering: %s",IDID(currRingHdl));
                rWrite(currRing,TRUE);PrintLn();
                idhdl hh=rFindHdl((ring)(al->m[nm->pos-1].data),NULL);
                const char *nn="??";
                if (hh!=NULL) nn=IDID(hh);
                Werror("(possible) name of ring of data: %s",nn);
                rWrite((ring)(al->m[nm->pos-1].data),TRUE);PrintLn();

                return TRUE;
              }
              #endif
            }
            if(al->m[nm->pos-1].data!=NULL)
            {
              ring old=(ring)al->m[nm->pos-1].data;
              old->ref--;
            }
            // remember the ring, if not already set
            al->m[nm->pos-1].data=(void *)currRing;
            al->m[nm->pos-1].rtyp=RING_CMD;
            if (currRing!=NULL)  currRing->ref++;
          }
          else if ((nm->typ==DEF_CMD)||(nm->typ==LIST_CMD))
          {
            if(al->m[nm->pos-1].data!=NULL)
            {
              ring old=(ring)al->m[nm->pos-1].data;
              old->ref--;
            }
            al->m[nm->pos-1].data=(void*)currRing;
            if (currRing!=NULL) currRing->ref++;
          }
          Subexpr r=(Subexpr)omAlloc0Bin(sSubexpr_bin);
          r->start = nm->pos+1;
          memcpy(res,a1,sizeof(sleftv));
          a1->Init();
          if (res->e==NULL) res->e=r;
          else
          {
            Subexpr sh=res->e;
            while (sh->next != NULL) sh=sh->next;
            sh->next=r;
          }
          //a1->CleanUp();// see Init() above
          a2->CleanUp();
          return FALSE;
        }
        else
        {
          WerrorS("name expected");
          return TRUE;
        }
      }
    }
  }
  else
  {
    a=getBlackboxStuff(a2->Typ());
    nt=(newstruct_desc)a->data;
    al=(lists)a2->Data();
  }
  newstruct_proc p=nt->procs;
  while((p!=NULL) && ( (p->t!=op) || (p->args!=2) )) p=p->next;
  if (p!=NULL)
  {
    sleftv tmp;
    tmp.Copy(a1);
    tmp.next=(leftv)omAlloc0(sizeof(sleftv));
    tmp.next->Copy(a2);
    idrec hh;
    hh.Init();
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    BOOLEAN sl=iiMake_proc(&hh,NULL,&tmp);
    a1->CleanUp();
    a2->CleanUp();
    if (sl) return TRUE;
    else
    {
      memcpy(res,&iiRETURNEXPR,sizeof(sleftv));
      iiRETURNEXPR.Init();
      return FALSE;
    }
  }
  return blackboxDefaultOp2(op,res,a1,a2);
}

// BOOLEAN opM(int op, leftv res, leftv args)
BOOLEAN newstruct_OpM(int op, leftv res, leftv args)
{
  // interpreter: args->1. arg is newstruct
  blackbox *a=getBlackboxStuff(args->Typ());
  newstruct_desc nt=(newstruct_desc)a->data;
  switch(op)
  {
    case STRING_CMD:
    {
      res->data=(void *)a->blackbox_String(a,args->Data());
      res->rtyp=STRING_CMD;
      args->CleanUp();
      return FALSE;
    }
    default:
      break;
  }
  newstruct_proc p=nt->procs;

  while((p!=NULL) &&( (p->t!=op) || (p->args!=4) )) p=p->next;

  if (p!=NULL)
  {
    idrec hh;
    hh.Init();
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    BOOLEAN sl=iiMake_proc(&hh,NULL,args);
    args->CleanUp();
    if (sl) return TRUE;
    else
    {
      memcpy(res,&iiRETURNEXPR,sizeof(sleftv));
      iiRETURNEXPR.Init();
      return FALSE;
    }
  }
  return blackboxDefaultOpM(op,res,args);
}

void newstruct_destroy(blackbox */*b*/, void *d)
{
  if (d!=NULL)
  {
    lists n=(lists)d;
    lClean_newstruct(n);
  }
}

void *newstruct_Init(blackbox *b)
{
  newstruct_desc n=(newstruct_desc)b->data;
  lists l=(lists)omAlloc0Bin(slists_bin);
  l->Init(n->size);
  newstruct_member nm=n->member;
  while (nm!=NULL)
  {
    l->m[nm->pos].rtyp=nm->typ;
    if (RingDependend(nm->typ) ||(nm->typ==DEF_CMD)||(nm->typ==LIST_CMD))
    {
      l->m[nm->pos-1].rtyp=RING_CMD;
      l->m[nm->pos-1].data=currRing; //idrecDataInit may create ringdep obj.
      if (currRing!=NULL) currRing->ref++;
    }
    l->m[nm->pos].data=idrecDataInit(nm->typ);
    nm=nm->next;
  }
  return l;
}

BOOLEAN newstruct_CheckAssign(blackbox */*b*/, leftv L, leftv R)
{
  int lt=L->Typ();
  int rt=R->Typ();
  if (iiTestConvert(rt,lt,dConvertTypes)==0)
  {
    const char *rt1=Tok2Cmdname(rt);
    const char *lt1=Tok2Cmdname(lt);
    if ((rt>0) && (lt>0)
    && ((strcmp(rt1,Tok2Cmdname(0))==0)||(strcmp(lt1,Tok2Cmdname(0))==0)))
    {
      Werror("can not assign %s(%d) to member of type %s(%d)",
            rt1,rt,lt1,lt);
    }
    else
    {
      Werror("can not assign %s to member of type %s",rt1,lt1);
    }
    return TRUE;
  }
  return FALSE;
}

/* check internal structure:
* BOOLEAN newstruct_Check(blackbox *b, void *d)
{
  newstruct_desc n=(newstruct_desc)b->data;
  lists l=(lists)d;
  newstruct_member nm=n->member;
  while (nm!=NULL)
  {
    if ((l->m[nm->pos].rtyp!=nm->typ)
    &&( nm->typ!=DEF_CMD))
    {
      Werror("type change in member %s (%s(%d) -> %s(%d))",nm->name,
          Tok2Cmdname(nm->typ),nm->typ,
          Tok2Cmdname(l->m[nm->pos].rtyp),l->m[nm->pos].rtyp);
      return TRUE;
    }
    nm=nm->next;
  }
  return FALSE;
}
*/

BOOLEAN newstruct_serialize(blackbox *b, void *d, si_link f)
{
  newstruct_desc dd=(newstruct_desc)b->data;
  sleftv l;
  l.Init();
  l.rtyp=STRING_CMD;
  l.data=(void*)getBlackboxName(dd->id);
  f->m->Write(f, &l);
  lists ll=(lists)d;
  int Ll=lSize(ll);
  l.rtyp=INT_CMD;
  l.data=(void*)(long)Ll;
  f->m->Write(f, &l);
  // set all entries corresponding to "real" mebers to 1 in rings
  char *rings=(char*)omAlloc0(Ll+1);
  newstruct_member elem=dd->member;
  while (elem!=NULL)
  {
    rings[elem->pos]='\1';
    elem=elem->next;
  }
  int i;
  BOOLEAN ring_changed=FALSE;
  ring save_ring=currRing;
  for(i=0;i<=Ll;i++)
  {
    if (rings[i]=='\0') // ring entry for pos i+1
    {
      if (ll->m[i].data!=NULL)
      {
        ring_changed=TRUE;
        f->m->SetRing(f,(ring)ll->m[i].data,TRUE);
      }
    }
    f->m->Write(f,&(ll->m[i]));
  }
  omFreeSize(rings,Ll+1);
  if (ring_changed)
    f->m->SetRing(f,save_ring,FALSE);
  return FALSE;
}

BOOLEAN newstruct_deserialize(blackbox **, void **d, si_link f)
{
  // newstruct is serialized as analog to a list,
  // just read a list and take data,
  // rtyp must be set correctly (to the blackbox id) by routine calling
  // newstruct_deserialize
  leftv l=f->m->Read(f); // int: length of list
  int Ll=(int)(long)(l->data);
  omFreeBin(l,sleftv_bin);
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(Ll+1);
  for(int i=0;i<=Ll;i++)
  {
    l=f->m->Read(f);
    memcpy(&(L->m[i]),l,sizeof(sleftv));
    omFreeBin(l,sleftv_bin);
  }
  //newstruct_desc n=(newstruct_desc)b->data;
  //TODO: check compatibility of list l->data with description in n
  *d=L;
  return FALSE;
}

void newstruct_Print(blackbox *b,void *d)
{
  newstruct_desc dd=(newstruct_desc)b->data;
  newstruct_proc p=dd->procs;
  while((p!=NULL)&&(p->t!=PRINT_CMD))
    p=p->next;
  if (p!=NULL)
  {
    BOOLEAN sl;
    sleftv tmp;
    tmp.Init();
    tmp.rtyp=dd->id;
    tmp.data=(void*)newstruct_Copy(b,d);
    idrec hh;
    hh.Init();
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    sl=iiMake_proc(&hh,NULL,&tmp);
    if (!sl)
    {
      if (iiRETURNEXPR.Typ()!=NONE) Warn("ignoring return value (%s)",Tok2Cmdname(iiRETURNEXPR.Typ()));
      iiRETURNEXPR.CleanUp();
    }
    iiRETURNEXPR.Init();
  }
  else
    blackbox_default_Print(b,d);
}
void newstruct_setup(const char *n, newstruct_desc d )
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite useful,
  // all other are simply error messages
  b->blackbox_destroy=newstruct_destroy;
  b->blackbox_String=newstruct_String;
  b->blackbox_Print=newstruct_Print;//blackbox_default_Print;
  b->blackbox_Init=newstruct_Init;
  b->blackbox_Copy=newstruct_Copy;
  b->blackbox_Assign=newstruct_Assign;
  b->blackbox_Op1=newstruct_Op1;
  b->blackbox_Op2=newstruct_Op2;
  //b->blackbox_Op3=blackboxDefaultOp3;
  b->blackbox_OpM=newstruct_OpM;
  b->blackbox_CheckAssign=newstruct_CheckAssign;
  b->blackbox_serialize=newstruct_serialize;
  b->blackbox_deserialize=newstruct_deserialize;
  b->data=d;
  b->properties=1; // list_like
  int rt=setBlackboxStuff(b,n);
  d->id=rt;
  //Print("create type %d (%s)\n",rt,n);
}

static newstruct_desc scanNewstructFromString(const char *s, newstruct_desc res)
{
  char *ss=omStrDup(s);
  char *p=ss;
  char *start;
  int t;
  char c;
  newstruct_member elem;

  idhdl save_ring=currRingHdl;
  currRingHdl=(idhdl)1; // fake ring detection
  loop
  {
    // read type:
    while ((*p!='\0') && (*p<=' ')) p++;
    start=p;
    while (isalnum(*p)) p++;
    *p='\0';
    IsCmd(start,t);
    if (t==0)
    {
      Werror("unknown type `%s`",start);
      omFree(ss);
      omFree(res);
      currRingHdl=save_ring;
      return NULL;
    }
    if (t==QRING_CMD) t=RING_CMD;
    else if (RingDependend(t) || (t==DEF_CMD)||(t==LIST_CMD))
      res->size++;    // one additional field for the ring (before the data)
    //Print("found type %s at real-pos %d",start,res->size);
    elem=(newstruct_member)omAlloc0(sizeof(*elem));
    // read name:
    p++;
    while ((*p!='\0') && (*p<=' ')) p++;
    start=p;
    while (isalnum(*p)) p++;
    c=*p;
    *p='\0';
    elem->typ=t;
    elem->pos=res->size;
    if ((*start=='\0') /*empty name*/||(isdigit(*start)))
    {
      WerrorS("illegal/empty name for element");
      goto error_in_newstruct_def;
    }
    elem->name=omStrDup(start);
    //Print(" name:%s\n",start);
    elem->next=res->member;
    res->member=elem;
    res->size++;

    // next ?
    *p=c;
    while ((*p!='\0') && (*p<=' ')) p++;
    if (*p!=',')
    {
      if (*p!='\0')
      {
        Werror("unknown character in newstruct:>>%s<<",p);
        goto error_in_newstruct_def;
      }
      break; // end-of-list
    }
    p++;
  }
  omFree(ss);
  currRingHdl=save_ring;
  //Print("new type with %d elements\n",res->size);
  //newstructShow(res);
  return res;
error_in_newstruct_def:
   omFree(elem);
   omFree(ss);
   omFree(res);
   currRingHdl=save_ring;
   return NULL;
}
newstruct_desc newstructFromString(const char *s)
{
  newstruct_desc res=(newstruct_desc)omAlloc0(sizeof(*res));
  res->size=0;

  return scanNewstructFromString(s,res);
}
newstruct_desc newstructChildFromString(const char *parent, const char *s)
{
  // find parent:
  int parent_id=0;
  blackboxIsCmd(parent,parent_id);
  if (parent_id<MAX_TOK)
  {
    Werror(">>%s< not found",parent);
    return NULL;
  }
  blackbox *parent_bb=getBlackboxStuff(parent_id);
  // check for the correct type:
  if (parent_bb->blackbox_destroy!=newstruct_destroy)
  {
    Werror(">>%s< is not a user defined type",parent);
    return NULL;
  }
  // setup for scanNewstructFromString:
  newstruct_desc res=(newstruct_desc)omAlloc0(sizeof(*res));
  newstruct_desc parent_desc=(newstruct_desc)parent_bb->data;
  res->size=parent_desc->size;
  res->member=parent_desc->member;
  res->parent=parent_desc;

  return scanNewstructFromString(s,res);
}

void newstructShow(newstruct_desc d)
{
  newstruct_member elem;
  Print("id: %d\n",d->id);
  elem=d->member;
  while (elem!=NULL)
  {
    Print(">>%s<< at pos %d, type %d (%s)\n",elem->name,elem->pos,elem->typ,Tok2Cmdname(elem->typ));
    if (RingDependend(elem->typ)|| (elem->typ==DEF_CMD) ||(elem->typ==LIST_CMD))
      Print(">>r_%s<< at pos %d, shadow ring\n",elem->name,elem->pos-1);
    elem=elem->next;
  }
  newstruct_proc p=d->procs;
  while (p!=NULL)
  {
    Print("op:%d(%s) with %d args -> %s\n",p->t,iiTwoOps(p->t),p->args,p->p->procname);
    p=p->next;
  }
}

BOOLEAN newstruct_set_proc(const char *bbname,const char *func, int args,procinfov pr)
{
  int id=0;
  blackboxIsCmd(bbname,id);
  if (id<MAX_TOK)
  {
    Werror(">>%s<< is not a newstruct type",bbname);
    return TRUE;
  }
  blackbox *bb=getBlackboxStuff(id);
  newstruct_desc desc=(newstruct_desc)bb->data;
  newstruct_proc p=(newstruct_proc)omAlloc(sizeof(*p));
  p->next=desc->procs; desc->procs=p;

  idhdl save_ring=currRingHdl;
  currRingHdl=(idhdl)1; // fake ring detection

  int tt;
  if(!(tt=IsCmd(func,p->t)))
  {
    int t;
    if((t=iiOpsTwoChar(func))!=0)
    {
      p->t=t;
      tt=CMD_2; /* ..,::, ==, <=, <>, >= !=i and +,-,*,/,%,.... */
      if ((t==PLUSPLUS)
      ||(t==MINUSMINUS)
      ||(t=='='))
        tt=CMD_1; /* ++,--,= */
      else if (t=='(') /* proc call */
        tt=CMD_M;
      else if (t=='-') /* unary and binary - */
        tt=CMD_12;
    }
    else
    {
      desc->procs=p->next;
      omFreeSize(p,sizeof(*p));
      Werror(">>%s<< is not a kernel command",func);
      currRingHdl = save_ring;
      return TRUE;
    }
  }
  switch(tt)
  {
    // type conversions:
    case BIGINTMAT_CMD:
    case MATRIX_CMD:
    case INTMAT_CMD:
    case RING_CMD:
    case RING_DECL:
    case ROOT_DECL:
    // operations:
    case CMD_1:
      if(args!=1) { Warn("args must be 1 for %s in %s",func,my_yylinebuf);args=1;}
      break;
    case CMD_2:
      if(args!=2) { Warn("args must be 2 in %s",my_yylinebuf);args=2;}
      break;
    case CMD_3:
      if(args!=3) { Warn("args must be 3 in %s",my_yylinebuf);args=3;}
      break;
    case CMD_12:
      if((args!=1)&&(args!=2)) { Werror("args must in 1 or 2 in %s",my_yylinebuf);}
      break;
    case CMD_13:
      if((args!=1)&&(args!=3)) { Werror("args must in 1 or 3 in %s",my_yylinebuf);}
      break;
    case CMD_23:
      if((args<2)||(args>3)) { Werror("args must in 2..3 in %s",my_yylinebuf);}
      break;
    case CMD_123:
      if((args<1)||(args>3)) { Werror("args must in 1..3 in %s",my_yylinebuf);}
      break;
    case RING_DECL_LIST:
    case ROOT_DECL_LIST:
    case CMD_M:
      break;
    default:
      Werror("unknown token type %d in %s",tt,my_yylinebuf);
      break;
  }
  currRingHdl = save_ring;
  if (errorreported)
  {
    desc->procs=p->next;
    omFreeSize(p,sizeof(*p));
    return TRUE;
  }
  p->args=args;
  p->p=pr; pr->ref++;
  pr->is_static=0;
  return FALSE;
}
