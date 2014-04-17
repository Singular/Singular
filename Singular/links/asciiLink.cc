/****************************************
 * *  Computer Algebra System SINGULAR     *
 * ****************************************/

/*
 * ABSTRACT: ascii links (standard)
 */

#include <kernel/mod2.h>
#include <misc/options.h>
#include <omalloc/omalloc.h>
#include <Singular/febase.h>

#include <Singular/tok.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipid.h>
#include <Singular/links/silink.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* declarations */
static BOOLEAN DumpAscii(FILE *fd, idhdl h);
static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h);
static const char* GetIdString(idhdl h);
static int DumpRhs(FILE *fd, idhdl h);
static BOOLEAN DumpQring(FILE *fd, idhdl h, const char *type_str);
static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl);

extern si_link_extension si_link_root;

/* =============== ASCII ============================================= */
BOOLEAN slOpenAscii(si_link l, short flag, leftv /*h*/)
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
  fprintf(fd, "option(set, intvec(%d, %d));\n", si_opt_1, si_opt_2);
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
              iiStringMatrix((matrix) IDRING(h)->qideal, 1, currRing, n_GetChar(currRing->cf)))
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
    char *pstr = IDSTRING(h);
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
    if (pi->language == LANG_SINGULAR)
    {
      if( pi->data.s.body==NULL) iiGetLibProcBuffer(pi);
      char *pstr = pi->data.s.body;
      fputc('"', fd);
      while (*pstr != '\0')
      {
        if (*pstr == '"' || *pstr == '\\') fputc('\\', fd);
        fputc(*pstr, fd);
        pstr++;
      }
      fputc('"', fd);
    }
    else fputs("(null)", fd);
  }
  else
  {
    char *rhs = h->String();

    if (rhs == NULL) return EOF;

    BOOLEAN need_klammer=FALSE;
    if (type_id == INTVEC_CMD) { fprintf(fd, "intvec(");need_klammer=TRUE; }
    else if (type_id == IDEAL_CMD) { fprintf(fd, "ideal(");need_klammer=TRUE; }
    else if (type_id == MODUL_CMD) { fprintf(fd, "module(");need_klammer=TRUE; }

    if (fprintf(fd, "%s", rhs) == EOF) return EOF;
    omFree(rhs);

    if ((type_id == RING_CMD || type_id == QRING_CMD) &&
        IDRING(h)->cf->type==n_algExt)
    {
      StringSetS("");
      p_Write(IDRING(h)->cf->extRing->qideal->m[0],IDRING(h)->cf->extRing);
      rhs = StringEndS();
      if (fprintf(fd, "; minpoly = %s", rhs) == EOF) { omFree(rhs); return EOF;}
      omFree(rhs);
    }
    else if (need_klammer) fprintf(fd, ")");
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


void slStandardInit()
{
  si_link_extension s;
  si_link_root=(si_link_extension)omAlloc0Bin(s_si_link_extension_bin);
  si_link_root->Open=slOpenAscii;
  si_link_root->Close=slCloseAscii;
  si_link_root->Kill=NULL;
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
