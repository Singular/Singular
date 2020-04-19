#include "singular_resourcesconfig.h"
#include "feResource.h"
#include "feFopen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#if defined(HAVE_PWD_H) && defined(HAVE_GETPWNAM)
#include <pwd.h>
#endif




extern "C" {
VAR void (*WerrorS_callback)(const char *s) = NULL;
VAR void (*PrintS_callback)(const char *s) = NULL;
VAR short errorreported=0;
void WerrorS(const char *s)
{
  errorreported = 1;
  if (WerrorS_callback == NULL)
  {
     fwrite("   ? ",1,5,stderr);
     fwrite((char *)s,1,strlen((char *)s),stderr);
     fwrite("\n",1,1,stderr);
     fflush(stderr);
  }
  else
  {
    WerrorS_callback(s);
  }
}
}

/*****************************************************************
 *
 * File handling
 *
 *****************************************************************/

FILE * feFopen(const char *path, const char *mode, char *where,
               short useWerror, short path_only)
{
  char longpath[MAXPATHLEN];
  if (path[0]=='~')
  {
    if (path[1] == DIR_SEP)
    {
      const char* home = getenv("HOME");
#ifdef __CUGWIN__
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
      if (dir_sep==NULL)
      {
        char buf[256];
        strcpy(buf,"illegal ~ in filename >>");
        strncat(buf,longpath,235);
        strcat(buf,"<<");
        WerrorS(buf);
        return NULL;
      }
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
    int res = -1;
    do
    {
      res = stat(path,&statbuf);
    } while((res < 0) and (errno == EINTR));
    if ((res == 0)
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

    if (where==NULL) s=(char *)malloc(1024);
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
        if (where==NULL) free(s);
        return f;
      }
    }
    else
    {
      if (where!=NULL) strcpy(s/*where*/,path);
      f=myfopen(path,mode);
    }
    if (where==NULL) free(s);
  }
  if ((f==NULL)&&(useWerror))
  {
    char buf[256];
    strcpy(buf,"cannot open `");
    strncat(buf,path,240);
    strcat(buf,"`");
    WerrorS(buf);
  }
  return f;
}

// Make sure that mode contains binary option
FILE* myfopen(const char *path, const char *mode)
{
#if (defined(__CUGWIN__))
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
#else
  return fopen(path, mode);
#endif
}
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
