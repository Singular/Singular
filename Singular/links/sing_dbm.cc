/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

//**************************************************************************/
//  'sing_dbm.cc' containes command to handle dbm-files under
// Singular.
//
//**************************************************************************/

#include "kernel/mod2.h"

#  include <fcntl.h>

#ifdef HAVE_DBM

#  include "Singular/tok.h"
#  include "Singular/ipid.h"
#  include "Singular/links/silink.h"
#  include "Singular/links/sing_dbm.h"

// #ifdef __CYGWIN__
// #  define USE_GDBM
// #  define BLOCKSIZE 1
// #  define GDBM_STATIC
// #  include <gdbm.h>
// #endif

#ifdef USE_GDBM
typedef struct {
  GDBM_FILE db;        // pointer to open database
  int first;      // firstkey to look for?
  datum actual;  // the actual key
} GDBM_info;

//**************************************************************************/
LINKAGE BOOLEAN dbOpen(si_link l, short flag, leftv u)
{
  if (FE_OPT_NO_SHELL_FLAG) {WerrorS("no links allowed");return TRUE;}
  char *mode = "r";
  GDBM_info *db;
  datum d_key;
  //  int dbm_flags = O_RDONLY | O_CREAT;  // open database readonly as default
  int read_write= GDBM_READER;

  if(flag & SI_LINK_WRITE)
  {
 //    if((l->mode==NULL)
//     || ((l->mode[0]!='w')&&(l->mode[1]!='w')))
//     {
//       // request w- open, but mode is not "w" nor "rw" => fail
//       return TRUE;
//    }
    //    dbm_flags = O_RDWR | O_CREAT;
    read_write =  GDBM_WRCREAT | GDBM_NOLOCK;
    mode = "rw";
  }
  if(flag & SI_LINK_READ)
    {
      if (strcmp(l->mode,"rw")==0) mode="rw";
    }
  //if (((db = (DBM_info *)omAlloc(sizeof *db)) != NULL)
  //&&((db->db = dbm_open(l->name, dbm_flags, 0664 )) != NULL ))
  db = (GDBM_info *)omAlloc0(sizeof *db);
  if((db->db = gdbm_open(l->name, BLOCKSIZE, read_write, 0664, 0)) != NULL )
  {
//     if (db->first) // first created?
//       {
//         db->actual=gdbm_firstkey(db->db);
//       }
//     else
//       {
//     d_key=db->actual;
//     if (d_key.dptr!=NULL)
//       {
//         db->actual=gdbm_nextkey(db->db,db->actual);
//       }
//     else { db->actual=gdbm_firstkey(db->db); }
//      }
    db->first=1;
    if(flag & SI_LINK_WRITE)
      SI_LINK_SET_W_OPEN_P(l);
    else
      SI_LINK_SET_R_OPEN_P(l);
    l->data=(void *)(db);
    omFreeBinAddr(l->mode);
    l->mode=omStrDup(mode);
    return FALSE;
  }
  Print("%d/%s",gdbm_errno,gdbm_strerror(gdbm_errno));
  return TRUE;
}

