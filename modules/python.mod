%{
/*
 *  $Id$
 *
 *  Python module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Python.h>
#include <wrapper.h>

void mbpython(char* in);
%}

// some comments here

category="tests";
package="python_module";
version	= "$Id$";
info	="LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C python(input); eval a string  in python";
//files= wrapper.cc;
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
  mbpython(a);
  %return();
}
example
{
  python("print 1+1");
}



%%
%C

void mbpython(char* inp){

  PyRun_SimpleString(inp);


}



