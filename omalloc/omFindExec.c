/*******************************************************************
 *  File:    omFindExec.c
 *  Purpose: routine which determines absolute pathname of executable
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omFindExec.c,v 1.1.1.1 1999-11-18 17:45:54 obachman Exp $
 *******************************************************************/

#include "omConfig.h"

#if ! defined(__MWERKS__) && defined(HAVE_UNISTD_H) && defined(STDC_HEADERS)

#include <unistd.h> /* always defiend */
#include <stdlib.h>
#include <strings.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif


/* ABSOLUTE_FILENAME_P (fname): True if fname is an absolute filename */
#ifdef atarist
#define ABSOLUTE_FILENAME_P(fname)        ((fname[0] == '/') || \
        (fname[0] && (fname[1] == ':')))
#else
#define ABSOLUTE_FILENAME_P(fname)        (fname[0] == '/')
#endif /* atarist */

/* Return the absolute name of the program named NAME.  This function
   searches the directories in the PATH environment variable if PROG
   has no directory components. */
#ifndef HAVE_READLINK
char * omFindExec (const char *name, char* executable)
#else
char * omFindExec_link (const char *name, char* executable)
#endif
{
  char *search;
  char *p;
  char tbuf[MAXPATHLEN];

  if (ABSOLUTE_FILENAME_P(name))
  {
      /* If we can execute the named file then return it. */
      if (! access (name, X_OK))
      {
        strcpy(executable, name);
        return executable;
      }
  }
  else
  {
    if (((name[0] == '.') && (name[1] == '/')) ||
        ((name[0] == '.') && (name[1] == '.') && (name[2] == '/')) ||
        strchr(name, '/') != NULL)
    {

#ifdef HAVE_GETCWD
      getcwd (tbuf, MAXPATHLEN);
#else
# ifdef HAVE_GETWD
      getwd (tbuf);
# endif
#endif
      strcat (tbuf, "/");
      strcat (tbuf, name);
      if (! access(tbuf, X_OK))
      {
        strcpy(executable, tbuf);
        return executable;
      }
    }


    search = getenv("PATH");
/* for winnt under msdos, cwd is implictly in the path */
#ifdef WINNT
    p = getenv("SHELL");
    if (p == NULL || strlen(p) < 2)
    {
      char *extra = NULL;
      /* we are under msdos display */
      FIX ME
        extra = (char*) AllocL((search != NULL ? strlen(search) : 0) + 3);
      strcpy(extra, ".:");
      if (search != NULL) strcat(extra, search);
      search = extra;
    }
#endif
    p = search;

    if (p != NULL)
    {
      while (1)
      {
        char *next;
        next = tbuf;

        /* Copy directory name into [tbuf]. */
        /* This is somewhat tricky: empty names mean cwd, w.r.t. some
           shell spec */
        while (*p && *p != ':')
          *next ++ = *p ++;
        *next = '\0';

        if ((tbuf[0] == '.' && tbuf[1] == '\0') || tbuf[0] == '\0') {
#ifdef HAVE_GETCWD
          getcwd (tbuf, MAXPATHLEN);
#else
# ifdef HAVE_GETWD
          getwd (tbuf);
# endif
#endif
        }

        if (tbuf[strlen(tbuf)-1] != '/') strcat(tbuf, "/");
        strcat (tbuf, name);
        
        /* If we can execute the named file, then return it. */
        if (! access (tbuf, X_OK))
        {
#ifdef WINNT
          if (extra != NULL)
            FreeL(extra);
#endif
          strcpy(executable, tbuf);
          return executable;
        }

        if (*p != '\0')
        {
          p ++;
        }
        else
        {
          break;
        }
      }
    }
  }
    
    return NULL;
  }

#ifdef HAVE_READLINK

#define MAX_LINK_LEVEL 10
/* similar to readlink (cf. man readlink), except that symbolic links are 
   followed up to MAX_LINK_LEVEL
*/
int full_readlink(const char* name, char* buf, size_t bufsize)
{
  int ret;
  
  if ((ret=readlink(name, buf, bufsize)) > 0)
  {
    char buf2[MAXPATHLEN];
    int ret2, i = 0;
    
    do
    {
      buf[ret] = '\0';
      if ((ret2 = readlink(buf, buf2, MAXPATHLEN)) > 0)
      {
        i++;
        buf2[ret2] = '\0';
        strcpy(buf, buf2);
        ret = ret2;
      }
      else
      {
        return ret;
      }
    }
    while (i<MAX_LINK_LEVEL);
  }
  return -1;
}
  
  
char * omFindExec (const char *name, char* exec)
{
  char * link = omFindExec_link(name, exec);
  char buf[MAXPATHLEN];
  int ret;

  if (link == NULL && (ret=full_readlink(name, buf, MAXPATHLEN)) > 0)
  {
    buf[ret] ='\0';
    link = omFindExec_link(buf, exec);
  }
  if (link != NULL && (ret=full_readlink(link, buf, MAXPATHLEN)) > 0)
  {
    buf[ret]='\0';
    return omFindExec_link(buf, exec);
  }
  return link;
}
#endif /* HAVE_READLINK */

#else

char* omFindExec (const char *name)
{
  return NULL;
}

#endif /* ! defined(__MWERKS__) && defined(HAVE_UNISTD_H) && defined(STDC_HEADERS) */


