/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: silink.cc,v 1.10 1997-04-18 11:25:01 obachman Exp $ */

/*
* ABSTRACT: general interface to links
*/

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "subexpr.h"
#include "ipid.h"
#include "silink.h"
#include "ipshell.h"
#include "ring.h"
#include "lists.h"
#include "ideals.h"
#include "numbers.h"
#include "intvec.h"

/* declarations */
static BOOLEAN DumpAscii(FILE *fd, idhdl h);
static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h);
static char* GetIdString(idhdl h);
static int DumpRhs(FILE *fd, idhdl h);
static BOOLEAN DumpQring(FILE *fd, idhdl h, char *type_str);
static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl);

/* ====================================================================== */
si_link_extension si_link_root=NULL;

BOOLEAN slInit(si_link l, char *istr)
{
  char *type = NULL, *mode = NULL, *name = NULL;
  int i = 0, j;
  
  // set mode and type
  if (istr != NULL)
  {
    // find the first colon char in istr
    i = 0;
    while (istr[i] != ':' && istr[i] != '\0') i++;
    if (istr[i] == ':')
    {
      // if found, set type
      if (i > 0)
      {
        istr[i] = '\0';
        type = mstrdup(istr);
        istr[i] = ':';
      }
      // and check for mode
      j = ++i;
      while (istr[j] != ' ' && istr[j] != '\0') j++;
      if (j > i)
      {
        mode = mstrdup(&(istr[i]));
        mode[j - i] = '\0';
      }
      // and for the name
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = mstrdup(&(istr[j]));
    }
    else // no colon find -- string is entire name
    {
      j=0;
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = mstrdup(&(istr[j]));
    }
  }

  // set the link extension
  if (type != NULL)
  {
    si_link_extension s = si_link_root;
    
    while (s != NULL && (strcmp(s->type, type) != 0)) s = s->next;

    if (s != NULL)
      l->m = s;
    else
    {
      Warn("Found unknown link type: %s", type);
      Warn("Use default link type: %s", si_link_root->type);
      l->m = si_link_root;
    }
    FreeL(type);
  }
  else
    l->m = si_link_root;

  l->name = (name != NULL ? name : mstrdup(""));
  l->mode = (mode != NULL ? mode : mstrdup(""));
  l->ref = 1;
  return FALSE;
}

void slCleanUp(si_link l)
{
  (l->ref)--;
  if (l->ref == 0)
  {
    if (SI_LINK_OPEN_P(l)) slClose(l);
    FreeL((ADDRESS)l->name);
    FreeL((ADDRESS)l->mode);
    memset((void *) l, 0, sizeof(ip_link));
  }
}

void slKill(si_link l)
{
  slCleanUp(l);
  if (l->ref == 0)
    Free((ADDRESS)l, sizeof(ip_link));
}

