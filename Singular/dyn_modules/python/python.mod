#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Python.h>
#include <wrapper.h>

void mbpython(char* in);
%modinitial
  Py_Initialize();
  initSingular();
  init_Singular();

%endinitial

%procedures
/*
 * NAME:     python
 *           procedure without string as parameter and no return val
 * PURPOSE:  interprets the string in python
 */
none python(string a)
{
  %declaration;
  %typecheck;
  PyRun_SimpleString(a);
  %return();
}
%C


