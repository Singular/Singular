#include <Singular/mod2.h>
#include <Singular/tok.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/blackbox.h>

//#define BLACKBOX_DEVEL 1
#define MAX_BB_TYPES 256

static blackbox* blackboxTable[MAX_BB_TYPES];
static char *    blackboxName[MAX_BB_TYPES];
static int blackboxTableCnt=0;
#define BLACKBOX_OFFSET (MAX_TOK+1)
blackbox* getBlackboxStuff(const int t)
{
  return (blackboxTable[t-BLACKBOX_OFFSET]);
}


void blackbox_default_destroy(blackbox  *b, void *d)
{
  Werror("missing blackbox_destroy");
}
char *blackbox_default_String(blackbox *b,void *d)
{
  Werror("missing blackbox_String");
  return omStrDup("");
}
void *blackbox_default_Copy(blackbox *b,void *d)
{
  Werror("missing blackbox_Copy");
  return NULL;
}
void blackbox_default_Print(blackbox *b,void *d)
{
  char *s=b->blackbox_String(b,d);
  PrintS(s);
  omFree(s);
}
void *blackbox_default_Init(blackbox *b)
{
  return NULL;
}
BOOLEAN blackboxDefaultOp1(int op,leftv l, leftv r)
{
  if (op==TYPEOF_CMD)
  {
    l->data=omStrDup(getBlackboxName(r->Typ()));
    l->rtyp=STRING_CMD;
    return FALSE;
  }
  Werror("blackbox_Op1 of type %s(%d) for op %s(%d) not implemented",
     getBlackboxName(r->Typ()),r->Typ(),Tok2Cmdname(op),op);
  return TRUE;
}
BOOLEAN blackboxDefaultOp2(int op,leftv l, leftv r1, leftv r2)
{
  Werror("blackbox_Op2 of type %s(%d) for op %s(%d) not implemented",
     getBlackboxName(r1->Typ()),r1->Typ(),Tok2Cmdname(op),op);
  return TRUE;
}
BOOLEAN blackbox_default_Op3(int op,leftv l, leftv r1,leftv r2, leftv r3)
{
  Werror("blackbox_Op3 of type %s(%d) for op %s(%d) not implemented",
     getBlackboxName(r1->Typ()),r1->Typ(),Tok2Cmdname(op),op);
  return TRUE;
}
BOOLEAN blackbox_default_OpM(int op,leftv l, leftv r)
{
  Werror("blackbox_OpM of type %s(%d) for op %s(%d) not implemented",
     getBlackboxName(r->Typ()),r->Typ(),Tok2Cmdname(op),op);
  return TRUE;
}

BOOLEAN blackbox_default_Check(blackbox *b, void *d)
{
  return FALSE;
}
int setBlackboxStuff(blackbox *bb, const char *n)
{
  int where=-1;
  if (MAX_BB_TYPES<=blackboxTableCnt)
  {
    // second try, find empty slot from removed bb:
    for (int i=0;i<MAX_BB_TYPES;i++)
    {
      if (blackboxTable[i]==NULL) { where=i; break; }
    }
  }
  else
  {
    where=blackboxTableCnt;
    blackboxTableCnt++;
  }
  if (where==-1)
  {
    WerrorS("too many bb types defined");
    return 0;
  }
  else
  {
    blackboxTable[where]=bb;
    blackboxName[where]=omStrDup(n);
#ifdef BLACKBOX_DEVEL
    Print("define bb:name=%s:rt=%d (table:cnt=%d)\n",blackboxName[where],where+BLACKBOX_OFFSET,where);
#endif
    if (bb->blackbox_destroy==NULL) bb->blackbox_destroy=blackbox_default_destroy;
    if (bb->blackbox_String==NULL)  bb->blackbox_String=blackbox_default_String;
    if (bb->blackbox_Print==NULL)   bb->blackbox_Print=blackbox_default_Print;
    if (bb->blackbox_Init==NULL)    bb->blackbox_Init=blackbox_default_Init;
    if (bb->blackbox_Copy==NULL)    bb->blackbox_Copy=blackbox_default_Copy;
    if (bb->blackbox_Op1==NULL)     bb->blackbox_Op1=blackboxDefaultOp1;
    if (bb->blackbox_Op2==NULL)     bb->blackbox_Op2=blackboxDefaultOp2;
    if (bb->blackbox_Op3==NULL)     bb->blackbox_Op3=blackbox_default_Op3;
    if (bb->blackbox_OpM==NULL)     bb->blackbox_OpM=blackbox_default_OpM;
    if (bb->blackbox_Check==NULL)   bb->blackbox_Check=blackbox_default_Check;
    return where+BLACKBOX_OFFSET;
  }
}
void removeBlackboxStuff(const int rt)
{
  omfree(blackboxTable[rt-BLACKBOX_OFFSET]);
  omfree(blackboxName[rt-BLACKBOX_OFFSET]);
  blackboxTable[rt-BLACKBOX_OFFSET]=NULL;
  blackboxName[rt-BLACKBOX_OFFSET]=NULL;
}
const char *getBlackboxName(const int t)
{
 char *b=blackboxName[t-BLACKBOX_OFFSET];
  if (b!=NULL) return b;
  else         return "";
}
int blackboxIsCmd(const char *n, int & tok)
{
  for(int i=blackboxTableCnt-1;i>=0;i--)
  {
    if(strcmp(n,blackboxName[i])==0)
    {
#ifdef BLACKBOX_DEVEL
      Print("found bb:%s:%d (table:%d)\n",n,i+BLACKBOX_OFFSET,i);
#endif
      tok=i+BLACKBOX_OFFSET;
      return ROOT_DECL;
    }
  }
  return 0;
}

void printBlackboxTypes()
{
  for(int i=blackboxTableCnt-1;i>=0;i--)
  {
    if (blackboxName[i]!=NULL)
      Print("type %d: %s\n",i,blackboxName[i]);
  }
}
