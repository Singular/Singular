/*
 * $Id: locals.h,v 1.2 1999-03-17 13:06:21 krueger Exp $
 */

#include <mod2.h>
#include <tok.h>
#include <ipid.h>
#include <mmemory.h>
#include <febase.h>
#include <subexpr.h>
//#include <utils.h>
#include <structs.h>
#include <ipconv.h>

BOOLEAN jjANY2LIST(leftv res, leftv v, int cnt);

#if 0
extern "C" 
{
  void   Print(char* fmt, ...);
  void   PrintLn();
  void   PrintTCLS(const char c, const char * s);
#define PrintTCLS(A,B) Print("TCL-ErrS:%s",B)
  void   PrintS(char* s);

  void   Werror(char *fmt, ...);
  void   WerrorS(const char *s);
}
#endif

char *Tok2Cmdname( int tok);
