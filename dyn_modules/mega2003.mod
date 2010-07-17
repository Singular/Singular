%{
/*
 *  $Id$
 *
 *  Test mod fuer modgen
 */

#include <stdio.h>
#include <mod2.h>
#include <tok.h>
#include <polys.h>


extern void piShowProcList();
%}

category="mega";
package="mega_module";
version	= "$Id$";
info	="
LIBRARY: mega.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C

  proclist();    Lists all procedures.
";

%%

%procedures

/*
 */
poly mysum(
	poly i,
	poly j
	) 
"Return sum of i and j"
{
   poly h;
  // generate internal variable declaration
  //
  %declaration;

  // generate typecheck and typeconversation code
  %typecheck;
  
  h = pCopy(i);
  %return = (void *)pAdd(h , j);
}
int mysumint(
	int i,
	int j
	) 
"Return sum of i and j"
{
  // generate internal variable declaration
  //
  %declaration;

  // generate typecheck and typeconversation code
  %typecheck;
  
  //%return = (void*)((int)i->Data() + (int)j->Data());
  %return = (void *)(i + j);
}
example
{ Demo::mysum(2,3); 
}


%%
%C

void mytest()
{
  printf("Test\n");
  printf("here commes some other C-text\n");
}
