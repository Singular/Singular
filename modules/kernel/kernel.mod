/*
 *  $Id: kernel.mod,v 1.3 2000-12-05 15:26:58 obachman Exp $
 *
 *  Test mod fuer modgen
 */
module="kernel";
version="$Id: kernel.mod,v 1.3 2000-12-05 15:26:58 obachman Exp $";
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
