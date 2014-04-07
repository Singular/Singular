/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    pipeLink.h
 *  Purpose: declaration of sl_link routines for pipe
 ***************************************************************/

#include <kernel/mod2.h>

#include <omalloc/omalloc.h>
#include <reporter/si_signals.h>

#include "tok.h"
#include "ipid.h"
#include "subexpr.h"
#include "links/silink.h"
#include "lists.h"
#include "pipeLink.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>          /* for portability */
#include <sys/select.h>
#include <sys/socket.h>

typedef struct
{
  FILE *f_read;
  FILE *f_write;
  pid_t pid; /* only valid for fork/tcp mode*/
  int fd_read,fd_write; /* only valid for fork/tcp mode*/
  char level;
} pipeInfo;

//**************************************************************************/
BOOLEAN pipeOpen(si_link l, short flag, leftv /*u*/)
{
  pipeInfo *d=(pipeInfo*)omAlloc0(sizeof(pipeInfo));
  if (flag & SI_LINK_OPEN)
  {
      flag = SI_LINK_READ| SI_LINK_WRITE;
  }
  int pc[2];
  int cp[2];
  pipe(pc);
  pipe(cp);
  pid_t pid=fork();
  if (pid==0) /*child*/
  {
    /* close unnecessary pipe descriptors for a clean environment */
    si_close(pc[1]); si_close(cp[0]);
    /* dup pipe read/write to stdin/stdout */
    si_dup2( pc[0], STDIN_FILENO );
    si_dup2( cp[1], STDOUT_FILENO  );
    int r=system(l->name);
    si_close(pc[0]);
    si_close(cp[1]);
    exit(r);
        /* never reached*/
  }
  else if (pid>0)
  {
    d->pid=pid;
    si_close(pc[0]); si_close(cp[1]);
    d->f_read=fdopen(cp[0],"r");
    d->fd_read=cp[0];
    d->f_write=fdopen(pc[1],"w");
    d->fd_write=pc[1];
    SI_LINK_SET_RW_OPEN_P(l);
  }
  else
  {
    Werror("fork failed (%d)",errno);
    omFreeSize(d,sizeof(*d));
    return TRUE;
  }
  l->data=d;
  return FALSE;
}

//**************************************************************************/
LINKAGE BOOLEAN pipeClose(si_link l)
{
  pipeInfo *d = (pipeInfo *)l->data;
  if (d!=NULL)
  {
    if (d->f_read!=NULL) fclose(d->f_read);
    if (d->f_write!=NULL) fclose(d->f_write);
    if (d->pid!=0) { kill(d->pid,15); kill(d->pid,9); }
  }
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

//**************************************************************************/
LINKAGE BOOLEAN pipeKill(si_link l)
{
  if(SI_LINK_OPEN_P(l)) pipeClose(l);
  pipeInfo *d = (pipeInfo *)l->data;
  if (d!=NULL)
  {
    omFreeSize((ADDRESS)d,(sizeof *d));
  }
  l->data=NULL;
  return FALSE;
}

//**************************************************************************/
LINKAGE leftv pipeRead1(si_link l)
{
  pipeInfo *d = (pipeInfo *)l->data;
  leftv res=(leftv)omAlloc0(sizeof(sleftv));
  char *s=(char *)omAlloc0(1024);
  char *ss=fgets(s,1024,d->f_read);
  if (ss==NULL) { omFreeSize(s,1024); pipeClose(l);return NULL; }
  int i=strlen(s)-1;
  if ((i>=0) && (s[i]=='\n')) s[i]='\0';
  res->rtyp=STRING_CMD;
  res->data=s;
  return res;
}
//**************************************************************************/
extern si_link pipeLastLink;
LINKAGE BOOLEAN pipeWrite(si_link l, leftv data)
{
  if(!SI_LINK_W_OPEN_P(l)) slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL);
  pipeInfo *d = (pipeInfo *)l->data;
  FILE *outfile=d->f_write;;
  BOOLEAN err=FALSE;
  char *s;
  pipeLastLink=l;
  while (data!=NULL)
  {
    s = data->String();
    // free data ??
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
    if (pipeLastLink==NULL) return TRUE;
    data = data->next;
  }
  fflush(outfile);
  pipeLastLink=NULL;
  return err;
}

const char* slStatusPipe(si_link l, const char* request)
{
  pipeInfo *d=(pipeInfo*)l->data;
  if (d==NULL) return "not open";
  if(strcmp(request, "read") == 0)
  {
    int s;
    if ((!SI_LINK_R_OPEN_P(l)) || (feof(d->f_read))) s=0;
    else
    {
      fd_set  mask/*, fdmask*/;
      struct timeval wt;
      /* Don't block. Return socket status immediately. */
      wt.tv_sec  = 0;
      wt.tv_usec = 0;

      FD_ZERO(&mask);
      FD_SET(d->fd_read, &mask);
      //Print("test fd %d\n",d->fd_read);
      /* check with select: chars waiting: no -> not ready */
      s=si_select(d->fd_read+1, &mask, NULL, NULL, &wt);
    }
    switch (s)
    {
      case 0: /* not ready */ return "not ready";
      case -1: /*error*/      return "error";
      default: /*1: ready ? */return "ready";
    }
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    return "not ready";
  }
  return "unknown status request";
}

si_link_extension slInitPipeExtension(si_link_extension s)
{
  s->Open=pipeOpen;
  s->Close=pipeClose;
  s->Kill=pipeKill;
  s->Read=pipeRead1;
  s->Read2=(slRead2Proc)NULL;
  s->Write=pipeWrite;

  s->Status=slStatusPipe;
  s->type="pipe";
  return s;
}
