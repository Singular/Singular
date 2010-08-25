/* run -- Wrapper program for console mode programs under Windows(TM)
 * Copyright (C) 1998  Charles S. Wilson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * This program is based on the runemacs.c distributed with XEmacs 21.0
 *
 * Simple program to start gnu-win32 X11 programs (and native XEmacs) 
 * with its console window hidden.
 *
 * This program is provided purely for convenience, since most users will
 * use XEmacs in windowing (GUI) mode, and will not want to have an extra
 * console window lying around. Ditto for desktop shortcuts to gnu-win32 
 * X11 executables.
 */


#define WIN32

#include <windows.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <run.h>

#if defined(__CYGWIN__)
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/cygwin.h>
 #include <sys/unistd.h>
//WinMainCRTStartup() { mainCRTStartup(); }
#else
 #include <direct.h>
#endif


char buffer[1024];

int WINAPI
WinMain (HINSTANCE hSelf, HINSTANCE hPrev, LPSTR cmdline, int nShow)
{
   int wait_for_child = FALSE;
   int compact_invocation = FALSE;
   DWORD ret_code = 0;


   char execname[FILENAME_MAX];
   char execpath[MAX_PATH];
   char* argv[MAX_ARGS+1]; /* leave extra slot for compact_invocation argv[0] */
   int argc;
   int i,j;
   char exec[MAX_PATH + FILENAME_MAX + 100];
   char cmdline2[MAX_ARGS * MAX_PATH];

   compact_invocation = get_exec_name_and_path(execname,execpath);

   if (compact_invocation)
   {
      argv[0] = execname;
      argc = parse_cmdline_to_arg_array(&(argv[1]),cmdline);
      argc++;
   }
   else
   {
      argc = parse_cmdline_to_arg_array(argv,cmdline);
      if (argc >= 1)
         strcpy(execname,argv[0]);
   }
   /* at this point, execpath is defined, as are argv[] and execname */
#ifdef DEBUG
   j = sprintf(buffer,"\nexecname : %s\nexecpath : %s\n",execname,execpath);
   for (i = 0; i < argc; i++)
      j += sprintf(buffer+j,"argv[%d]\t: %s\n",i,argv[i]);
   Trace((buffer));
#endif

   if (execname == NULL)
      error("you must supply a program name to run");

#if defined(__CYGWIN__)
   /* this insures that we search for symlinks before .exe's */
//   if (compact_invocation)
//      strip_exe(execname);
#endif

   process_execname(exec,execname,execpath);
   Trace(("exec\t%s\nexecname\t%s\nexecpath\t%s\n",
         exec,execname,execpath));

   wait_for_child = build_cmdline(cmdline2,exec,argc,argv);
   Trace((cmdline2));

   xemacs_special(exec);
   ret_code = start_child(cmdline2,wait_for_child);
   if (compact_invocation)
      for (i = 1; i < argc; i++) // argv[0] was not malloc'ed
         free(argv[i]);
   else
      for (i = 0; i < argc; i++)
         free(argv[i]);
   return (int) ret_code;
}
int start_child(char* cmdline, int wait_for_child)
{
   STARTUPINFO start;
   SECURITY_ATTRIBUTES sec_attrs;
   SECURITY_DESCRIPTOR sec_desc;
   PROCESS_INFORMATION child;
   int retval;

   memset (&start, 0, sizeof (start));
   start.cb = sizeof (start);
   start.dwFlags = STARTF_USESHOWWINDOW;
   start.wShowWindow = SW_HIDE;

   sec_attrs.nLength = sizeof (sec_attrs);
   sec_attrs.lpSecurityDescriptor = NULL;
   sec_attrs.bInheritHandle = FALSE;

   if (CreateProcess (NULL, cmdline, &sec_attrs, NULL, TRUE, 0,
                      NULL, NULL, &start, &child))
   {
      if (wait_for_child)
      {
         WaitForSingleObject (child.hProcess, INFINITE);
         GetExitCodeProcess (child.hProcess, &retval);
      }
      CloseHandle (child.hThread);
      CloseHandle (child.hProcess);
   }
   else
      error("could not start %s",cmdline);
   return retval;
}
void xemacs_special(char* exec)
{
  /*
   * if we're trying to run xemacs, AND this file was in %emacs_dir%\bin,
   * then set emacs_dir environment variable
   */
   char* p;
   char* p2;
   char exec2[MAX_PATH + FILENAME_MAX + 100];
   char tmp[MAX_PATH + FILENAME_MAX + 100];
   strcpy(exec2,exec);
   /* this depends on short-circuit evaluation */
   if ( ((p = strrchr(exec2,'\\')) && stricmp(p,"\\xemacs") == 0) ||
        ((p = strrchr(exec2,'/')) && stricmp(p,"/xemacs") == 0) ||
        ((p = strrchr(exec2,'\\')) && stricmp(p,"\\xemacs.exe") == 0) ||
        ((p = strrchr(exec2,'/')) && stricmp(p,"/xemacs.exe") == 0) )
   {
      if ( ((p2 = strrchr(p, '\\')) && stricmp(p2, "\\bin") == 0) ||
           ((p2 = strrchr(p, '/')) && stricmp(p2, "/bin") == 0) )
      {
         *p2 = '\0';   
#if defined(__CYGWIN__)
         CYGWIN_CONV_TO_POSIX_PATH((exec2,tmp));
         strcpy(exec2,tmp);
#else /* NATIVE xemacs DOS-style paths with forward slashes */
         for (p = exec2; *p; p++)
            if (*p == '\\') *p = '/';
#endif
         SetEnvironmentVariable ("emacs_dir", exec2);
      }
   }
}
int build_cmdline(char* new_cmdline, char* exec, int argc, char* argv[])
{
   int retval = FALSE;
   int first_arg = 1;
   int i;
   int char_cnt = 0;
   /*
    * look for "-wait" as first true argument; we'll apply that ourselves
    */
   if ((argc >= 2) && (stricmp(argv[1],"-wait") == 0))
   {
      retval = TRUE;
      first_arg++;
   }

   char_cnt = strlen(exec);
   for (i = first_arg; i < argc; i++)
      char_cnt += strlen(argv[i]);
   if (char_cnt > MAX_ARGS*MAX_PATH) /* then we ran out of room */
      error("command line too long -\n%s",new_cmdline);
   
   strcpy(new_cmdline,exec);
   for (i = first_arg; i < argc; i++)
   {
      strcat(new_cmdline," ");
      strcat(new_cmdline,argv[i]);
   }
   return retval;
}
/* process exec_arg : if it
 * NATIVE:
 *  1) starts with '\\' or '/', it's a root-path and leave it alone
 *  2) starts with 'x:\\' or 'x:/', it's a root-path and leave it alone
 *  3) starts with '.\\' or './', two possible meanings:
 *       1) exec is in the current directory
 *       2) exec in same directory as this program
 *  4) otherwise, search path (and _prepend_ "." to the path!!!)
 *  5) convert all '/' to '\\'
 * CYGWIN
 *  1) starts with '\\' or '/', it's a root-path and leave it alone
 *  2) starts with 'x:\\' or 'x:/', it's a root-path and leave it alone
 *  3) starts with '.\\' or './', two possible meanings:
 *       1) exec is in the current directory
 *       2) exec in same directory as this program
 *  4) otherwise, search path (and _prepend_ "." to the path!!!)
 *  5) convert to cygwin-style path to resolve symlinks within the pathspec
 *  6) check filename: if it's a symlink, resolve it by peeking inside
 *  7) convert to win32-style path+filename since we're using Windows 
 *       createProcess() to launch
 */
