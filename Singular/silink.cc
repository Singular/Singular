/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT: general interface to links
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kernel/mod2.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <Singular/subexpr.h>
#include <Singular/ipid.h>
#include <Singular/silink.h>
#include <Singular/ipshell.h>
#include <kernel/matpol.h>
#include <kernel/ring.h>
#include <Singular/lists.h>
#include <kernel/ideals.h>
#include <kernel/numbers.h>
#include <kernel/intvec.h>
#include <Singular/ssiLink.h>
#include <Singular/pipeLink.h>

// #ifdef HAVE_DBM
// #ifdef ix86_Win
// #define USE_GDBM
// #endif
// #endif

omBin s_si_link_extension_bin = omGetSpecBin(sizeof(s_si_link_extension));
omBin sip_link_bin = omGetSpecBin(sizeof(sip_link));
omBin ip_link_bin = omGetSpecBin(sizeof(ip_link));

/* declarations */
static BOOLEAN DumpAscii(FILE *fd, idhdl h);
static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h);
static const char* GetIdString(idhdl h);
static int DumpRhs(FILE *fd, idhdl h);
static BOOLEAN DumpQring(FILE *fd, idhdl h, const char *type_str);
static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl);
static si_link_extension slTypeInit(si_link_extension s, const char* type);

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
        type = omStrDup(istr);
        istr[i] = ':';
      }
      // and check for mode
      j = ++i;
      while (istr[j] != ' ' && istr[j] != '\0') j++;
      if (j > i)
      {
        mode = omStrDup(&(istr[i]));
        mode[j - i] = '\0';
      }
      // and for the name
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = omStrDup(&(istr[j]));
    }
    else // no colon find -- string is entire name
    {
      j=0;
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = omStrDup(&(istr[j]));
    }
  }

  // set the link extension
  if (type != NULL)
  {
    si_link_extension s = si_link_root;
    si_link_extension prev = s;

    while (strcmp(s->type, type) != 0)
    {
      if (s->next == NULL)
      {
        prev = s;
        s = NULL;
        break;
      }
      else
      {
        s = s->next;
      }
    }

    if (s != NULL)
      l->m = s;
    else
    {
      l->m = slTypeInit(prev, type);
    }
    omFree(type);
  }
  else
    l->m = si_link_root;

  if (l->m == NULL) return TRUE;

  l->name = (name != NULL ? name : omStrDup(""));
  l->mode = (mode != NULL ? mode : omStrDup(""));
  l->ref = 1;
  return FALSE;
}

void slCleanUp(si_link l)
{
  (l->ref)--;
  if (l->ref == 0)
  {
    if (SI_LINK_OPEN_P(l))
    {
      if (l->m->Kill != NULL) l->m->Kill(l);
      else if (l->m->Close != NULL) l->m->Close(l);
    }
    omFree((ADDRESS)l->name);
    omFree((ADDRESS)l->mode);
    memset((void *) l, 0, sizeof(ip_link));
  }
}

void slKill(si_link l)
{
  slCleanUp(l);
  if (l->ref == 0)
    omFreeBin((ADDRESS)l,  ip_link_bin);
}

