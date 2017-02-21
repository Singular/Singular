#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/stat.h>
#include <boost/python.hpp>
#include <Python.h>
#include <kernel/mod2.h>
#include <tok.h>
#include <structs.h>
#include <ipid.h>

#include <locals.h>
#include <omalloc.h>
#include "python.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Python.h>
#include "wrapper.h"

extern "C" {
//int mod_init(
//  int (*iiAddCproc)(char *libname, char *procname, BOOLEAN pstatic,
//              BOOLEAN(*func)(leftv res, leftv v))
//  )
  int mod_init( SModulFunctions* psModulFunctions)
{
  enter_id("category","tests", STRING_CMD);
  enter_id("version","$Id$", STRING_CMD);
  enter_id("info","LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C python(input); eval a string  in python", STRING_CMD);
  Py_Initialize();
  PyRun_SimpleString("from sys import path\n\
path.insert(0,'.')\n");
  initSingular();
  init_Singular();

  psModulFunctions->iiAddCproc(currPack->libname,"python",FALSE, mod_python);
  return 0;
}

BOOLEAN mod_python(leftv __res, leftv __h)
{
  leftv __v = __h, __v_save;
  int __tok = NONE, __index = 0;
  sleftv __sa; leftv __za = &__sa;
  char * a;
  if(__v==NULL) goto mod_python_error;
  __tok = __v->Typ();
  if((__index=iiTestConvert(__tok, STRING_CMD))==0)
     goto mod_python_error;
  __v_save = __v->next;
  __v->next = NULL;
  if(iiConvert(__tok, STRING_CMD, __index, __v, __za))
     goto mod_python_error;
  __v = __v_save;
  a = (char *)__za->Data();
  if(__v!=NULL) { __tok = __v->Typ(); goto mod_python_error; }

  PyRun_SimpleString(a);
  __res->data = NULL;
  __res->rtyp = END_RING;
  return FALSE;

  mod_python_error:
    Werror("python(`%s`) is not supported", Tok2Cmdname(__tok));
    Werror("expected python('string')");
    return TRUE;
}