void process_execname(char *exec, const char* execname,const char* execpath )
{
   char* orig_pathlist;
   char* pathlist;
   char exec_tmp[MAX_PATH + FILENAME_MAX + 100];
   char exec_tmp2[MAX_PATH + FILENAME_MAX + 100];
   char buf[MAX_PATH + FILENAME_MAX + 100];
   int i,j;

   int len = 0;
   /* 
    * STARTS WITH / or \ 
    * execpath NOT used
    */
   if ((execname[0] == '\\') || (execname[0] == '/'))
   {
#if defined(__CYGWIN__)
      strcpy(exec_tmp,execname);
#else    
      exec_tmp[0] = ((char) (_getdrive() + ((int) 'A') - 1));
      exec_tmp[1] = ':';
      exec_tmp[2] = '\0';
      strcat(exec_tmp,execname);
#endif
      Trace(("/ -\nexec_tmp\t%s\nexecname\t%s\nexecpath\t%s\n",
             exec_tmp,execname,execpath));
      if (! fileExistsMulti(exec_tmp2,NULL,exec_tmp,exts,NUM_EXTENSIONS) )
      {
          j = 0;
          for (i = 0; i < NUM_EXTENSIONS; i++)
              j += sprintf(buf + j," [%d]: %s\n",i+1,exts[i]);
          error("Couldn't locate %s\nI tried appending the following "
                "extensions: \n%s",exec_tmp,buf);
      }
      Trace((exec_tmp2));
   }
   /* 
    * STARTS WITH x:\ or x:/
    * execpath NOT used
    */
    else if ((strlen(execname) > 3) && // avoid boundary errors
       (execname[1] == ':') &&
       ((execname[2] == '\\') || (execname[2] == '/')))
   {
      strcpy(exec_tmp,execname);       
      Trace(("x: -\nexec_tmp\t%s\nexecname\t%s\nexecpath\t%s\n",
             exec_tmp,execname,execpath));
      if (! fileExistsMulti(exec_tmp2,NULL,exec_tmp,exts,NUM_EXTENSIONS) )
      {
          j = 0;
          for (i = 0; i < NUM_EXTENSIONS; i++)
              j += sprintf(buf + j," [%d]: %s\n",i+1,exts[i]);
          error("Couldn't locate %s\nI tried appending the following "
                "extensions: \n%s",exec_tmp,buf);
      }
      Trace((exec_tmp2));
   }
   /* 
    * STARTS WITH ./ or .\
    */
   else if ((execname[0] == '.') &&
            ((execname[1] == '\\') || (execname[1] == '/')))
   {
      if (((char*) getcwd(exec_tmp,MAX_PATH))==NULL)
         error("can't find current working directory");
      if (! fileExistsMulti(exec_tmp2,exec_tmp,&(execname[2]),
                            exts,NUM_EXTENSIONS) )
          if (! fileExistsMulti(exec_tmp2,execpath,&(execname[2]),
                                exts,NUM_EXTENSIONS) )
          {
              j = 0;
              for (i = 0; i < NUM_EXTENSIONS; i++)
                  j += sprintf(buf + j," [%d]: %s\n",i+1,exts[i]);
              error("Couldn't locate %s\n"
                    "I looked in the following directories:\n [1]: %s\n [2]: %s\n"
                    "I also tried appending the following "
                    "extensions: \n%s",execname,exec_tmp,execpath,buf);
          }
      Trace((exec_tmp2));
   }
   /*
    * OTHERWISE, SEARCH PATH (prepend '.' and run.exe's directory)
    * can't use fileExistsMulti because we want to search entire path
    * for exts[0], then for exts[1], etc.
    */
   else
   {
      orig_pathlist = getenv("PATH");
      if ((pathlist = malloc (strlen(orig_pathlist)
                              + strlen(".") 
                              + strlen(execpath)+ 3)) == NULL)
         error("internal error - out of memory");
      strcpy(pathlist,".");
      strcat(pathlist,SEP_CHARS);
      strcat(pathlist,execpath);
      strcat(pathlist,SEP_CHARS);
      strcat(pathlist,orig_pathlist);

      Trace((pathlist));
      for (i = 0; i < NUM_EXTENSIONS; i++)
      {
          strcpy(exec_tmp,execname);
          strcat(exec_tmp,exts[i]);
          pfopen(exec_tmp2,exec_tmp,pathlist);
          if (fileExists(NULL,NULL,exec_tmp2))
              break;
          exec_tmp2[0] = '\0';
      }
      Trace(("exec_tmp\t%s\npathlist\t%s\n",exec_tmp2,pathlist));

      free(pathlist);
      if (exec_tmp2[0] == '\0')
      {
          j = 0;
          for (i = 0; i < NUM_EXTENSIONS; i++)
              j += sprintf(buf + j," [%d]: %s\n",i+1,exts[i]);
          error("Couldn't find %s anywhere.\n"
                "I even looked in the PATH \n"
                "I also tried appending the following "
                "extensions: \n%s",execname,buf);
      }
   }
/*
 * At this point, we know that exec_tmp2 contains a filename
 * and we know that exec_tmp2 exists.
 */
#if defined(__CYGWIN__)
   {
      struct stat stbuf;
      char sym_link_name[MAX_PATH+1];
      char real_name[MAX_PATH+1];
      char dummy[MAX_PATH+1];

      strcpy(exec_tmp,exec_tmp2);

      CYGWIN_CONV_TO_POSIX_PATH((exec_tmp,sym_link_name));
      Trace((sym_link_name));
      
      if (lstat(sym_link_name, &stbuf) == 0)
      {
         if ((stbuf.st_mode & S_IFLNK) == S_IFLNK)
         {
	    if (readlink(sym_link_name, real_name, sizeof(real_name)) == -1)
               error("problem reading symbolic link for %s",exec_tmp);
            else
            {
                // if realname starts with '/' it's a rootpath 
                if (real_name[0] == '/')
                    strcpy(exec_tmp2,real_name);
                else // otherwise, it's relative to the symlink's location
                {
                   CYGWIN_SPLIT_PATH((sym_link_name,exec_tmp2,dummy));
                   if (!endsWith(exec_tmp2,PATH_SEP_CHAR_STR))
                      strcat(exec_tmp2,PATH_SEP_CHAR_STR);
                   strcat(exec_tmp2,real_name);
                }
            }
         }
         else /* NOT a symlink */
            strcpy(exec_tmp2, sym_link_name);
      }
      else
         error("can't locate executable - %s",sym_link_name);
   }
   CYGWIN_CONV_TO_FULL_WIN32_PATH((exec_tmp2,exec));
#else					
   strcpy (exec, exec_tmp2);
#endif  
}
int endsWith(const char* s1, const char* s2)
{
    int len1;
    int len2;
    int retval = FALSE;
    len1 = strlen(s1);
    len2 = strlen(s2);
    if (len1 - len2 >= 0)
        if (stricmp(&(s1[len1-len2]),s2) == 0)
            retval = TRUE;
    return retval;
}void strip_exe(char* s)
{
   if ((strlen(s) > 4) && // long enough to have .exe extension
       // second part not evaluated (short circuit) if exec_arg too short
       (stricmp(&(s[strlen(s)-4]),".exe") == 0))
      s[strlen(s)-4] = '\0';
}
void error(char* fmt, ...)
{
   char buf[4096];
   int j;
   va_list args;
   va_start(args, fmt);
   j =   sprintf(buf,    "Error: ");
   j += vsprintf(buf + j,fmt,args);
   j +=  sprintf(buf + j,"\n");
   va_end(args);
   MessageBox(NULL, buf, "Run.exe", MB_ICONSTOP);
   exit(1);
}
void message(char* fmt, ...)
{
   char buf[10000];
   int j;
   va_list args;
   va_start(args, fmt);
   j = vsprintf(buf,fmt,args);
   j +=  sprintf(buf + j,"\n");
   va_end(args);
   MessageBox(NULL, buf, "Run.exe Message", MB_ICONSTOP);
}
void Trace_(char* fmt, ...)
{
   char buf[10000];
   int j;
   va_list args;
   va_start(args, fmt);
   j = vsprintf(buf,fmt,args);
   j +=  sprintf(buf + j,"\n");
   va_end(args);
   MessageBox(NULL, buf, "Run.exe DEBUG", MB_ICONSTOP);
}
/*
 * Uses system info to determine the path used to invoke run
 * Also attempts to deduce the target execname if "compact_invocation"
 * method was used.
 *
 * returns TRUE if compact_invocation method was used
 *   (and target execname was deduced successfully)
 * otherwise returns FALSE, and execname == run or run.exe
 */
