/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: encapsulation of call to Win32 WinHelp call for opening 
            a help file 
*/

/* you can try this out by compiling with -DTEST and runing:
   a.exe "poly"*/
#ifdef WINNT
#include <windows.h>
#include <winuser.h>

#ifndef MAXPATHLEN 
#define MAXPATHLEN 1024
#endif

void heOpenWinntHlp(const char* keyw)
{
  const char helppath[]="/E/home/html/SINGULAR.HLP";
  char path[MAXPATHLEN];
#ifdef TEST
  printf("keyw:%s\n", keyw);
#endif    
  cygwin_conv_to_full_win32_path(helppath, path);
#ifdef TEST
  printf("path:%s\n", path);
#endif
      WinHelp(NULL, path, HELP_PARTIALKEY,(DWORD)keyw);
}

#ifdef TEST
int main(int argc, char* argv[])
{
  heOpenWinntHlp(argv[1]);
}
#endif
#endif
