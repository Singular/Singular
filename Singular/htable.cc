/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "kernel/mod2.h"

#include "resources/hash_me.h"
#include "reporter/reporter.h"
#include "Singular/htable.h"

stablerec* t_createTable(int s)
{
  stablerec* t=(stablerec*)omAlloc(sizeof(stablerec));
  t->max=s;
  t->t=(telem*)omAlloc0(s*sizeof(telem));
  t->ref=1;
  return t;
}

void t_destroyTable(stablerec* t)
{
  t->ref--;
  if (t->ref<=0)
  {
    for(int i=t->max-1;i>=0;i--)
    {
      telem p=t->t[i];
      while(p!=NULL)
      {
        omFree(p->key);
        p->val.CleanUp();
        telem pp=p;
        p=p->next;
        omFreeSize(pp,sizeof(stelem));
      }
    }
    omFreeSize(t->t,t->max*sizeof(telem));
    omFreeSize(t,sizeof(stablerec));
  }
}

stablerec* copyTable(stablerec* t)
{
  t->ref++;
  return t;
}

char* stringTable(stablerec* t)
{
  StringSetS("table:\n");
  char *s;
  for(int i=t->max-1;i>=0;i--)
  {
    telem p=t->t[i];
    while(p!=NULL)
    {
        StringAppendS(p->key);
        StringAppendS(" -> ");
        s=p->val.String();
        StringAppendS(s);
        omFree(s);
        StringAppendS("\n");
        telem pp=p;
        p=p->next;
    }
  }
  return StringEndS();
}

/// find the entry to key s
telem t_findTable(stablerec* t,const char *s)
{
  uint32_t h=hashlittle(s,strlen(s));
  h=h%t->max;
  telem p=t->t[h];
  while(p!=NULL)
  {
    if (strcmp(s,p->key)==0) return p;
    p=p->next;
  }
  return NULL;
}

/// find the data to key s
leftv t_findTabelVal(stablerec* t,const char *s)
{
  telem p=t_findTable(t,s);
  if (p==NULL) return NULL;
  return &(p->val);
}

/// add a new entry (key s, data v) to table t, eats s, copies v
void t_addTable(stablerec* t,char *s, leftv v)
{
  uint32_t h=hashlittle(s,strlen(s));
  telem p=(telem)omAlloc(sizeof(*p));
  p->next=NULL;
  p->key=s;
  p->val.Init();
  p->val.rtyp=v->Typ();
  p->val.data=v->CopyD();
  p->hash=h;
  h=h%t->max;
  p->next=t->t[h];
  t->t[h]=p;
}
//-------------------------------------------------------
void htable_Print(stablerec *d)
{
  stablerec* lt=(stablerec*)d;
  char* s=stringTable(lt);
  PrintS(s);
  omFree(s);
  int cnt=0;int cnt2=0;
  for(int i=0;i<lt->max;i++)
  {
    if (lt->t[i]!=NULL)
    {
      cnt++;
      telem p=lt->t[i];
      while(p!=NULL) { cnt2++;p=p->next;}
    }
  }
  Print("%d columns, %d entries, size:%d",cnt,cnt2,lt->max);
}
