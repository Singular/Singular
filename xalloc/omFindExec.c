/*******************************************************************
 *  File:    omFindExec.c
 *  Purpose: routine which determines absolute pathname of executable
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omFindExec.c,v 1.11 2008/02/03 20:03:14 wienand Exp $
 *******************************************************************/

#include <unistd.h> /* always defiend */
#include <stdlib.h>
#include <string.h>
#include "omalloc.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#define HAVE_GETCWD

/* ABSOLUTE_FILENAME_P (fname): True if fname is an absolute filename */
#define ABSOLUTE_FILENAME_P(fname)        (fname[0] == '/')

/* Return the absolute name of the program named NAME.  This function
   searches the directories in the PATH environment variable if PROG
   has no directory components. */
static char * omFindExec_link (const char *name, char* executable)
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
#ifdef __CYGWIN__
        strcat(executable, ".exe");
#endif
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
#ifdef __CYGWIN__
        strcat(executable, ".exe");
#endif
        return executable;
      }
    }


    search = getenv("PATH");
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
          strcpy(executable, tbuf);
#ifdef __CYGWIN__
          strcat(executable, ".exe");
#endif
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

/* similar to readlink, but dont' mess up absolute pathnames */
static int my_readlink(const char* name, char* buf, size_t bufsize)
{
  char buf2[MAXPATHLEN];
  int ret;

  if ((ret = readlink(name, buf2, bufsize)) > 0)
  {
    buf2[ret] = 0;
    if (*name == '/' && *buf2 != '/')
    {
      char* last = strrchr(name, '/');
      int i = 0;
      while (&(name[i]) != last)
      {
        buf[i] = name[i];
        i++;
      }
      buf[i] = '/';
      i++;
      strcpy(&(buf[i]), buf2);
      return i + ret;
    }
    else
    {
      strcpy(buf, buf2);
    }
  }
  return ret;
}

#define MAX_LINK_LEVEL 10
/* similar to readlink (cf. man readlink), except that symbolic links are
   followed up to MAX_LINK_LEVEL
*/
static int full_readlink(const char* name, char* buf, size_t bufsize)
{
  int ret;

  if ((ret=my_readlink(name, buf, bufsize)) > 0)
  {
    char buf2[MAXPATHLEN];
    int ret2, i = 0;

    do
    {
      buf[ret] = '\0';
      if ((ret2 = my_readlink(buf, buf2, MAXPATHLEN)) > 0)
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
    char *p = strrchr(link, '/');


    if(p!=NULL) *(p+1)='\0';
    buf[ret]='\0';

    if (buf[0] != '/')
    {
      strcpy(exec, link);
      strcat(exec, buf);
    }
    else
    {
      strcpy(exec, buf);
    }

    return exec;
  }
  return link;
}

int om_sing_opt_show_mem; /* dummy */
struct omInfo_s om_Info; /* dummy */
struct omOpts_s om_Opts; /* dummy */

