/*
 *  $Id: kernel.mod,v 1.1 1998-11-19 15:49:22 krueger Exp $
 *
 *  Test mod fuer modgen
 */
module="kernel";
version="$Id: kernel.mod,v 1.1 1998-11-19 15:49:22 krueger Exp $";
info="
LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C

  proclist();    Lists all procedures.
";

cxxsource = proclist.cc
cxxsource = misc.cc
/*cxxsource = sscanf.cc*/

proc proclist=kProclist;

proc nn(string)=iiKernelMiscNN;

/*proc sscanf(string, string) = IOsscanf; */
