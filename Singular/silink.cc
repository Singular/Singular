/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: silink.cc,v 1.2 1997-03-24 14:25:45 Singular Exp $ */
/*
* ABSTRACT
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

/* =============== general utilities ====================================== */
void GetCmdArgs(int *argc, char ***argv, char *str)
{
  int i = 0, sl = strlen(str)+1, j;
  char *s2=mstrdup(str);

#ifdef HAVE_MPSR
  char *appl = strstr(s2, "-MPapplication");
  if (appl != NULL)
  {
    *(appl-1) = '\0';
    i = 2;
  }
#endif

  if (strtok(s2, " ") != NULL)
  {
    i++;
    while (strtok(NULL," ") != NULL) i++;
  }
  *argc = i;
  if (i>0)
  {
    *argv = (char **) Alloc0(i*sizeof(char *));
#ifdef HAVE_MPSR
    if (appl != NULL) i -= 2;
#endif
    if (i>0)
    {
      strcpy(s2,str);
      *argv[0] = mstrdup(strtok(s2, " "));
      for(j = 1; j <i; j++)
        (*argv)[j] = mstrdup(strtok(NULL, " "));
    }
  }
  else
    *argv = NULL;

#ifdef HAVE_MPSR
  if (appl != NULL)
  {
    (*argv)[*argc -2] = mstrdup("-MPapplication");
    (*argv)[*argc -1] = mstrdup(&(appl[14]));
  }
#endif

  FreeL(s2);
}
/* ====================================================================== */
si_link_extension si_link_root=NULL;
BOOLEAN slInit(si_link l, char *istr)
{
  char **argv;
  int argc;

  if (istr!=NULL)
  {
    GetCmdArgs(&argc, &argv, istr);
    // let's parse the results
    if ( argc <2 )
    {
      if (argc == 0)
      {
        argv = (char **) Alloc(2*sizeof(char *));
        argv[0]=mstrdup("");
      }
      else if (argc == 1)
      {
        char *n=argv[0];
        Free((ADDRESS)argv,sizeof(char *));
        argv = (char **) Alloc(2*sizeof(char *));
        argv[0]=n;
      }
      argv[1]=mstrdup("ascii");
      argc=2;
    }
    l->argc = argc;
    l->argv = argv;
  }
  if (l->name==NULL)
    l->name = mstrdup(argv[0]);

  BOOLEAN not_found=TRUE;
  si_link_extension s=si_link_root;
  while (s!=NULL)
  {
    if (!(not_found=s->Init(l,s)))
    {
      l->m=s;
      l->linkType=s->index;
      break;
    }
    s=s->next;
  }
  if (not_found) Werror("unknown link type `%s`",argv[1]);
  return not_found;
}

void slKill(si_link l)
{
  if (SI_LINK_OPEN_P(l)) slClose(l);
  FreeL((ADDRESS)l->name);
  l->name=NULL;
  if (l->argc!=0)
  {
    int i=l->argc-1;
    while(i>=0)
    {
      FreeL((ADDRESS)l->argv[i]);
      i--;
    }
    Free((ADDRESS)l->argv,l->argc*sizeof(char *));
    l->argv=NULL;
    l->argc=0;
  }
}

//--------------------------------------------------------------------------
BOOLEAN slOpenRead(si_link l)
{
  if(SI_LINK_R_OPEN_P(l)) return FALSE; // already open r
  si_link_extension s=si_link_root;
  while ((s!=NULL) && (s->index!=l->linkType)) s=s->next;
  if ((s==NULL)||(s->OpenRead==NULL))
     return TRUE;
  l->m=s;   
  return  s->OpenRead(l);   
}

BOOLEAN slOpenWrite(si_link l)
{
  if(SI_LINK_W_OPEN_P(l)) return FALSE; // already open w
  si_link_extension s=si_link_root;
  while ((s!=NULL) && (s->index!=l->linkType)) s=s->next;
  if ((s==NULL)||(s->OpenWrite==NULL))
     return TRUE;
  l->m=s;   
  return  s->OpenWrite(l);   
}

BOOLEAN slClose(si_link l)
{
  if(! SI_LINK_OPEN_P(l)) return FALSE; // already closed
  si_link_extension s=l->m;
  if (s==NULL)
  {
    s=si_link_root;
    while ((s!=NULL) && (s->index!=l->linkType)) s=s->next;
  }  
  if ((s==NULL)||(s->Close==NULL))
    return TRUE;
  return  s->Close(l);   
}

leftv slRead(si_link l,leftv a)
{
  leftv v = NULL;
  if( ! SI_LINK_R_OPEN_P(l)) // open r ?
  {
    if (slOpenRead(l)) return NULL;
  }
  if ((SI_LINK_R_OPEN_P(l))&&(l->m!=NULL))
  { // open r
    if (a==NULL)
    {
      if (l->m->Read!=NULL)
        v=l->m->Read(l);
    }
    else
    {
      if (l->m->Read2!=NULL)
        v=l->m->Read2(l,a);
    }
  }
  // here comes the eval:
  if (v != NULL) 
  {
    v->Eval();
  }
  return v;
}