const char* slStatus(si_link l, const char *request)
{
  if (l == NULL) return "empty link";
  else if (l->m == NULL) return "unknown link type";
  else if (strcmp(request, "type") == 0) return l->m->type;
  else if (strcmp(request, "mode") == 0) return l->mode;
  else if (strcmp(request, "name") == 0) return l->name;
  else if (strcmp(request, "exists") ==0)
  {
    struct stat buf;
    if (lstat(l->name,&buf)==0) return "yes";
    else return "no";
  }
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
BOOLEAN slOpen(si_link l, short flag, leftv h)
{
  BOOLEAN res;

  if (l->m == NULL) slInit(l, ((char*)""));

  if (SI_LINK_OPEN_P(l))
  {
    Warn("open: link of type: %s, mode: %s, name: %s is already open",
         l->m->type, l->mode, l->name);
    return FALSE;
  }
  else if (l->m->Open != NULL)
    res = l->m->Open(l, flag, h);
  else
    res = TRUE;

  const char *c="_";;
  if (h!=NULL) c=h->Name();

  if (res)
    Werror("open: Error for link %s of type: %s, mode: %s, name: %s",
           c, l->m->type, l->mode, l->name);
  return res;
}

BOOLEAN slClose(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_OPEN_P(l))
    return FALSE;
  else if (l->m->Close != NULL)
    res = l->m->Close(l);
  else
    res = TRUE;

  if (res)
    Werror("close: Error for link of type: %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  return res;
}

leftv slRead(si_link l, leftv a)
{
  char *mode;
  leftv v = NULL;
  if( ! SI_LINK_R_OPEN_P(l)) // open r ?
  {
#ifdef HAVE_DBM
#ifdef USE_GDBM
    if (! SI_LINK_CLOSE_P(l))
      {
        if (slClose(l)) return NULL;
      }
#endif
#endif
    if (slOpen(l, SI_LINK_READ,NULL)) return NULL;
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
  if (v != NULL)
  {
    if (v->Eval() && !errorreported)
      WerrorS("eval: failed");
  }
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
#ifdef HAVE_DBM
#ifdef USE_GDBM
    if (! SI_LINK_CLOSE_P(l))
      {
        if (slClose(l)) return TRUE;
      }
#endif
#endif
    if (slOpen(l, SI_LINK_WRITE,NULL)) return TRUE;
  }

  if (SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Write != NULL)
      res = l->m->Write(l,v);
    else
      res = TRUE;

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
    if (slOpen(l, SI_LINK_WRITE,NULL)) return TRUE;
  }

  if(SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Dump != NULL)
      res = l->m->Dump(l);
    else
      res = TRUE;

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
    if (slOpen(l, SI_LINK_READ,NULL)) return TRUE;
  }

  if(SI_LINK_R_OPEN_P(l))
  { // now open r
    if (l->m->GetDump != NULL)
      res = l->m->GetDump(l);
    else
      res = TRUE;

    if (res)
      Werror("getdump: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    //res|=slClose(l);
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
BOOLEAN slOpenAscii(si_link l, short flag, leftv h)
{
  const char *mode;
  if (flag & SI_LINK_OPEN)
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
    outfile=myfopen(filename,mode);
    if (outfile!=NULL)
      l->data = (void *) outfile;
    else
      return TRUE;
  }

  omFree(l->mode);
  l->mode = omStrDup(mode);
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

leftv slReadAscii2(si_link l, leftv pr)
{
  FILE * fp=(FILE *)l->data;
  char * buf=NULL;
  if (fp!=NULL && l->name[0] != '\0')
  {
    fseek(fp,0L,SEEK_END);
    long len=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    buf=(char *)omAlloc((int)len+1);
    if (BVERBOSE(V_READING))
      Print("//Reading %ld chars\n",len);
    myfread( buf, len, 1, fp);
    buf[len]='\0';
  }
  else
  {
    if (pr->Typ()==STRING_CMD)
    {
      buf=(char *)omAlloc(80);
      fe_fgets_stdin((char *)pr->Data(),buf,80);
    }
    else
    {
      WerrorS("read(<link>,<string>) expected");
      buf=omStrDup("");
    }
  }
  leftv v=(leftv)omAlloc0Bin(sleftv_bin);
  v->rtyp=STRING_CMD;
  v->data=buf;
  return v;
}

leftv slReadAscii(si_link l)
{
  sleftv tmp;
  memset(&tmp,0,sizeof(sleftv));
  tmp.rtyp=STRING_CMD;
  tmp.data=(void*) "? ";
  return slReadAscii2(l,&tmp);
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
      omFree((ADDRESS)s);
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

const char* slStatusAscii(si_link l, const char* request)
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
  idhdl h = IDROOT, rh = currRingHdl;
  BOOLEAN status = DumpAscii(fd, h);

  if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh);
  fprintf(fd, "option(set, intvec(%d, %d));\n", test, verbose);
  fprintf(fd, "RETURN();\n");
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
    rSetHdl(h);

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
    rSetHdl(rhdl);
    rhs = h->String();

    if (fprintf(fd, "setring %s;\n", IDID(rhdl)) == EOF) return TRUE;
    if (fprintf(fd, "%s %s = %s, %s;\n", Tok2Cmdname(MAP_CMD), IDID(h),
                IDMAP(h)->preimage, rhs) == EOF)
    {
      omFree(rhs);
      return TRUE;
    }
    else
    {
      omFree(rhs);
      return FALSE;
    }
  }
  else return FALSE;
}

static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h)
{
  const char *type_str = GetIdString(h);
  int type_id = IDTYP(h);

  if ((type_id == PACKAGE_CMD) &&(strcmp(IDID(h), "Top") == 0))
    return FALSE;

  // we do not throw an error if a wrong type was attempted to be dumped
  if (type_str == NULL)
    return FALSE;

  // handle qrings separately
  if (type_id == QRING_CMD)
    return DumpQring(fd, h, type_str);

  // C-proc not to be dumped
  if ((type_id == PROC_CMD) && (IDPROC(h)->language == LANG_C))
    return FALSE;

  // put type and name
  if (fprintf(fd, "%s %s", type_str, IDID(h)) == EOF)
    return TRUE;
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

  if (type_id == PACKAGE_CMD)
  {
    return (fprintf(fd, ";\n") == EOF);
  }

  // write the equal sign
  if (fprintf(fd, " = ") == EOF) return TRUE;

  // and the right hand side
  if (DumpRhs(fd, h) == EOF) return TRUE;

  // semicolon und tschuess
  if (fprintf(fd, ";\n") == EOF) return TRUE;

  return FALSE;
}

