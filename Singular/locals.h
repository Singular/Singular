/*
 * $Id$
 * part of modgen
 */

#include <Singular/mod2.h>
#include <Singular/tok.h>
#include <Singular/ipid.h>
//#include <mmemory.h>
#include <kernel/febase.h>
#include <Singular/subexpr.h>
//#include <utils.h>
#include <kernel/structs.h>
#include <Singular/ipconv.h>

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

const char *Tok2Cmdname( int tok);
