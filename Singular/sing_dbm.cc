/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

//**************************************************************************/
//
// $Id: sing_dbm.cc,v 1.5 1997-08-08 12:59:30 obachman Exp $
//
//**************************************************************************/
//  'sing_dbm.cc' containes command to handle dbm-files under
// Singular. 
//
//**************************************************************************/

#include "mod2.h"

#ifdef HAVE_DBM

#  include <stdio.h>
#  include <fcntl.h>
#  include <errno.h>
#  include "tok.h"
#  include "febase.h"
#  include "mmemory.h"
#  include "ipid.h"
#  include "silink.h"
#  include "sing_dbm.h"

/* These are the routines in dbm. */
#  include "ndbm.h"
typedef struct {
  DBM *db;        // pointer to open database
  int first;      // firstkey to look for?
} DBM_info;

//extern char *getenv(char *var);
extern char *index(char *str, char c);

//**************************************************************************/
BOOLEAN dbOpen(si_link l, short flag)
{
  char *mode = "r";
  DBM_info *db;
  int dbm_flags = O_RDONLY | O_CREAT;  // open database readonly as default

  if((flag & SI_LINK_OPEN)
  || (flag & SI_LINK_READ)
  || (flag & SI_LINK_WRITE)) {
    if (l->mode[0] == '\0' || (strcmp(l->mode, "r") == 0))
      flag = SI_LINK_READ;
    else flag = SI_LINK_READ | SI_LINK_WRITE;
  }

  if((flag & SI_LINK_READ) || (flag & SI_LINK_WRITE)) {
    if( l->data == NULL ) {
      if ((db = (DBM_info *)malloc(sizeof *db)) == 0) {
	errno = ENOMEM;
	return TRUE;
      }
      if( flag & SI_LINK_WRITE ) {
	dbm_flags = O_RDWR | O_CREAT;
	mode = "rw";
      }
      if( (db->db = dbm_open(l->name, dbm_flags, 0664 )) != NULL ) {
	db->first=1;
	if(flag & SI_LINK_WRITE) SI_LINK_SET_RW_OPEN_P(l);
	else SI_LINK_SET_R_OPEN_P(l);
	l->data=(void *)(db);
	FreeL(l->mode);
	l->mode=mstrdup(mode);
	return FALSE;
      }
      Werror("dbm_open of `%s` failed",l->name);
    }
  }
  return TRUE;
}

//**************************************************************************/
BOOLEAN dbClose(si_link l)
{
  DBM_info *db = l->data;

  dbm_close(db->db);
  free(db);
  l->data=NULL;  
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

//**************************************************************************/
static datum d_value;
leftv dbRead2(si_link l, leftv key)
{
  DBM_info *db = l->data;
  leftv v=NULL;
  if(l->data != NULL) {
    datum d_key;

    if(key!=NULL)
    {
      if (key->Typ()==STRING_CMD)
      {
        d_key.dptr = (char*)key->Data();
        d_key.dsize = strlen(d_key.dptr)+1;
        d_value = dbm_fetch(db->db, d_key);
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
    else {
      if(db->first) d_value = dbm_firstkey((DBM *)db->db);
      else d_value = dbm_nextkey((DBM *)db->db);

      v=(leftv)Alloc0(sizeof(sleftv));
      v->rtyp=STRING_CMD;
      if (d_value.dptr!=NULL) {
	v->data=mstrdup(d_value.dptr);
	db->first = 0;
      }
      else {
	v->data=mstrdup("");
	db->first = 1;
      }

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
//**************************************************************************/
BOOLEAN dbWrite(si_link l, leftv key)
{
  DBM_info *db = l->data;
  BOOLEAN b=TRUE;
  register int ret;

  if(l->data != NULL) {                      // is database opened ?
    if((key!=NULL) && (key->Typ()==STRING_CMD) ) { 
      if (key->next!=NULL) {                 // have a second parameter ?
	if(key->next->Typ()==STRING_CMD) {   // replace (key,value)
	  datum d_key, d_value;

	  d_key.dptr = (char *)key->Data();
	  d_key.dsize = strlen(d_key.dptr)+1;
	  d_value.dptr = (char *)key->next->Data();
	  d_value.dsize = strlen(d_value.dptr)+1;
	  ret  = dbm_store(db->db, d_key, d_value, DBM_REPLACE);
	  if(!ret ) b=FALSE;
	  else {
	    if(dbm_error(db->db)) {
	      Werror("DBM link I/O error. is '%s' readonly?", l->name);
	      dbm_clearerr(db->db);
	    }
	  }
	}
      } else {                               // delete (key)
	datum d_key;

	d_key.dptr = (char *)key->Data();
	d_key.dsize = strlen(d_key.dptr)+1;
	dbm_delete(db->db, d_key);
	b=FALSE;
      }
    } else {
      WerrorS("write(`DBM link`,`key string`,`data string`) expected");
    }
  }
  else
  {
    Werror("DBM-link `%s` not open in write",l->name);
  }
  return b;
}
//**************************************************************************/
char *dbStatus(si_link l, char *request)
{
  if (strcmp(request, "read") == 0)
  {
    if (SI_LINK_R_OPEN_P(l)) return "ready";
    else return "not ready";
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    else return "not ready";
  }
  else return "unknown status request";
}
//**************************************************************************/
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
#endif /* HAVE_DBM */