int get_exec_name_and_path(char* execname, char* execpath)
{
   char modname[MAX_PATH];
   char* tmp_execname;
   char* p;
   int retval = FALSE;

   if (!GetModuleFileName (NULL, modname, MAX_PATH))
      error("internal error - can't find my own name");
   if ((p = strrchr (modname, '\\')) == NULL)
      error("internal error - my own name has no path\n%s",modname);
   tmp_execname = p + 1;
   p[0] = '\0';
   // if invoked by a name like "runxemacs" then strip off
   // the "run" and let "xemacs" be execname.
   // To check for this, make that:
   //   1) first three chars are "run"
   //   2) but the string doesn't end there, or start ".exe"
   // Also, set "compact_invocation" TRUE
   if ( ((tmp_execname[0] == 'r') || (tmp_execname[0] == 'R')) &&
        ((tmp_execname[1] == 'u') || (tmp_execname[1] == 'U')) &&
        ((tmp_execname[2] == 'n') || (tmp_execname[2] == 'N')) &&
        ((tmp_execname[3] != '.') && (tmp_execname[3] != '\0')) )
   {
      tmp_execname += 3;
      retval = TRUE;
   }
   else
      tmp_execname = NULL;

   if (tmp_execname == NULL)
      strcpy(execname,"");
   else
      strcpy(execname,tmp_execname);
#if defined(__CYGWIN__)
   CYGWIN_CONV_TO_POSIX_PATH((modname,execpath));
#else
   strcpy(execpath,modname);
#endif
   return retval;
}
/*
 * works like strtok, but:
 * double quotes (") suspends tokenizing until closing " reached
 * CYGWIN ONLY:
 *   additionally, backslash escapes next character, even if that
 *   next character is a delimiter. Or a double quote.
 *   WARNING: this means that backslash may NOT be a delimiter 
 */