char* slStatus(si_link l, char *request)
{
  if (l == NULL) return "empty link";
  else if (l->m == NULL) return "unknown link type";
  else if (strcmp(request, "type") == 0) return l->m->type;
  else if (strcmp(request, "mode") == 0) return l->mode;
  else if (strcmp(request, "name") == 0) return l->name;
  else if (strcmp(request, "open") == 0)
  {
    if (SI_LINK_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (strcmp(request, "openread") == 0)
  {
    if (SI_LINK_R_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (strcmp(request, "openwrite") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (l->m->Status == NULL) return "unknown status request";
  else return l->m->Status(l, request);
}

//--------------------------------------------------------------------------
BOOLEAN slOpen(si_link l, short flag)
{
  BOOLEAN res;
  
  if (l->m == NULL) slInit(l, "");

  if (SI_LINK_OPEN_P(l))
  {
    Warn("open: link of type: %s, mode: %s, name: %s is already open",
         l->m->type, l->mode, l->name);
    return FALSE;
  }
  else if (l->m->Open != NULL) res = l->m->Open(l, flag);
  else res = TRUE;

  if (res)
    Werror("open: Error for link of type: %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  return res;
}

BOOLEAN slClose(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_OPEN_P(l)) return FALSE;
  else if (l->m->Close != NULL) return res = l->m->Close(l);
  else res = TRUE;
  
  if (res)
    Werror("close: Error for link of type: %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  return res;
}

leftv slRead(si_link l, leftv a)
{
  leftv v = NULL;
  if( ! SI_LINK_R_OPEN_P(l)) // open r ?
  {
    if (slOpen(l, SI_LINK_READ)) return NULL;
  }

  if (SI_LINK_R_OPEN_P(l))
  { // open r
    if (a==NULL)
    {
      if (l->m->Read != NULL) v = l->m->Read(l);
    }
    else
    {
      if (l->m->Read2 != NULL) v = l->m->Read2(l,a);
    }
  }
  else
  {
    Werror("read: Error to open link of type %s, mode: %s, name: %s for reading",
           l->m->type, l->mode, l->name);
    return NULL;
  }
  
  // here comes the eval:
  if (v != NULL) v->Eval();
  else
    Werror("read: Error for link of type %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  return v;
}

BOOLEAN slWrite(si_link l, leftv v)
{
  BOOLEAN res;
  
  if(! SI_LINK_W_OPEN_P(l)) // open w ?
  {
    if (slOpen(l, SI_LINK_WRITE)) return TRUE;
  }

  if(SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Write != NULL) res = l->m->Write(l,v);
    else res = TRUE;

    if (res)
      Werror("write: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    return res;
  }
  else
  {
    Werror("write: Error to open link of type %s, mode: %s, name: %s for writing",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}

BOOLEAN slDump(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_W_OPEN_P(l)) // open w ?
  {
    if (slOpen(l, SI_LINK_WRITE)) return TRUE;
  }

  if(SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Dump != NULL) res = l->m->Dump(l);
    else res = TRUE;

    if (res)
      Werror("dump: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    return res;
  }
  else
  {
    Werror("dump: Error to open link of type %s, mode: %s, name: %s for writing",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}

BOOLEAN slGetDump(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_R_OPEN_P(l)) // open r ?
  {
    if (slOpen(l, SI_LINK_READ)) return TRUE;
  }

  if(SI_LINK_R_OPEN_P(l))
  { // now open r
    if (l->m->GetDump != NULL) res = l->m->GetDump(l);
    else res = TRUE;

    if (res)
      Werror("getdump: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    return res;
  }
  else
  {
    Werror("dump: Error open link of type %s, mode: %s, name: %s for reading",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}


/* =============== ASCII ============================================= */
BOOLEAN slOpenAscii(si_link l, short flag)
{
  char *mode;
  if (flag == SI_LINK_OPEN)
  {
    if (l->mode[0] != '\0' && (strcmp(l->mode, "r") == 0))
      flag = SI_LINK_READ;
    else flag = SI_LINK_WRITE;
  }

  if (flag == SI_LINK_READ) mode = "r";
  else if (strcmp(l->mode, "w") == 0) mode = "w";
  else mode = "a";
    
        
  if (l->name[0] == '\0')
  {
    // stdin or stdout
    if (flag == SI_LINK_READ)
    {
      l->data = (void *) stdin;
      mode = "r";
    }
    else
    {
      l->data = (void *) stdout;
      mode = "a";
    }
  }
  else
  {
    // normal ascii link to a file
    FILE *outfile;
    char *filename=l->name;

    if(filename[0]=='>')
    {
      if (filename[1]=='>')
      {
        filename+=2;
        mode = "a";
      }
      else
      {
        filename++;
        mode="w";
      }
    }
    outfile=fopen(filename,mode);
    if (outfile!=NULL) l->data = (void *) outfile;
    else return TRUE;
  }

  FreeL(l->mode);
  l->mode = mstrdup(mode);
  SI_LINK_SET_OPEN_P(l, flag);
  return FALSE;
}

BOOLEAN slCloseAscii(si_link l)
{
  SI_LINK_SET_CLOSE_P(l);
  if (l->name[0] != '\0')
  {
    return (fclose((FILE *)l->data)!=0);
  }
  return FALSE;
}

leftv slReadAscii(si_link l)
{
  FILE * fp=(FILE *)l->data;
  char * buf=NULL;
  if (fp!=NULL && l->name[0] != '\0')
  {
    fseek(fp,0L,SEEK_END);
    long len=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    buf=(char *)AllocL((int)len+1);
    if (BVERBOSE(V_READING)) Print("//Reading %d chars\n",len);
    fread( buf, len, 1, fp);
    buf[len]='\0';
  }
  else
  {
    PrintS("? "); mflush();
    buf=(char *)AllocL(80);
    fe_fgets_stdin(buf,80);
  }
  leftv v=(leftv)Alloc0(sizeof(sleftv));
  v->rtyp=STRING_CMD;
  v->data=buf;
  return v;
}
BOOLEAN slWriteAscii(si_link l, leftv v)
{
  FILE *outfile=(FILE *)l->data;
  BOOLEAN err=FALSE;
  char *s;
  while (v!=NULL)
  {
    s = v->String();
    // free v ??
    if (s!=NULL)
    {
      fprintf(outfile,"%s\n",s);
      FreeL((ADDRESS)s);
    }
    else
    {
      Werror("cannot convert to string");
      err=TRUE;
    }
    v = v->next;
  }
  fflush(outfile);
  return err;
}

char* slStatusAscii(si_link l, char* request)
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

/*------------------ Dumping in Ascii format -----------------------*/

BOOLEAN slDumpAscii(si_link l)
{
  FILE *fd = (FILE *) l->data;
  idhdl h = idroot, rh = currRingHdl;
  BOOLEAN status = DumpAscii(fd, h);

  if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh, TRUE);
  fflush(fd);
  
  return status;
}

// we do that recursively, to dump ids in the the order in which they 
// were actually defined
static BOOLEAN DumpAscii(FILE *fd, idhdl h)
{
  if (h == NULL) return FALSE;

  if (DumpAscii(fd, IDNEXT(h))) return TRUE;

  // need to set the ring before writing it, otherwise we get in
  // trouble with minpoly
  if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    rSetHdl(h, TRUE);

  if (DumpAsciiIdhdl(fd, h)) return TRUE;

  if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    return DumpAscii(fd, IDRING(h)->idroot);
  else
    return FALSE;
}

static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl)
{
  if (h == NULL) return FALSE;
  if (DumpAsciiMaps(fd, IDNEXT(h), rhdl)) return TRUE;

  if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    return DumpAsciiMaps(fd, IDRING(h)->idroot, h);
  else if (IDTYP(h) == MAP_CMD)
  {
    char *rhs;
    rSetHdl(rhdl, TRUE);
    rhs = ((leftv) h)->String();
    
    if (fprintf(fd, "setring %s;\n", IDID(rhdl)) == EOF) return TRUE;
    if (fprintf(fd, "%s %s = %s, %s;\n", Tok2Cmdname(MAP_CMD), IDID(h),
                IDMAP(h)->preimage, rhs) == EOF)
    {
      FreeL(rhs);
      return TRUE;
    }
    else
    {
      FreeL(rhs);
      return FALSE;
    }
  }
  else return FALSE;
}
  
static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h)
{
  char *type_str = GetIdString(h);
  idtyp type_id = IDTYP(h);

  // we do not throw an error if a wrong type was attempted to be dumped
  if (type_str == NULL) return FALSE;

  // handle qrings separately
  if (type_id == QRING_CMD) return DumpQring(fd, h, type_str);
  
  if (type_id == STRING_CMD && strcmp("LIB", IDID(h)) == 0)
  {
    if (fprintf(fd, "LIB \"%s\";\n", IDSTRING(h)) == EOF) return TRUE;
    else return FALSE;
  }
                                      
  // put type and name 
  if (fprintf(fd, "%s %s", type_str, IDID(h)) == EOF) return TRUE;

  // for matricies, append the dimension
  if (type_id == MATRIX_CMD)
  {
    ideal id = IDIDEAL(h);
    if (fprintf(fd, "[%d][%d]", id->nrows, id->ncols)== EOF) return TRUE;
  }
  else if (type_id == INTMAT_CMD)
  {
    if (fprintf(fd, "[%d][%d]", IDINTVEC(h)->rows(), IDINTVEC(h)->cols())
        == EOF) return TRUE;
  }
  // write the equal sign
  if (fprintf(fd, " = ") == EOF) return TRUE;

  // and the right hand side
  if (DumpRhs(fd, h) == EOF) return TRUE;
  
  // semicolon und tschuess
  if (fprintf(fd, ";\n") == EOF) return TRUE;

  return FALSE;
}

static char* GetIdString(idhdl h)
{
  idtyp type = IDTYP(h);
  
  switch(type)
  {
      case LIST_CMD:
      {
        lists l = IDLIST(h);
        int i, nl = l->nr + 1;
        char *name;

        for (i=0; i<nl; i++)
          if (GetIdString((idhdl) &(l->m[i])) == NULL) return NULL;
      }
      case INT_CMD:
      case INTVEC_CMD:
      case INTMAT_CMD:
      case STRING_CMD:
      case RING_CMD:
      case QRING_CMD:
      case PROC_CMD:
      case NUMBER_CMD:
      case POLY_CMD:
      case IDEAL_CMD:
      case VECTOR_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
        return Tok2Cmdname(type);
        
      case MAP_CMD:
      case LINK_CMD:
        return NULL;

      default:
       Warn("Error dump data of type %s", Tok2Cmdname(IDTYP(h)));
       return NULL;
  }
}

static BOOLEAN DumpQring(FILE *fd, idhdl h, char *type_str)
{
  char *ring_str = ((leftv) h)->String();
  if (fprintf(fd, "%s temp_ring = %s;\n", Tok2Cmdname(RING_CMD), ring_str)
              == EOF) return TRUE;
  if (fprintf(fd, "%s temp_ideal = %s;\n", Tok2Cmdname(IDEAL_CMD),
              iiStringMatrix((matrix) IDRING(h)->qideal, 1))
      == EOF) return TRUE;
  if (fprintf(fd, "attrib(temp_ideal, \"isSB\", 1);\n") == EOF) return TRUE;
  if (fprintf(fd, "%s %s = temp_ideal;\n", type_str, IDID(h)) == EOF)
    return TRUE;
  if (fprintf(fd, "kill temp_ring;\n") == EOF) return TRUE;
  else
  {
    FreeL(ring_str);
    return FALSE;
  }
}

  
static int DumpRhs(FILE *fd, idhdl h)
{
  idtyp type_id = IDTYP(h);

  if (type_id == LIST_CMD)
  {
    lists l = IDLIST(h);
    int i, nl = l->nr;

    fprintf(fd, "list(");
    
    for (i=0; i<nl; i++)
    {
      if (DumpRhs(fd, (idhdl) &(l->m[i])) == EOF) return EOF;
      fprintf(fd, ",");
    }
    if (nl > 0)
    {
      if (DumpRhs(fd, (idhdl) &(l->m[nl])) == EOF) return EOF;
    }
    fprintf(fd, ")");
  }
  else  if (type_id == PROC_CMD || type_id == STRING_CMD)
  {
    char *pstr = IDSTRING(h), c;
    fputc('"', fd);
    while (*pstr != '\0')
    {
      if (*pstr == '"') fputc('\\', fd);
      fputc(*pstr, fd);
      pstr++;
    }
    fputc('"', fd);
  }
  else
  {
    char *rhs = ((leftv) h)->String();

    if (rhs == NULL) return EOF;

    if (type_id == INTVEC_CMD) fprintf(fd, "intvec(");

    if (fprintf(fd, "%s", rhs) == EOF) return EOF;
    FreeL(rhs);

    if ((type_id == RING_CMD || type_id == QRING_CMD) &&
        IDRING(h)->minpoly != NULL)
    {
      StringSetS("");
      nWrite(IDRING(h)->minpoly);
      rhs = StringAppend("");
      if (fprintf(fd, "; minpoly = %s", rhs) == EOF) return EOF;
    }
    else if (type_id == INTVEC_CMD) fprintf(fd, ")");
  }
  return 1;
}
    
BOOLEAN slGetDumpAscii(si_link l)
{
  if (l->name[0] == '\0')
  {
    Werror("getdump: Can not get dump from stdin");
    return TRUE;
  }
  else
  {
    BOOLEAN status = iiPStart(NULL, l->name, NULL);

    if (status)
      return TRUE;
    else
    {
      // lets reset the file pointer to the end to reflect that
      // we are finished with reading
      FILE *f = (FILE *) l->data;
      fseek(f, 0L, SEEK_END);
      return FALSE;
    }
  }
}
      
  
/*------------Initialization at Start-up time------------------------*/

#ifdef HAVE_DBM
#include "sing_dbm.h"
#endif

#ifdef HAVE_MPSR
#include "sing_mp.h"
#endif  

void slStandardInit()
{
  si_link_extension s;
  si_link_root=(si_link_extension)Alloc0(sizeof(*si_link_root));
  si_link_root->Open=slOpenAscii;
  si_link_root->Close=slCloseAscii;
  si_link_root->Read=slReadAscii;
  si_link_root->Write=slWriteAscii;
  si_link_root->Dump=slDumpAscii;
  si_link_root->GetDump=slGetDumpAscii;
  si_link_root->Status=slStatusAscii;
  si_link_root->type="Ascii";
  s = si_link_root;
#ifdef HAVE_DBM
#ifndef HAVE_MODULE_DBM
  s->next = (si_link_extension)Alloc0(sizeof(*si_link_root));
  s = s->next;
  slInitDBMExtension(s);
#endif
#endif
#ifdef HAVE_MPSR
  s->next = (si_link_extension)Alloc0(sizeof(*si_link_root));
  s = s->next;
  slInitMPFileExtension(s);
  s->next = (si_link_extension)Alloc0(sizeof(*si_link_root));
  s = s->next;
  slInitMPTcpExtension(s);
#endif  
}
