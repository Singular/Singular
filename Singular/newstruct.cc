#include <ctype.h>

#include <Singular/mod2.h>
#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <Singular/lists.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/newstruct.h>

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


char * newstruct_String(blackbox *b, void *d)
{
  if (d==NULL) return omStrDup("oo");
  else
  {
    newstruct_desc ad=(newstruct_desc)(b->data);
    lists l=(lists)d;
    newstruct_member a=ad->member;
    StringSetS("");
    loop
    {
      StringAppendS(a->name);
      char *tmp=omStrDup(StringAppendS("="));
      if ((!RingDependend(a->typ))
      || ((l->m[a->pos-1].data==(void *)currRing)
         && (currRing!=NULL)))
      {
        if (l->m[a->pos].rtyp==LIST_CMD)
        {
          StringAppendS("<list>");
        }
        else
        {
          StringSetS("");
          char *tmp2=omStrDup(l->m[a->pos].String());
          StringSetS(tmp);
          if ((strlen(tmp2)>80)||(strchr(tmp2,'\n')!=NULL))
          {
            StringAppend("<%s>",Tok2Cmdname(l->m[a->pos].rtyp));
          }
          else StringAppendS(tmp2);
          omFree(tmp2);
        }
      }
      else StringAppendS("??");
      omFree(tmp);
      if (a->next==NULL) break;
      StringAppendS("\n");
      if(errorreported) break;
      a=a->next;
    }
    return omStrDup(StringAppendS(""));
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
    if (RingDependend(L->m[n].rtyp))
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
void * newstruct_Copy(blackbox*b, void *d)
{
  lists n1=(lists)d;
  return (void*)lCopy_newstruct(n1);
}

BOOLEAN newstruct_Assign(leftv l, leftv r)
{
  blackbox *ll=getBlackboxStuff(l->Typ());
  if (r->Typ()>MAX_TOK)
  {
    blackbox *rr=getBlackboxStuff(r->Typ());
    if (l->Typ()!=r->Typ())
    {
      newstruct_desc rrn=(newstruct_desc)rr->data;
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
    }
    if (l->Typ()==r->Typ())
    {
      if (l->Data()!=NULL)
      {
        lists n1=(lists)l->Data();
        n1->Clean(); n1=NULL;
      }
      lists n2=(lists)r->Data();
      n2=lCopy_newstruct(n2);
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
  }
  Werror("assign %s(%d) = %s(%d)",
        Tok2Cmdname(l->Typ()),l->Typ(),Tok2Cmdname(r->Typ()),r->Typ());
  return TRUE;
}

BOOLEAN newstruct_Op2(int op, leftv res, leftv a1, leftv a2)
{
  // interpreter: a1 is newstruct
  blackbox *a=getBlackboxStuff(a1->Typ());
  newstruct_desc nt=(newstruct_desc)a->data;
  lists al=(lists)a1->Data();
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
          Werror("member %s nor found", a2->name);
          return TRUE;
        }
        if (search_ring)
        {
          ring r;
          res->rtyp=RING_CMD;
          res->data=al->m[nm->pos-1].data;
          r=(ring)res->data;
          if (r==NULL) { res->data=(void *)currRing; r=currRing; }
          if (r!=NULL) r->ref++;
          else Werror("ring of this member is not set and no basering found");
          return r==NULL;
        }
        else if (RingDependend(nm->typ))
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
            if ((al->m[nm->pos-1].data!=(void *)currRing)
            &&(al->m[nm->pos-1].data!=(void*)0L))
            {
              Werror("different ring %lx(data) - %lx(basering)",
                (long unsigned)(al->m[nm->pos-1].data),(long unsigned)currRing);
              return TRUE;
            }
          }
          if ((currRing!=NULL)&&(al->m[nm->pos-1].data==NULL))
          {
            // remember the ring, if not already set
            al->m[nm->pos-1].data=(void *)currRing;
            al->m[nm->pos-1].rtyp=RING_CMD;
            currRing->ref++;
          }
        }
        Subexpr r=(Subexpr)omAlloc0Bin(sSubexpr_bin);
        r->start = nm->pos+1;
        memcpy(res,a1,sizeof(sleftv));
        memset(a1,0,sizeof(sleftv));
        if (res->e==NULL) res->e=r;
        else
        {
          Subexpr sh=res->e;
          while (sh->next != NULL) sh=sh->next;
          sh->next=r;
        }
        return FALSE;
      }
      else
      {
        WerrorS("name expected");
        return TRUE;
      }
    }
  }
  newstruct_proc p=nt->procs;
  while((p!=NULL) &&(p->t=op)&&(p->args!=2)) p=p->next;
  if (p!=NULL)
  {
    leftv sl;
    sleftv tmp;
    memset(&tmp,0,sizeof(sleftv));
    tmp.Copy(a1);
    tmp.next=(leftv)omAlloc0(sizeof(sleftv));
    tmp.next->Copy(a2);
    idrec hh;
    memset(&hh,0,sizeof(hh));
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    sl=iiMake_proc(&hh,NULL,&tmp);
    if (sl==NULL) return TRUE;
    else
    {
      res->Copy(sl);
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
      return FALSE;
    }
    default:
      break;
  }
  newstruct_proc p=nt->procs;
  while((p!=NULL) &&(p->t=op)&&(p->args!=4)) p=p->next;
  if (p!=NULL)
  {
    leftv sl;
    sleftv tmp;
    memset(&tmp,0,sizeof(sleftv));
    tmp.Copy(args);
    idrec hh;
    memset(&hh,0,sizeof(hh));
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    sl=iiMake_proc(&hh,NULL,&tmp);
    if (sl==NULL) return TRUE;
    else
    {
      res->Copy(sl);
      return FALSE;
    }
  }
  return blackboxDefaultOpM(op,res,args);
}

