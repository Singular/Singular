/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

//**************************************************************************
//
// $Id: sing_dbm.cc,v 1.3 1997-04-08 16:45:32 Singular Exp $
//
//**************************************************************************
//  'sing_dbm.cc' containes command to handle dbm-files under
// Singular. Don't forget to compile Singular with the option -ldbm
//
//**************************************************************************

#include "mod2.h"

#ifdef HAVE_DBM

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "ipid.h"
#include "silink.h"
#include "sing_dbm.h"


static BOOLEAN dbOpened = FALSE;

/* The data and key structure.  This structure is defined for compatibility. */
typedef struct {
        char *dptr;
        int   dsize;
} datum;


/* These are the routines in dbm. */


extern "C" {
extern int dbminit(const char *__file);
extern int store(datum __key, datum __content);
extern datum fetch(datum);
extern datum firstkey(void);
extern datum nextkey(datum);
}

//extern char *getenv(char *var);
extern char *index(char *str, char c);

//**************************************************************************
BOOLEAN dbOpen(si_link l, short flag)
{
  if((flag==SI_LINK_OPEN)
  || (flag==SI_LINK_READ)
  || (flag==SI_LINK_WRITE))
  {
    if (!dbOpened)
    {
      if(dbminit(l->name) == 0)
      {
        dbOpened=TRUE;
        SI_LINK_SET_RW_OPEN_P(l);
        l->data=NULL;
        FreeL(l->mode);
        l->mode=mstrdup("rw");
        return FALSE;
      }
      Werror("dbminit of `%s` failed",l->name);
    }
    else
      Werror("only one DBM link allowed:`%s`",l->name);
  }
  return TRUE;
}

//**************************************************************************
BOOLEAN dbClose(si_link l)
{
  dbOpened=FALSE;
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

//**************************************************************************
static datum d_value;
leftv dbRead2(si_link l, leftv key)
{
  leftv v=NULL;
  if(dbOpened)
  {
    datum d_key;

    if(key!=NULL)
    {
      if (key->Typ()==STRING_CMD)
      {
        d_key.dptr = (char*)key->Data();
        d_key.dsize = strlen(d_key.dptr)+1;
        d_value = fetch(d_key);
        v=(leftv)Alloc0(sizeof(sleftv));
        if (d_value.dptr!=NULL) v->data=mstrdup(d_value.dptr);
        else                    v->data=mstrdup("");
        v->rtyp=STRING_CMD;
      }
      else
      {
        WerrorS("read(`link`,`string`) expected");
      }
    }
    else
    {
      if (l->data==NULL)
      {
        d_value = firstkey();
        l->data=(void *)&d_value;
      }
      else
      {
        d_value = nextkey(*(datum *)l->data);
      }
      v=(leftv)Alloc0(sizeof(sleftv));
      v->rtyp=STRING_CMD;
      if (d_value.dptr!=NULL) v->data=mstrdup(d_value.dptr);
      else                    v->data=mstrdup("");
    }
  }
  else
  {
    Werror("DBM-link `%s` not open in read",l->name);
  }
  return v;
}
leftv dbRead1(si_link l)
{
  return dbRead2(l,NULL);
}
//**************************************************************************
BOOLEAN dbWrite(si_link l, leftv key)
{
  BOOLEAN b=TRUE;

  if(dbOpened)
  {
    if((key!=NULL)
    && (key->Typ()==STRING_CMD)
    && (key->next!=NULL)
    && (key->next->Typ()==STRING_CMD))
    {
      datum d_key, d_value;
      d_key.dptr = (char *)key->Data();
      d_key.dsize = strlen(d_key.dptr)+1;
      d_value.dptr = (char *)key->next->Data();
      d_value.dsize = strlen(d_value.dptr)+1;
      store(d_key, d_value);
      b=FALSE;
    }
    else
    {
      WerrorS("write(`DBM link`,`key string`,`data string`) expected");
    }
  }
  else
  {
    Werror("DBM-link `%s` not open in write",l->name);
  }
  return b;
}
//**************************************************************************
char *dbStatus(si_link l, char *request)
{
  if ((strcmp(request, "read") == 0)
  ||  (strcmp(request, "write") == 0))
  {
    if (SI_LINK_R_OPEN_P(l)) return "ready";
    else                     return "not ready";
  }
  else return "unknown status request";
}
//**************************************************************************
si_link_extension slInitDBMExtension(si_link_extension s)
{
  s->Open=dbOpen;
  s->Close=dbClose;
  s->Read=dbRead1;
  s->Read2=dbRead2;
  s->Write=dbWrite;
  //s->Dump=NULL;
  //s->GetDump=NULL;
  s->Status=dbStatus;
  s->type="DBM";
  return s;
}
#endif
