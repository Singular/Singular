#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"

#include <misc/auxiliary.h>

#include <omalloc/omalloc.h>
#include <reporter/reporter.h>

#include "feFopen.h"
#include "feResource.h"



/*****************************************************************
 *
 * File handling
 *
 *****************************************************************/

FILE * feFopen(const char *path, const char *mode, char *where,
               int useWerror, int path_only)
{
  char longpath[MAXPATHLEN];
  if (path[0]=='~')
  {
    if (path[1] == DIR_SEP)
    {
      const char* home = getenv("HOME");
#ifdef ix86_Win
      if ((home==NULL)||(!access(home,X_OK)))
        home = getenv("SINGHOME");
#endif
      if (home != NULL)
      {
        strcpy(longpath, home);
        strcat(longpath, &(path[1]));
        path = longpath;
      }
    }
#if defined(HAVE_PWD_H) && defined(HAVE_GETPWNAM)
    else
    {
      char* dir_sep;
      struct passwd *pw_entry;
      strcpy (longpath, path);
      dir_sep = strchr(longpath, DIR_SEP);
      *dir_sep = '\0';
      pw_entry = getpwnam(&longpath[1]);
      if (pw_entry != NULL)
      {
        strcpy(longpath, pw_entry->pw_dir);
        dir_sep = strchr((char *)path, DIR_SEP);
        strcat(longpath, dir_sep);
        path = longpath;
      }
    }
#endif
  }
  FILE * f=NULL;
  if (! path_only)
  {
    struct stat statbuf;
    if ((stat(path,&statbuf)==0)
    && (S_ISREG(statbuf.st_mode)))
      f = myfopen(path,mode);
  }
  if (where!=NULL) strcpy(where,path);
  if ((*mode=='r') &&
      (path[0]!=DIR_SEP) &&
      ! (path[0] == '.' && path[1] == DIR_SEP) &&
      (f==NULL))
  {
    char found = 0;
    char* spath = feResource('s');
    char *s;

    if (where==NULL) s=(char *)omAlloc(250);
    else             s=where;

    if (spath!=NULL)
    {
      char *p,*q;
      p = spath;
      while( (q=strchr(p, fePathSep)) != NULL)
      {
        *q = '\0';
        strcpy(s,p);
        *q = fePathSep;
        strcat(s, DIR_SEPP);
        strcat(s, path);
        if(!access(s, R_OK)) { found++; break; }
        p = q+1;
      }
      if(!found)
      {
        strcpy(s,p);
        strcat(s, DIR_SEPP);
        strcat(s, path);
      }
      f=myfopen(s,mode);
      if (f!=NULL)
      {
        if (where==NULL) omFree((ADDRESS)s);
        return f;
      }
    }
    else
    {
      if (where!=NULL) strcpy(s/*where*/,path);
      f=myfopen(path,mode);
    }
    if (where==NULL) omFree((ADDRESS)s);
  }
  if ((f==NULL)&&(useWerror))
    Werror("cannot open `%s`",path);
  return f;
}

#ifdef ix86_Win
// Make sure that mode contains binary option
FILE* myfopen(const char *path, const char *mode)
{
  char mmode[4];
  int i;
  int done = 0;

  for (i=0;;i++)
  {
    mmode[i] = mode[i];
    if (mode[i] == '\0') break;
    if (mode[i] == 'w') done = 1;
    if (mode[i] == 'a') done = 1;
    if (mode[i] == 'b') done = 1;
  }

  if (! done)
  {
    mmode[i] = 'b';
    mmode[i+1] = '\0';
  }
  return fopen(path, mmode);
}
#endif
// replace "\r\n" by " \n" and "\r" by "\n"

size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t got = fread(ptr, size, nmemb, stream) * size;
  size_t i;

  for (i=0; i<got; i++)
  {
    if ( ((char*) ptr)[i] == '\r')
    {
      if (i+1 < got && ((char*) ptr)[i+1] == '\n')
        ((char*) ptr)[i] = ' ';
      else
        ((char*) ptr)[i] = '\n';
    }
  }
  return got;
}
