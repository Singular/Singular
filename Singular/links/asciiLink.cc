/****************************************
 * *  Computer Algebra System SINGULAR     *
 * ****************************************/

/*
 * ABSTRACT: ascii links (standard)
 */

#include "kernel/mod2.h"
#include "misc/options.h"

#include "Singular/tok.h"
#include "Singular/subexpr.h"
#include "Singular/ipshell.h"
#include "Singular/ipid.h"
#include "Singular/fevoices.h"
#include "kernel/oswrapper/feread.h"
#include "Singular/ipshell.h"
#include "Singular/links/silink.h"

/* declarations */
static BOOLEAN DumpAscii(FILE *fd, idhdl h,char ***list_of_libs);
static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h,char ***list_of_libs);
static const char* GetIdString(idhdl h);
static int DumpRhs(FILE *fd, idhdl h);
static BOOLEAN DumpQring(FILE *fd, idhdl h);
static BOOLEAN DumpNCring(FILE *fd, idhdl h);
static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl);
static BOOLEAN CollectLibs(char *name, char ***list_of_libs);
//static BOOLEAN DumpLibs(FILE *fd, char ***list_of_libs);

EXTERN_VAR si_link_extension si_link_root;

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
    if (len<0) len=0;
    fseek(fp,0L,SEEK_SET);
    buf=(char *)omAlloc((int)len+1);
    if (BVERBOSE(V_READING))
      Print("//Reading %ld chars\n",len);
    if (len>0) myfread( buf, len, 1, fp);
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
    switch(v->Typ())
    {
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
      {
        ideal I=(ideal)v->Data();
        for(int i=0;i<IDELEMS(I);i++)
        {
          char *s=pString(I->m[i]);
          fwrite(s,strlen(s),1,outfile);
          omFree(s);
          if (i<IDELEMS(I)-1) fwrite(",",1,1,outfile);
        }
        break;
      }
    #if 1
    case LIST_CMD:
      {
        lists l=(lists)v->Data();
        for(int i=0;i<l->nr;i++)
        {
          char *s=l->m[i].String();
          fwrite(s,strlen(s),1,outfile);
          omFree(s);
          if (i!=l->nr-1) fputc(',',outfile);
          fputc('\n',outfile);
        }
        break;
      }
    #endif
    default:
      s = v->String();
      // free v ??
      if (s!=NULL)
      {
        fputs(s,outfile);
        fputc('\n',outfile);
        omFree((ADDRESS)s);
      }
      else
      {
        WerrorS("cannot convert to string");
        err=TRUE;
      }
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
  char **list_of_libs=NULL;
  BOOLEAN status = DumpAscii(fd, h, &list_of_libs);

  if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh);
  fprintf(fd, "option(set, intvec(%d, %d));\n", si_opt_1, si_opt_2);
  char **p=list_of_libs;
  if (p!=NULL)
  {
    while((*p!=NULL) && (*p!=(char*)1))
    {
      fprintf(fd,"load(\"%s\",\"try\");\n",*p);
      p++;
    }
    omFree(list_of_libs);
  }
  fputs("RETURN();\n",fd);
  fflush(fd);

  return status;
}

// we do that recursively, to dump ids in the the order in which they
// were actually defined
static BOOLEAN DumpAscii(FILE *fd, idhdl h, char ***list_of_libs)
{
  if (h == NULL) return FALSE;

  if (DumpAscii(fd, IDNEXT(h),list_of_libs)) return TRUE;

  // need to set the ring before writing it, otherwise we get in
  // trouble with minpoly
  if (IDTYP(h) == RING_CMD)
    rSetHdl(h);

  if (DumpAsciiIdhdl(fd, h,list_of_libs)) return TRUE;

  if (IDTYP(h) == RING_CMD)
    return DumpAscii(fd, IDRING(h)->idroot,list_of_libs);
  else
    return FALSE;
}