char* my_strtok(char* s, const char* delim, char** lasts)
{
   char *spanp;
   int c, sc;
   char *tok;
   
   if ((s == NULL) && ((s = *lasts) == NULL))
      return NULL;
   /* Skip leading delimiters */
cont:
   c = *s++;
   for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
      if (c == sc)
         goto cont;
   }
   if (c == 0) {		/* no non-delimiter characters */
      *lasts = NULL;
      return (NULL);
   }
   tok = s - 1;
   /*
    * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
    * Note that delim must have one NUL; we stop if we see that, too.
    * If we see a double quote, continue until next double quote, then
    *   start scanning for delimiters again.
    * CYGWIN ONLY: if we see a backslash, just copy next character -
    *   don't consider it as a delimiter even if it is in delim string.
    */
   for (;;) {
      /* if this c is ", then scan until we find next " */
      if (c == '\"')
         while ((c = *s++) != '\"')
            if (c == 0) /* oops, forgot to close the ", clean up & return */
            {
               s = NULL;
               *lasts = s;
               return (tok);
            }
#if defined(__CYGWIN__)
      if (c == '\\')
      {
         c = *s++; /* skip the backslash */
         if (c == 0) /* if escaped character is end-of-string, clean up & return */
         {
            s = NULL;
            *lasts = s;
            return (tok);
         }
         c = *s++; /* otherwise, skip the escaped character */
      }
#endif   
      spanp = (char *)delim;
      do {
         if ((sc = *spanp++) == c) {
            if (c == 0)
               s = NULL;
            else
               s[-1] = 0;
            *lasts = s;
            return (tok);
         }
      } while (sc != 0);
      c = *s++;
   }
   /* NOTREACHED */
}
int parse_cmdline_to_arg_array(char* argv[MAX_ARGS], char* cmdline)
{
   char seps[] = " \t\n";
   char* token;
   int argc = 0;
   char* lasts;

   token = my_strtok(cmdline, seps, &lasts);
   while ((token != NULL) && (argc < MAX_ARGS))
   {
      if ((argv[argc] = malloc(strlen(token)+1)) == NULL)
      {
         error("internal error - out of memory");
      }
      strcpy(argv[argc++],token);
      token = my_strtok(NULL,seps,&lasts);
   }
   if (argc >= MAX_ARGS)
      error("too many arguments on commandline\n%s",cmdline);
   return argc;
}
/* Taken from pfopen.c by David Engel (5-Jul-97).
 * Original comments appear below. Superseded by next comment block.
 *
 *  Written and released to the public domain by David Engel.
 *
 *  This function attempts to open a file which may be in any of
 *  several directories.  It is particularly useful for opening
 *  configuration files.  For example, PROG.EXE can easily open
 *  PROG.CFG (which is kept in the same directory) by executing:
 *
 *      cfg_file = pfopen("PROG.CFG", "r", getenv("PATH"));
 *
 *  NULL is returned if the file can't be opened.
 */

