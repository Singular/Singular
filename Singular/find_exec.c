/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $id:$ */

/* find_exec.c -- Find a program, given an argv[0]
   Copyright (C) 1996 Free Software Foundation, Inc.
   Copyright (C) 1990 by W. Wilson Ho.
   This file is part of the GNU Dld Library. */

/* Adapted for use with Singular by obachman@mathematik.uni-kl.de  4/98*/

#include "mod2.h"

#if ! defined(MSDOS) && ! defined(__MWERKS__)

#include <unistd.h> /* always defiend */
#include <stdlib.h>
#include <strings.h>

#ifndef ESINGULAR
#include "mmemory.h"
#endif


#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif


/* Do not return copies of sth, but simply the strings
   -- we make copies later */
#define copy_of(string) mstrdup(string)

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
char * find_executable (const char *name)
#else
char * find_executable_link (const char *name)
#endif
{
  char *search;
  char *p;
  char *extra = NULL;
  char tbuf[MAXPATHLEN];

  if (ABSOLUTE_FILENAME_P(name))
  {
      /* If we can execute the named file then return it. */
      if (! access (name, X_OK))
        return copy_of (name);
  }
  else
  {
    if (((name[0] == '.') && (name[1] == '/')) ||
        ((name[0] == '.') && (name[1] == '.') && (name[2] == '/')))
    {
      strcpy (tbuf, (name[1] == '.' ? ".." : "."));

#ifdef HAVE_GETCWD
      getcwd (tbuf, MAXPATHLEN);
#else
# ifdef HAVE_GETWD
      getwd (tbuf);
# endif
#endif
      strcat (tbuf, "/");
      strcat (tbuf, name);
      return copy_of (tbuf);
    }


    search = getenv("PATH");
/* for winnt under msdos, cwd is implictly in the path */
#ifdef WINNT
    p = getenv("SHELL");
    if (p == NULL || strlen(p) < 2)
    {
      /* we are under msdos display */
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
          return copy_of (tbuf);
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
char * find_executable (const char *name)
{
  char * link = find_executable_link(name);
  char buf[MAXPATHLEN];
  int ret;

  if (link == NULL && (ret=readlink(name, buf, MAXPATHLEN)) > 0)
  {
    buf[ret] ='\0';
    link = find_executable_link(buf);
  }
  if (link != NULL && (ret=readlink(link, buf, MAXPATHLEN)) > 0)
  {
    char *p = strrchr(link, '/');
    char *executable;


    if(p!=NULL) *(p+1)='\0';
    buf[ret]='\0';

    if (buf[0] != '/')
    {
      executable = (char*) AllocL(strlen(link) + ret + 1);
      strcpy(executable, link);
      strcat(executable, buf);
    }
    else
    {
      executable = copy_of(buf);
    }

    FreeL(link);
    return executable;
  }

  return link;
}
#endif /* HAVE_READLINK */

#else

char* find_executable (const char *name)
{
  return NULL;
}

#endif /* #if ! defined(MSDOS) && ! defined(__MWERKS__) && defined(HAVE_SYS_TYPES_H) && defined(HAVE_PWD_H)&&defined(HAVE_SYS_PARAM_H) */


