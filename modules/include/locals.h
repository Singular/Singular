/*
 * $Id: locals.h,v 1.1 1998-11-19 16:16:06 krueger Exp $
 */

#include <mod2.h>
#include <tok.h>
#include <ipid.h>
#include <mmemory.h>
#include <febase.h>
#include <subexpr.h>
//#include <utils.h>
#include <structs.h>

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