/*
 * This function attempts to locate a file which may be in any of
 * several directories. Unlike the original pfopen, it does not
 * return a FILE pointer to the opened file, but rather returns
 * the fully-qualified filename of the first match found. Returns
 * empty string if not found.
 */
char *pfopen(char *retval, const char *name, const char *dirs)
{
    char *ptr;
    char *tdirs;
    char returnval[MAX_PATH + FILENAME_MAX + 100];
    char *recursive_name;
    int foundit = FALSE;
    
    returnval[0] = '\0';

    if (dirs == NULL || dirs[0] == '\0')
        return NULL;

    if ((tdirs = malloc(strlen(dirs)+1)) == NULL)
        return NULL;

    strcpy(tdirs, dirs);

    for (ptr = strtok(tdirs, SEP_CHARS); (foundit == FALSE) && ptr != NULL;
         ptr = strtok(NULL, SEP_CHARS))
    {
       foundit = fileExists(returnval,ptr,name);
    }

    free(tdirs);
    if (!foundit)
        retval[0] = '\0';
    else
        strcpy(retval,returnval);
    return retval;
}
int fileExistsMulti(char* fullname, const char* path, 
                    const char* name_noext, const char* exts[],
                    const int extcnt)
{
    char tryName[MAX_PATH + FILENAME_MAX];
    int i = 0;
    int retval = FALSE;
    fullname[0] = '\0';
    for (i = 0; i < extcnt; i++)
    {
        strcpy(tryName,name_noext);
        strcat(tryName,exts[i]);
        if (fileExists(fullname, path, tryName) == TRUE)
        {
            retval = TRUE;
            break;
        }
        fullname[0] = '\0';
    }
    return retval;
}
int fileExists(char* fullname, const char* path, const char* name)
{
   int retval = FALSE;
   FILE* file;
   size_t len;
   char work[FILENAME_MAX];
   char work2[MAX_PATH + FILENAME_MAX + 100];
   if (path != NULL)
   {
      strcpy(work, path);
      len = strlen(work);
      if (len && work[len-1] != '/' && work[len-1] != '\\')
         strcat(work, PATH_SEP_CHAR_STR);
   }
   else
      work[0]='\0';
   
   strcat(work, name);
#if defined(__CYGWIN__)
   CYGWIN_CONV_TO_POSIX_PATH((work, work2)); 
#else
   strcpy(work2,work);
#endif

#ifdef DEBUGALL
   Trace(("looking for...\t%s\n",work2));
#endif

   file = fopen(work2, "rb");
   if (file != NULL)
   {
      if (fullname != NULL)
         strcpy(fullname,work2);
      retval = TRUE;
      fclose(file);
   }
   return retval;
}
