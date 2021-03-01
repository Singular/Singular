#include "kernel/mod2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/stat.h>
#include "Singular/tok.h"
#include "kernel/structs.h"
#include "Singular/mod_lib.h"
#include "Singular/ipid.h"

#include "Singular/locals.h"

#include "omalloc/omalloc.h"

#include <time.h>
#ifdef HAVE_PYTHON
#include <boost/python.hpp>
#include <Python.h>
#include <Python.h>
#include "wrapper.h"

static BOOLEAN mod_python(leftv __res, leftv __h)
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
//int mod_init(
//  int (*iiAddCproc)(char *libname, char *procname, BOOLEAN pstatic,
//              BOOLEAN(*func)(leftv res, leftv v))
//  )
extern "C" int SI_MOD_INIT(python_module)(SModulFunctions* psModulFunctions)
{
  VAR char *py=getenv("PYTHONPATH");
  VAR char buf[1024];
  if (py!=NULL)
  {
    strcpy(buf,py);
    strcat(buf,":");
    strcat(buf,feResource('s',FALSE));
  }
  else
    strcpy(buf,feResource('s',FALSE));
  Print("setting PYTHONAPTH to %s\n",buf);
  setenv("PYTHONPATH",buf,1);

  Py_Initialize();
  PyRun_SimpleString("from sys import path\n\
path.insert(0,'.')\n");
  initSingular();
  init_Singular();

  psModulFunctions->iiAddCproc(currPack->libname,"python",FALSE, mod_python);
  return MAX_TOK;
}
#else
extern "C" int SI_MOD_INIT(python_module)(SModulFunctions* psModulFunctions)
{
  PrintS("no python_mod support\n");
  return MAX_TOK;
}
#endif
