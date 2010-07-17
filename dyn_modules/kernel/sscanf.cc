/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interpreter: LIB and help
*/

#include <locals.h>
//#include <varargs.h>
#include <stdarg.h>

int MYsscanf(const char *s, const char *fmt,...)
{
  va_list args;
  int ret = 0;

  va_start (args, fmt, s);
  ret = vsscanf (s, fmt, args);
  va_end (args);
  return 0;
}


BOOLEAN IOsscanf(leftv res, leftv h)
{
  //void *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10;
  void *args;
  char *input = (char *)mstrdup((char*)(h->Data()));
  const char *format = (char *)mstrdup((char*)(h->next->Data()));

  //va_start(args , format);

  Print("sscanf(%s,%s,<va_list>)\n\n", input, format);
  //sscanf(input, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
  sscanf(input, format, &args);
  Print("test 3\n");
  Print("sscanf(%s,%s,<va_list>)\n", input, format);
  //Print("sscanf(%s,%s,%s[%d])\n", input, format, (char *)a1, (int)a2);
  //Print("sscanf(%s,%s,%s[%d])\n", input, format, (char *)args[0], (int)args[1]);
Print("sscanf(%s,%s,%s)\n", input, format, (char *)args[0]);  
  //va_end(args);
  
  FreeL(input);
  FreeL(format);
  res->rtyp=NONE;
  return FALSE;
}