void newstruct_destroy(blackbox *b, void *d)
{
  if (d!=NULL)
  {
    lists n=(lists)d;
    n->Clean();
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
    l->m[nm->pos].data=idrecDataInit(nm->typ);
    nm=nm->next;
  }
  return l;
}

BOOLEAN newstruct_Check(blackbox *b, void *d)
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

BOOLEAN newstruct_serialize(blackbox *b, void *d, si_link f)
{
  newstruct_desc dd=(newstruct_desc)b->data;
  sleftv l;
  memset(&l,0,sizeof(l));
  l.rtyp=STRING_CMD;
  l.data=(void*)getBlackboxName(dd->id);
  f->m->Write(f, &l);
  lists ll=(lists)d;
  memset(&l,0,sizeof(l));
  l.rtyp=LIST_CMD;
  l.data=ll;
  f->m->Write(f, &l);
  return FALSE;
}

BOOLEAN newstruct_deserialize(blackbox **b, void **d, si_link f)
{
  // newstruct is serialiazed as a list,
  // just read a list and take data,
  // rtyp must be set correctly (to the blackbox id) by routine calling
  // newstruct_deserialize
  leftv l=f->m->Read(f);
  //newstruct_desc n=(newstruct_desc)b->data;
  //TODO: check compatibility of list l->data with description in n
  *d=l->data;
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
    leftv sl;
    sleftv tmp;
    memset(&tmp,0,sizeof(tmp));
    tmp.rtyp=dd->id;
    tmp.data=(void*)newstruct_Copy(b,d);
    idrec hh;
    memset(&hh,0,sizeof(hh));
    hh.id=Tok2Cmdname(p->t);
    hh.typ=PROC_CMD;
    hh.data.pinf=p->p;
    sl=iiMake_proc(&hh,NULL,&tmp);
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
  //b->blackbox_Op1=blackboxDefaultOp1;
  b->blackbox_Op2=newstruct_Op2;
  //b->blackbox_Op3=blackbox_default_Op3;
  b->blackbox_OpM=newstruct_OpM;
  b->blackbox_Check=newstruct_Check;
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
    while (*p==' ') p++;
    start=p;
    while (isalpha(*p)) p++;
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
    if (RingDependend(t))
      res->size++;    // one additional field for the ring (before the data)
    //Print("found type %s at real-pos %d",start,res->size);
    elem=(newstruct_member)omAlloc0(sizeof(*elem));
    // read name:
    p++;
    while (*p==' ') p++;
    start=p;
    while (isalpha(*p)) p++;
    c=*p;
    *p='\0';
    elem->typ=t;
    elem->pos=res->size;
    if (*start=='\0') /*empty name*/
    {
      WerrorS("empty name for element");
      goto error_in_newstruct_def;
    }
    elem->name=omStrDup(start);
    //Print(" name:%s\n",start);
    elem->next=res->member;
    res->member=elem;
    res->size++;

    // next ?
    *p=c;
    while (*p==' ') p++;
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

BOOLEAN newstruct_set_proc(const char *bbname,const char *func, int args,procinfov pr)
{
  int id=0;
  blackboxIsCmd(bbname,id);
  blackbox *bb=getBlackboxStuff(id);
  newstruct_desc desc=(newstruct_desc)bb->data;
  newstruct_proc p=(newstruct_proc)omAlloc(sizeof(*p));
  p->next=desc->procs; desc->procs=p;
  if(!IsCmd(func,p->t))
  {
    if (func[1]=='\0') p->t=func[0];
    else
    {
      Werror(">>%s<< is not e kernel command",func);
      return TRUE;
    }
  }
  p->args=args;
  p->p=pr; pr->ref++;
  return FALSE;
}
