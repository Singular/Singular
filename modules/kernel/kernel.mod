/*
 *  $Id: kernel.mod,v 1.2 1999-03-31 22:09:53 krueger Exp $
 *
 *  Test mod fuer modgen
 */
module="kernel";
version="$Id: kernel.mod,v 1.2 1999-03-31 22:09:53 krueger Exp $";
info="
LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITEN IN C

  proclist();    Lists all procedures.
";

cxxsource = proclist.cc
cxxsource = misc.cc
/*cxxsource = sscanf.cc*/

proc proclist {
 function=piShowProcList;
};

proc ideal toid(ideal) {
function=toid;
}

/*
proc string nn(string) {
  function=iiKernelMiscNN;
};
*/

/*proc sscanf(string, string) = IOsscanf; */