static BOOLEAN DumpAsciiMaps(FILE *fd, idhdl h, idhdl rhdl)
{
  if (h == NULL) return FALSE;
  if (DumpAsciiMaps(fd, IDNEXT(h), rhdl)) return TRUE;

  if (IDTYP(h) == RING_CMD)
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

static BOOLEAN DumpAsciiIdhdl(FILE *fd, idhdl h, char ***list_of_libs)
{
  const char *type_str = GetIdString(h);
  int type_id = IDTYP(h);

  if (type_id == PACKAGE_CMD)
  {
    if (strcmp(IDID(h),"Top")==0) return FALSE; // do not dump "Top"
    if (IDPACKAGE(h)->language==LANG_SINGULAR) return FALSE;
    if (IDPACKAGE(h)->language==LANG_MIX) return FALSE;
  }
  if (type_id == CRING_CMD)
  {
    // do not dump the default CRINGs:
    if (strcmp(IDID(h),"QQ")==0) return FALSE;
    if (strcmp(IDID(h),"ZZ")==0) return FALSE;
    #ifdef SINGULAR_4_2
    if (strcmp(IDID(h),"AE")==0) return FALSE;
    if (strcmp(IDID(h),"QAE")==0) return FALSE;
    #endif
  }

  // we do not throw an error if a wrong type was attempted to be dumped
  if (type_str == NULL)
    return FALSE;

  // handle nc-rings separately
  if ((type_id == RING_CMD)&&(rIsNCRing(IDRING(h))))
    return DumpNCring(fd,h);

  // handle qrings separately
  if ((type_id == RING_CMD)&&(IDRING(h)->qideal!=NULL))
    return DumpQring(fd, h);

  // C-proc not to be dumped
  if ((type_id == PROC_CMD) && (IDPROC(h)->language == LANG_C))
    return FALSE;

  // handle libraries
  if ((type_id == PROC_CMD)
  && (IDPROC(h)->language == LANG_SINGULAR)
  && (IDPROC(h)->libname!=NULL))
    return CollectLibs(IDPROC(h)->libname,list_of_libs);

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
  else if (type_id == SMATRIX_CMD)
  {
    ideal id = IDIDEAL(h);
    if (fprintf(fd, "[%d][%d]", (int)id->rank, IDELEMS(id))== EOF) return TRUE;
  }

  if (type_id == PACKAGE_CMD)
  {
    return (fputs(";\n",fd) == EOF);
  }

  // write the equal sign
  if (fputs(" = ",fd) == EOF) return TRUE;

  // and the right hand side
  if (DumpRhs(fd, h) == EOF) return TRUE;

  // semicolon und tschuess
  if (fputs(";\n",fd) == EOF) return TRUE;

  return FALSE;
}

static const char* GetIdString(idhdl h)
{
  int type = IDTYP(h);

  switch(type)
  {
    case LIST_CMD:
    //{
    //
    //
    //  lists l = IDLIST(h);
    //  int i, nl = l->nr + 1;
//
    //  for (i=0; i<nl; i++)
    //    if (GetIdString((idhdl) &(l->m[i])) == NULL) return NULL;
    //  break;
    //}
    case CRING_CMD:
    #ifdef SINGULAR_4_2
    case CNUMBER_CMD:
    case CMATRIX_CMD:
    #endif
    case BIGINT_CMD:
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
    case SMATRIX_CMD:
      return Tok2Cmdname(type);

    case MAP_CMD:
    case LINK_CMD:
      return NULL;

    default:
      Warn("Error dump data of type %s", Tok2Cmdname(IDTYP(h)));
       return NULL;
  }
}

static BOOLEAN DumpNCring(FILE *fd, idhdl h)
{
  char *ring_str = h->String();
  ring r=IDRING(h);

  if (rIsPluralRing(r))
  {
    if (fprintf(fd, "ring temp_ring = %s;\n", ring_str)
      == EOF) return TRUE;
    if (fprintf(fd, "ideal temp_C = %s;\n",
              iiStringMatrix((matrix) r->GetNC()->C, 2, r, n_GetChar(r->cf)))
      == EOF) return TRUE;
    if (fprintf(fd, "ideal temp_D = %s;\n",
              iiStringMatrix((matrix) r->GetNC()->D, 2, r, n_GetChar(r->cf)))
      == EOF) return TRUE;
    if (fprintf(fd, "def %s = nc_algebra(temp_C,temp_D);\n",IDID(h)) == EOF)
      return TRUE;
    if (fputs("kill temp_ring;\n",fd) == EOF) return TRUE;
  }
  if (rIsLPRing(r))
  {
    //if (fprintf(fd, "ring %s = %s;\n", IDID(h), ring_str) == EOF) return TRUE;
    //if (fprintf(fd, "attrib(%s,\"isLetterplaceRing\",%d);\n",IDID(h),r->isLPring) ==EOF) return TRUE;
    Warn("cannot write LP ring %s",IDID(h));
    return TRUE;
  }
  omFree(ring_str);
  return FALSE;
}

static BOOLEAN DumpQring(FILE *fd, idhdl h)
{
  char *ring_str = h->String();
  ring r=IDRING(h);
  if (fprintf(fd, "ring temp_ring = %s;\n", ring_str) == EOF) return TRUE;
  if (fprintf(fd, "ideal temp_ideal = %s;\n",
              iiStringMatrix((matrix) r->qideal, 1, currRing, n_GetChar(r->cf)))
      == EOF) return TRUE;
  if (fputs("attrib(temp_ideal, \"isSB\", 1);\n",fd) == EOF) return TRUE;
  if (fprintf(fd, "qring %s = temp_ideal;\n",IDID(h)) == EOF)
    return TRUE;
  if (fputs("kill temp_ring;\n",fd) == EOF) return TRUE;
  else
  {
    omFree(ring_str);
    return FALSE;
  }
}

static BOOLEAN CollectLibs(char *name, char *** list_of_libs)
{
  if (*list_of_libs==NULL)
  {
    #define MAX_LIBS 256
    (*list_of_libs)=(char**)omAlloc0(MAX_LIBS*sizeof(char**));
    (*list_of_libs)[0]=name;
    (*list_of_libs)[MAX_LIBS-1]=(char*)1;
    return FALSE;
  }
  else
  {
    char **p=*list_of_libs;
    while (((*p)!=NULL)&&((*p!=(char*)1)))
    {
      if (strcmp((*p),name)==0) return FALSE;
      p++;
    }
    if (*p==(char*)1)
    {
      WerrorS("too many libs");
      return TRUE;
    }
    else
    {
      *p=name;
    }
  }
  return FALSE;
}


static int DumpRhs(FILE *fd, idhdl h)
{
  int type_id = IDTYP(h);

  if (type_id == LIST_CMD)
  {
    lists l = IDLIST(h);
    int i, nl = l->nr;

    fputs("list(",fd);

    for (i=0; i<nl; i++)
    {
      if (DumpRhs(fd, (idhdl) &(l->m[i])) == EOF) return EOF;
      fputs(",",fd);
    }
    if (nl > 0)
    {
      if (DumpRhs(fd, (idhdl) &(l->m[nl])) == EOF) return EOF;
    }
    fputs(")",fd);
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
      /* pi-Libname==NULL */
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
    if (type_id == INTVEC_CMD) { fputs("intvec(",fd);need_klammer=TRUE; }
    else if (type_id == IDEAL_CMD) { fputs("ideal(",fd);need_klammer=TRUE; }
    else if ((type_id == MODUL_CMD)||(type_id == SMATRIX_CMD))
                                   { fputs("module(",fd);need_klammer=TRUE; }
    else if (type_id == BIGINT_CMD) { fputs("bigint(",fd);need_klammer=TRUE; }

    if (fputs(rhs,fd) == EOF) return EOF;
    omFree(rhs);

    if ((type_id == RING_CMD) &&
        IDRING(h)->cf->type==n_algExt)
    {
      StringSetS("");
      p_Write(IDRING(h)->cf->extRing->qideal->m[0],IDRING(h)->cf->extRing);
      rhs = StringEndS();
      if (fprintf(fd, "; minpoly = %s", rhs) == EOF) { omFree(rhs); return EOF;}
      omFree(rhs);
    }
    else if (need_klammer) fputc(')',fd);
  }
  return 1;
}

BOOLEAN slGetDumpAscii(si_link l)
{
  if (l->name[0] == '\0')
  {
    WerrorS("getdump: Can not get dump from stdin");
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