//**************************************************************************/
LINKAGE BOOLEAN dbClose(si_link l)
{
  GDBM_info *db = (GDBM_info *)l->data;
  gdbm_sync(db->db);
  gdbm_close(db->db);
  omFreeSize((ADDRESS)db,(sizeof *db));
  l->data=NULL;
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

//**************************************************************************/
STATIC_VAR datum d_value;
LINKAGE leftv dbRead2(si_link l, leftv key)
{
  GDBM_info *db = (GDBM_info *)l->data;
  // GDBM_info *db;
//   db = (GDBM_info *)omAlloc0(sizeof *db);
//   db = (GDBM_info *)l->data;
  leftv v=NULL;
  datum d_key;
  int flag;

  if (!SI_LINK_R_OPEN_P(l)) //exceptions
    //  if (strcmp(l->mode,"rw")==0) //rw-mode
    {
      if (!SI_LINK_CLOSE_P(l))
      {
        if (!dbClose(l)) {PrintS("cannot close link!\n");}
      }
      //(SI_LINK_CLOSE_P(l)) automatically
      if (dbOpen(l, SI_LINK_READ)) return NULL;
    }
  if (SI_LINK_RW_OPEN_P(l)) {PrintS("I/O Error!\n");}

  if(key!=NULL)
  {
    if (key->Typ()==STRING_CMD)
    {
      d_key.dptr = (char*)key->Data();
      d_key.dsize = strlen(d_key.dptr)+1;
      d_value = gdbm_fetch(db->db, d_key);
      v=(leftv)omAlloc0Bin(sleftv_bin);
      if (d_value.dptr!=NULL) v->data=omStrDup(d_value.dptr);
      else                    v->data=omStrDup("");
      v->rtyp=STRING_CMD;
    }
    else
    {
      WerrorS("read(`GDBM link`,`string`) expected");
    }
  }
  else
  {
    if (db->first)
    {
      db->first=0;
      d_key = gdbm_firstkey(db->db);
   //    db->actual=d_key;
//       Print("firstkey:%s\n",d_key.dptr);
    }
    else
    {
      if (db->actual.dptr==NULL)
      {
        db->actual=gdbm_firstkey(db->db);
      }
      d_key = gdbm_nextkey(db->db,db->actual);
      db->actual=d_key;
      if (d_key.dptr==NULL)
      {
        db->first=1;
      // Print("nextkey:NULL\n");
      }
     //  else
//       Print("nextkey:%s\n",d_key.dptr);
    }

    if (d_key.dptr!=NULL)
      d_value = gdbm_fetch(db->db, d_key);
    else
      d_value.dptr=NULL;

    v=(leftv)omAlloc0Bin(sleftv_bin);
    v->rtyp=STRING_CMD;
    if (d_value.dptr!=NULL)
    {
      v->data=omStrDup(d_key.dptr);
      db->first = 0;
    }
    else
    {
      v->data=omStrDup("");
      //      db->first = 1;
    }

  }
  return v;
}
LINKAGE leftv dbRead1(si_link l)
{
  return dbRead2(l,NULL);
}
//**************************************************************************/
LINKAGE BOOLEAN dbWrite(si_link l, leftv key)
{
  GDBM_info *db = (GDBM_info *)l->data;
 //  GDBM_info *db;
//   db = (GDBM_info *)omAlloc0(sizeof *db);
//   db = (GDBM_info *)l->data;
  BOOLEAN b=TRUE;
  REGISTER int ret;

  if (strcmp(l->mode,"rw")!=0) // r-mode
    {
      PrintS("Write error on readonly source\n");
    }
  else //rw-mode
    {
      if (!SI_LINK_W_OPEN_P(l)) //exceptions
      {
        if (!SI_LINK_CLOSE_P(l))
        {
          if (!dbClose(l)) {PrintS("close error\n");};
        }
        if (!dbOpen(l,SI_LINK_WRITE)) {PrintS("open_for_write error\n");}
      }
    }

  if((key!=NULL) && (key->Typ()==STRING_CMD) )
  {
    if (key->next!=NULL)                   // have a second parameter ?
    {
      if(key->next->Typ()==STRING_CMD)     // replace (key,value)
      {
        datum d_key, d_value;

        d_key.dptr = (char *)key->Data();
        d_key.dsize = strlen(d_key.dptr)+1;
        d_value.dptr = (char *)key->next->Data();
        d_value.dsize = strlen(d_value.dptr)+1;
        ret  = gdbm_store(db->db, d_key, d_value, GDBM_REPLACE);
//         db->actual=d_key;
        if (ret==-1) {PrintS("reader calls gdbm_store!");}
        if (ret==0)
          { b=FALSE; }
        else
        {
          //          if(gdbm_error(db->db))
          if (gdbm_errno != 0)
          {
            Werror("GDBM link I/O error: '%s' ", gdbm_errno);
            //            Print(gdbm_strerror(gdbm_errno));
            //dbm_clearerr(db->db);
            //            gdbm_errno=0;
          }
        }
      }
    }
    else
    {                               // delete (key)
      datum d_key;

      d_key.dptr = (char *)key->Data();
      d_key.dsize = strlen(d_key.dptr)+1;
 //      db->actual=gdbm_nextkey(db->db,d_key);
      gdbm_delete(db->db, d_key);
      b=FALSE;
    }
  }
  else
  {
    WerrorS("write(`GDBM link`,`key string` [,`data string`]) expected");
  }
  gdbm_sync(db->db);
  return b;
}
#endif /* USE_GDBM */

#ifndef USE_GDBM
/* These are the routines in dbm. */
#  include "ndbm.h"
typedef struct {
  DBM *db;        // pointer to open database
  int first;      // firstkey to look for?
} DBM_info;

//**************************************************************************/
LINKAGE BOOLEAN dbOpen(si_link l, short flag, leftv /*u*/)
{
  const char *mode = "r";
  DBM_info *db;
  int dbm_flags = O_RDONLY | O_CREAT;  // open database readonly as default

  if((l->mode!=NULL)
  && ((l->mode[0]=='w')||(l->mode[1]=='w')))
  {
    dbm_flags = O_RDWR | O_CREAT;
    mode = "rw";
    flag|=SI_LINK_WRITE|SI_LINK_READ;
  }
  else if(flag & SI_LINK_WRITE)
  {
    // request w- open, but mode is not "w" nor "rw" => fail
    return TRUE;
  }
  //if (((db = (DBM_info *)omAlloc(sizeof *db)) != NULL)
  //&&((db->db = dbm_open(l->name, dbm_flags, 0664 )) != NULL ))
  db = (DBM_info *)omAlloc(sizeof *db);
  if((db->db = dbm_open(l->name, dbm_flags, 0664 )) != NULL )
  {
    db->first=1;
    if(flag & SI_LINK_WRITE)
      SI_LINK_SET_RW_OPEN_P(l);
    else
      SI_LINK_SET_R_OPEN_P(l);
    l->data=(void *)(db);
    omFreeBinAddr(l->mode);
    l->mode=omStrDup(mode);
    return FALSE;
  }
  return TRUE;
}

//**************************************************************************/
LINKAGE BOOLEAN dbClose(si_link l)
{
  DBM_info *db = (DBM_info *)l->data;

  dbm_close(db->db);
  omFreeSize((ADDRESS)db,(sizeof *db));
  l->data=NULL;
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

//**************************************************************************/
STATIC_VAR datum d_value;
LINKAGE leftv dbRead2(si_link l, leftv key)
{
  DBM_info *db = (DBM_info *)l->data;
  leftv v=NULL;
  datum d_key;

  if(key!=NULL)
  {
    if (key->Typ()==STRING_CMD)
    {
      d_key.dptr = (char*)key->Data();
      d_key.dsize = strlen(d_key.dptr)+1;
      d_value = dbm_fetch(db->db, d_key);
      v=(leftv)omAlloc0Bin(sleftv_bin);
      if (d_value.dptr!=NULL) v->data=omStrDup(d_value.dptr);
      else                    v->data=omStrDup("");
      v->rtyp=STRING_CMD;
    }
    else
    {
      WerrorS("read(`DBM link`,`string`) expected");
    }
  }
  else
  {
    if(db->first)
      d_value = dbm_firstkey((DBM *)db->db);
    else
      d_value = dbm_nextkey((DBM *)db->db);

    v=(leftv)omAlloc0Bin(sleftv_bin);
    v->rtyp=STRING_CMD;
    if (d_value.dptr!=NULL)
    {
      v->data=omStrDup(d_value.dptr);
      db->first = 0;
    }
    else
    {
      v->data=omStrDup("");
      db->first = 1;
    }

  }
  return v;
}
LINKAGE leftv dbRead1(si_link l)
{
  return dbRead2(l,NULL);
}
//**************************************************************************/
LINKAGE BOOLEAN dbWrite(si_link l, leftv key)
{
  DBM_info *db = (DBM_info *)l->data;
  BOOLEAN b=TRUE;
  int ret;

  // database is opened
  if((key!=NULL) && (key->Typ()==STRING_CMD) )
  {
    if (key->next!=NULL)                   // have a second parameter ?
    {
      if(key->next->Typ()==STRING_CMD)     // replace (key,value)
      {
        datum d_key, d_value;

        d_key.dptr = (char *)key->Data();
        d_key.dsize = strlen(d_key.dptr)+1;
        d_value.dptr = (char *)key->next->Data();
        d_value.dsize = strlen(d_value.dptr)+1;
        ret  = dbm_store(db->db, d_key, d_value, DBM_REPLACE);
        if(!ret )
          b=FALSE;
        else
        {
          if(dbm_error(db->db))
          {
            Werror("DBM link I/O error. Is '%s' readonly?", l->name);
            dbm_clearerr(db->db);
          }
        }
      }
    }
    else
    {                               // delete (key)
      datum d_key;

      d_key.dptr = (char *)key->Data();
      d_key.dsize = strlen(d_key.dptr)+1;
      dbm_delete(db->db, d_key);
      b=FALSE;
    }
  }
  else
  {
    WerrorS("write(`DBM link`,`key string` [,`data string`]) expected");
  }
  return b;
}
//**************************************************************************/
//char *dbStatus(si_link l, char *request)
//{
//  if (strcmp(request, "read") == 0)
//  {
//    if (SI_LINK_R_OPEN_P(l))
//      return "ready";
//    else
//      return "not ready";
//  }
//  else if (strcmp(request, "write") == 0)
//  {
//    if (SI_LINK_W_OPEN_P(l))
//      return "ready";
//    else
//      return "not ready";
//  }
//  else return "unknown status request";
//}
//**************************************************************************/

#endif /* USE_GDBM */
#endif /* HAVE_DBM */
