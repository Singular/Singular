%{
/*
 *  $Id: demo.mod,v 1.1 2000-03-29 13:47:24 krueger Exp $
 *
 *  Test mod fuer modgen
 */

#include <stdio.h>

extern void piShowProcList();
%}

// some comments here

module="demo_module";
version	= "$Id: demo.mod,v 1.1 2000-03-29 13:47:24 krueger Exp $";
info	="
LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C

  proclist();    Lists all procedures.
";

//%{
//static int i;
//%}

other ="hallo";   // should return an error

//cxxsource = proclist.cc , misc.cc;
//cxxsource = misc.cc;
/*cxxsource = sscanf.cc 
*/

%%

/*
 * NAME:     proclist
 *           procedure without any parameter and no return value;
 * PURPOSE:  shows a list of all defined procedure.
 */
proclist
"gives of list all procedures.
 RETURN none
"
 {
  piShowProcList();
  %return;
}
example
{
  proclist();
}

/*
 *
 */
static start_sg
{
//  %singularcmd;
//  %singularcmd();
  %singularcmd(inout::f(q,w,e));
}

/*
 */
int mysum(
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
  
  %return = (void*)((int)i->Data() + (int)j->Data());
}
example
{ Demo::mysum(2,3); 
}

ideal toid()
// call function toid();
{
  // generate internal variable declaration
  // (will write nothing: there are no parameters
  %declaration;

  // generate typecheck and typeconversation code
  // (will write nothing: there are no parameters)
  %typecheck;

  // generate return vector
  // data=toid();
  //%return(my_toid);
  return FALSE;

};

ideal toid2(ideal) {
  // generate internal variable declaration
  //
  %declaration;

  return FALSE;
//  %return();
};

//	typedef,
k_test(
	ideal,
	int,
	intvec,
	intmat,
	link,
	list,
	map,
	matrix,
	module,
	number,
	package,
	poly,
	proc,
	qring,
	ring,
	string,
	vector
)
//function="k_test_func";
{
  %declaration;
  
  printf("Nur ein Test\n");

  %typecheck;
  %return;
  
}

into {
}

/*
string nn(string) {
  function=iiKernelMiscNN;
};
*/

%%

/*proc sscanf(string, string) = IOsscanf; */

void mytest()
{
  printf("Test\n");
  printf("here commes some other C-text\n");
}