BOOLEAN slWrite(si_link l, leftv v)
{
  if(! SI_LINK_W_OPEN_P(l)) // open w ?
  {
    if (slOpenWrite(l)) return TRUE;
  }
  if((SI_LINK_W_OPEN_P(l))&&(l->m!=NULL))
  { // now open w
    if (l->m->Write!=NULL)
      return l->m->Write(l,v);
  }
  return TRUE;
}

/* =============== ASCII ============================================= */
BOOLEAN slOpenWriteAscii(si_link l)
{
  if(SI_LINK_R_OPEN_P(l)&&(l->name[0]!='\0'))
  {
    Werror("cannot open input file %s as output file",l->name);
    return TRUE; // already open r
  }
  FILE *outfile;
  char *mode="a";
  char *filename=l->name;
  if(filename[0]=='\0')
  {
    SI_LINK_SET_W_OPEN_P(l); /*open, write */
    l->data=(void *)stdout;
    return FALSE;
  }
  else if(filename[0]=='>')
  {
    if (filename[1]=='>')
      filename+=2;
    else
    {
      filename++;
      mode="w";
    }
  }
  int i=2;
  while (i<l->argc)
  {
    if(strncmp(l->argv[i],"mode:",5)==0)
      mode=l->argv[i]+5;
    else if(strcmp(l->argv[i],"showPath")!=0)
      Warn("ignore link property %s",l->argv[i]);
    i++;
  }
  outfile=fopen(filename,mode);
  if (outfile!=NULL)
  {
    SI_LINK_SET_W_OPEN_P(l); /*open, write */
    l->data=(void *)outfile;
    return FALSE;
  }
  return TRUE;
}
BOOLEAN slOpenReadAscii(si_link l)
{
  if(SI_LINK_W_OPEN_P(l))
  {
    Werror("cannot open output file %s as input file",l->name);
    return TRUE; // already open w
  }
  if (l->name[0]!='\0')
  {
    char *where=NULL;
    int i=2;
    while (i<l->argc)
    {
      if(strcmp(l->argv[i],"showPath")==0)
        where=(char *)Alloc(120);
      else if(strncmp(l->argv[i],"mode:",5)!=0)
        Warn("ignore link property %s",l->argv[i]);
      i++;
    }
    FILE *rfile=feFopen(l->name,"r",where);
    if (where!=NULL)
    {
      Print("open %s, success:%d\n",where,rfile!=NULL);
      Free((ADDRESS)where,120);
    }
    if (rfile!=NULL)
    {
      l->data=(void *)rfile;
      SI_LINK_SET_R_OPEN_P(l);
      return FALSE;
    }
    Werror("cannot open %s",l->name);
    l->data=NULL;
    return TRUE;
  }
  SI_LINK_SET_R_OPEN_P(l);
  return FALSE;
}
BOOLEAN slCloseAscii(si_link l)
{
  SI_LINK_SET_CLOSE_P(l);
  if (l->name[0]!='\0')
  {
    return (fclose((FILE *)l->data)!=0);
  }
  return FALSE;
}
leftv slReadAscii(si_link l)
{
  FILE * fp=(FILE *)l->data;
  char * buf=NULL;
  if (fp!=NULL)
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

BOOLEAN slInitALink(si_link l,si_link_extension s)
{
  if (strcmp(l->argv[1], s->name) == 0)
  {
    return FALSE;
  }
  return TRUE;
}


/*-------------------------------------------------------------------*/
void slExtensionInit(si_link_extension s)
{
  s->index=si_link_root->index+10;
  s->next=si_link_root;
  si_link_root=s;
}

#ifdef HAVE_DBM
#include "sing_dbm.h"
#endif

#ifdef HAVE_MPSR
#include "sing_mp.h"
#endif  

void slStandardInit()
{
  si_link_root=(si_link_extension)Alloc0(sizeof(*si_link_root));
  si_link_root->Init=slInitALink;
  si_link_root->OpenRead=slOpenReadAscii;
  si_link_root->OpenWrite=slOpenWriteAscii;
  si_link_root->Close=slCloseAscii;
  si_link_root->Read=slReadAscii;
  //si_link_root->Read2=NULL;
  si_link_root->Write=slWriteAscii;
  si_link_root->name="ascii";
  si_link_root->index=1;
#ifdef HAVE_DBM
#ifndef HAVE_MODULE_DBM
  slExtensionInit(dbInit());
#endif
#endif
#ifdef HAVE_MPSR
  slExtensionInit(slInitMPFile());
  slExtensionInit(slInitMPTcp());
#endif  
}
