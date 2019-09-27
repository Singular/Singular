#include "kernel/mod2.h" // general settings/macros
#include "Singular/ipid.h" // for SModulFunctions, leftv
BOOLEAN hello(leftv result, leftv arg)
{
	result->rtyp=NONE; // set the result type
	PrintS("hello world\n");
	return FALSE; // return FALSE: no error
}
extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
	// this is the initialization routine of the module
	// adding the routine hello:
	psModulFunctions->iiAddCproc(
			(currPack->libname? currPack->libname: ""),
			// the library name,
			// rely on the loader to set it in currPack->libname
			"hello",// the name for the singular interpreter
			FALSE, // should enter the global name space
			hello); // the C/C++ routine
	return 1;
}
