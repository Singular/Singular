/*
 *  $Id$
 *
 *  Test mod fuer modgen
 */
module="kernel";
version="$Id$";
info="
LIBRARY: kernel.lib  PROCEDURES OF GENERAL TYPE WRITTEN IN C

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