static const char* GetIdString(idhdl h)
{
  int type = IDTYP(h);

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
      case PACKAGE_CMD:
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

static BOOLEAN DumpQring(FILE *fd, idhdl h, const char *type_str)
{
  char *ring_str = h->String();
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
    omFree(ring_str);
    return FALSE;
  }
}


static int DumpRhs(FILE *fd, idhdl h)
{
  int type_id = IDTYP(h);

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
  else  if (type_id == STRING_CMD)
  {
    char *pstr = IDSTRING(h), c;
    fputc('"', fd);
    while (*pstr != '\0')
    {
      if (*pstr == '"' || *pstr == '\\')  fputc('\\', fd);
      fputc(*pstr, fd);
      pstr++;
    }
    fputc('"', fd);
  }
  else  if (type_id == PROC_CMD)
  {
    procinfov pi = IDPROC(h);
    if (pi->language == LANG_SINGULAR) {
      if( pi->data.s.body==NULL) iiGetLibProcBuffer(pi);
      char *pstr = pi->data.s.body, c;
      fputc('"', fd);
      while (*pstr != '\0') {
        if (*pstr == '"' || *pstr == '\\') fputc('\\', fd);
        fputc(*pstr, fd);
        pstr++;
      }
      fputc('"', fd);
    } else fputs("(null)", fd);
  }
  else
  {
    char *rhs = h->String();

    if (rhs == NULL) return EOF;

    if (type_id == INTVEC_CMD) fprintf(fd, "intvec(");

    if (fprintf(fd, "%s", rhs) == EOF) return EOF;
    omFree(rhs);

    if ((type_id == RING_CMD || type_id == QRING_CMD) &&
        IDRING(h)->minpoly != NULL)
    {
      StringSetS("");
      nWrite(IDRING(h)->minpoly);
      rhs = StringAppendS("");
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
    BOOLEAN status = newFile(l->name);
    if (status)
      return TRUE;

    int old_echo=si_echo;
    si_echo=0;

    status=yyparse();

    si_echo=old_echo;

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

#include <Singular/slInit.h>

static si_link_extension slTypeInit(si_link_extension s, const char* type)
{
  assume(s != NULL);
  s->next = NULL;
  si_link_extension ns = (si_link_extension)omAlloc0Bin(s_si_link_extension_bin);

  if (0) 0;
#ifdef HAVE_MPSR
  else if (strcmp(type, "MPfile") == 0)
    s->next = slInitMPFileExtension(ns);
  else if (strcmp(type, "MPtcp") == 0)
    s->next = slInitMPTcpExtension(ns);
#endif
#ifdef HAVE_DBM
  else if (strcmp(type, "DBM") == 0)
    s->next = slInitDBMExtension(ns);
#endif
#if 1
  else if (strcmp(type, "ssi") == 0)
    s->next = slInitSsiExtension(ns);
#endif
#if 1
  else if (strcmp(type, "|") == 0)
    s->next = slInitPipeExtension(ns);
#endif
  else
  {
    Warn("Found unknown link type: %s", type);
    Warn("Use default link type: %s", si_link_root->type);
    omFreeBin(ns, s_si_link_extension_bin);
    return si_link_root;
  }

  if (s->next == NULL)
  {
    Werror("Can not initialize link type %s", type);
    omFreeBin(ns, s_si_link_extension_bin);
    return NULL;
  }
  return s->next;
}

void slStandardInit()
{
  si_link_extension s;
  si_link_root=(si_link_extension)omAlloc0Bin(s_si_link_extension_bin);
  si_link_root->Open=slOpenAscii;
  si_link_root->Close=slCloseAscii;
  si_link_root->Kill=slCloseAscii;
  si_link_root->Read=slReadAscii;
  si_link_root->Read2=slReadAscii2;
  si_link_root->Write=slWriteAscii;
  si_link_root->Dump=slDumpAscii;
  si_link_root->GetDump=slGetDumpAscii;
  si_link_root->Status=slStatusAscii;
  si_link_root->type="ASCII";
  s = si_link_root;
  s->next = NULL;
}
