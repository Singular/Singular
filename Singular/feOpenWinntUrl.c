/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: encapsulation of call to Win32 ShellExecute call for opening
            a URL


*/

/*!
!
 you can try this out by compiling with -DTEST and runing:
   a.exe file|url [not-local]

*/
#ifdef WINNT
#include <windows.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

void heOpenWinntUrl(const char* url, int local)
{
#ifdef TEST
  printf("url:%s:local:%d\n", url, local);
#endif
  if (local)
  {
    char path[MAXPATHLEN];
    char *p;
    cygwin_conv_to_full_win32_path(url, path);
    /*!
!
 seems like I can not open url's wit # at the end 

*/
    if ((p=strchr(path, '#')) != NULL) *p = '\0';
#ifdef TEST
    printf("path:%s:local:%d\n", path, local);
#endif
    ShellExecute(NULL, "open", path, 0, 0, SW_SHOWNORMAL);
  }
  else
  {
    // need to check whether this works
    ShellExecute(NULL, "open", url, 0, 0, SW_SHOWNORMAL);
  }
}

#ifdef TEST
int main(int argc, char* argv[])
{
  heOpenWinntUrl(argv[1], argc > 2 ? 0 : 1);
}
#endif
#endif
