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

#if ! defined(MSDOS) && ! defined(__MWERKS__) && \
    defined(HAVE_SYS_TYPES_H) && defined(HAVE_PWD_H)&&defined(HAVE_SYS_PARAM_H)

#include <sys/types.h>
#include <unistd.h> /* always defiend */
#include <pwd.h>

#include <stdlib.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h> /* can live without it */
#endif
#include <sys/param.h>
#include <strings.h>

#include "mmemory.h"

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
char *
find_executable (const char *name)
{
  char *search;
  char *p;
  char tbuf[MAXPATHLEN];

  if (ABSOLUTE_FILENAME_P(name)) {
      /* If we can execute the named file, and it is normal, then return it. */
      if (! access (name, X_OK)) {
        struct stat stat_temp;

        if (stat (name, &stat_temp))
          return 0;

#ifndef STAT_MACROS_BROKEN
        if (! S_ISREG(stat_temp.st_mode))
          return 0;
#endif
        return copy_of (name);
      }
    }
  else {
    if ((name[0] == '.') && (name[1] == '/')) {
      strcpy (tbuf, ".");

#ifdef HAVE_GETCWD
      getcwd (tbuf, MAXPATHLEN);
#else
# ifdef HAVE_GETWD
      getwd (tbuf);
# endif
#endif
      strcat (tbuf, name + 1);
      return copy_of (tbuf);
    }

    search = getenv ("PATH");
    p = search;

    while (*p) {
      char *next;
      next = tbuf;

      /* Perform tilde-expansion. Stolen from GNU readline/tilde.c. */
      if (p[0] == '~') {
        if (! p[1] || p[1] == '/') {
          /* Prepend $HOME to the rest of the string. */
          char *temp_home = (char *) getenv ("HOME");

          /* If there is no HOME variable, look up the directory in the
             password database. */
          if (! temp_home) {
            struct passwd *entry;

            entry = getpwuid (getuid ());
            if (entry)
              temp_home = entry->pw_dir;
          }

          strcpy (tbuf, temp_home);
          next = tbuf + strlen (tbuf);
          p ++;
        }
        else {
          char username[MAXPATHLEN];
          struct passwd *user_entry;
          int i;

          p ++;                        /* Skip the tilde. */
          for (i = 0; *p && *p != '/' && *p != ':'; i++)
            username[i] = *p ++;
          username[i] = '\0';

          user_entry = getpwnam (username);
          if (user_entry) {
            strcpy (tbuf, user_entry->pw_dir);
            next = tbuf + strlen (tbuf);
          }
        }

        endpwent ();
      }

      /* Copy directory name into [tbuf]. */
      while (*p && *p != ':')
        *next ++ = *p ++;
      *next = '\0';
      if (*p != '\0')
        p ++;

      if (tbuf[0] == '.' && tbuf[1] == '\0') {
#ifdef HAVE_GETCWD
        getcwd (tbuf, MAXPATHLEN);
#else
# ifdef HAVE_GETWD
        getwd (tbuf);
# endif
#endif
      }

      strcat (tbuf, "/");
      strcat (tbuf, name);

      /* If we can execute the named file, and it is normal, then return it. */
      if (! access (tbuf, X_OK)) {
        struct stat stat_temp;

        if (stat (tbuf, &stat_temp))
          continue;

#ifndef STAT_MACROS_BROKEN
        if (! S_ISREG(stat_temp.st_mode))
          continue;
#endif
        return copy_of (tbuf);
      }
    }
  }

  return NULL;
}

#else

char* find_executable (const char *name)
{
  return NULL;
}

#endif /* #if ! defined(MSDOS) && ! defined(__MWERKS__) && defined(HAVE_SYS_TYPES_H) && defined(HAVE_PWD_H)&&defined(HAVE_SYS_PARAM_H) */


