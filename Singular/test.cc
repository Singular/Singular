#include <misc/auxiliary.h>

#ifdef HAVE_FACTORY
#include <factory/factory.h> // :(
#endif

#include <omalloc/omalloc.h>

#include <misc/intvec.h>
#include <misc/int64vec.h>
#include <misc/mylimits.h>
#include <misc/options.h>

#include <reporter/reporter.h>

#include <resources/feFopen.h>
#include <resources/feResource.h>

#include <coeffs/coeffs.h>

#include <coeffs/si_gmp.h>

#include <polys/kbuckets.h>
#include <polys/matpol.h>
#include <polys/mod_raw.h>
#include <polys/prCopy.h>
#include <polys/sbuckets.h>
#include <polys/simpleideals.h>
#include <polys/weight.h>

#include <polys/monomials/maps.h>
#include <polys/monomials/monomials.h>
#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>

#include <polys/nc/nc.h>
#include <polys/nc/ncSACache.h>
#include <polys/nc/ncSAFormula.h>
#include <polys/nc/ncSAMult.h>
#include <polys/nc/sca.h>
#include <polys/nc/summator.h>


#include <polys/templates/p_MemAdd.h>
#include <polys/templates/p_Procs.h>

#include <polys/operations/pShallowCopyDelete.h>

#include <polys/clapsing.h>


#ifdef HAVE_FACTORY
int initializeGMP(){ return 1; } // NEEDED FOR MAIN APP. LINKING!!!
int mmInit(void) {return 1; } // ? due to SINGULAR!!!...???
#endif


#include <coeffs/numbers.h>
#include <kernel/polys.h>


#include <kernel/mod2.h>

#include <kernel/structs.h>


// HEADERS:
#include <kernel/hutil.h>
//#include "idrec.h" // moved to Singular
#include <kernel/stairc.h>
#include <kernel/ideals.h>
#include <kernel/syz.h>
#include <kernel/fast_maps.h>
#include <kernel/febase.h>
#include <kernel/walkProc.h>
#include <kernel/walkMain.h>
#include <kernel/walkSupport.h>
#include <kernel/khstd.h>
/// #include <kernel/sparsmat.h> // TODO: install polys/this!
//+

#include <kernel/fglm.h>
#include <kernel/kstd1.h>
#include <kernel/fglmgauss.h>
#include <kernel/fglmvec.h>
#include <kernel/kstdfac.h>
#include <kernel/kmatrix.h>
#include <kernel/GMPrat.h>
#include <kernel/multicnt.h>
#include <kernel/npolygon.h>
#include <kernel/semic.h>
#include <kernel/spectrum.h>
#include <kernel/splist.h>
#include <kernel/multicnt.h>
#include <kernel/eigenval.h>
#include <kernel/units.h>
#include <kernel/fegetopt.h>
#include <kernel/ratgring.h>
#include <kernel/shiftgb.h>
#include <kernel/mmalloc.h>
#include <kernel/gfan.h>

#include <kernel/kutil.h>

// #include <kernel/dbm_sl.h> // TODO: needs si_link// already moved to Singular/!

// #include "CCRing.h" // Too old!
#include <kernel/digitech.h>
#include <kernel/eigenval.h>
#include <kernel/fast_maps.h>
#include <kernel/fast_mult.h>
#include <kernel/febase.h>
#include <kernel/fegetopt.h>

#include <kernel/fglmgauss.h>
#include <kernel/fglm.h>
#include <kernel/fglmvec.h>

////////#include "F5cData.h"
#include <kernel/f5c.h>
#include <kernel/f5data.h>
#include <kernel/f5gb.h>
#include <kernel/f5lists.h>
////////#include <kernel/F5cLists.h>

#include <kernel/gfan.h>


#include <kernel/GMPrat.h>

// #include "htmlhelp.h" // For Windows // 
#include <kernel/hutil.h>
// #include <kernel/Ideal.h> // Too old?


#include <kernel/ideals.h>

#include <kernel/kmatrix.h>
#include <kernel/kstd1.h>
#include <kernel/kstdfac.h>
#include <kernel/khstd.h>

#include <kernel/linearAlgebra.h>



// #include "lplist.h" // Too old!
#include <kernel/mmalloc.h>
#include <kernel/multicnt.h>
#include <kernel/npolygon.h>
// #include <kernel/Number.h> // Too old?
#include <kernel/page.h>
// #include <kernel/Poly.h> // Too old?
// #include <kernel/PowerSeries.h> // Too old?

#include <kernel/preimage.h>

#include <kernel/nc.h>

#include <kernel/ratgring.h>
#include <kernel/ringgb.h>
#include <kernel/semic.h>
#include <kernel/shiftgb.h>

#include <kernel/spectrum.h>
#include <kernel/splist.h>
#include <kernel/stairc.h>
#include <kernel/structs.h>
#include <kernel/syz.h>
// #include <kernel/testpoly.h> // Too old?

#include <kernel/tgbgauss.h>
#include <kernel/tgb.h>

#include <kernel/timer.h>

#include <kernel/units.h>
#include <kernel/walkMain.h>
#include <kernel/walkProc.h>
#include <kernel/walkSupport.h>


// #include <polys/clapconv.h> // due to factory? :(
// #include <kernel/tgb_internal.h> // :(
// #include <kernel/F4.h> // uses tgb_internal // :(
// #include <kernel/IIntvec.h> // :(



// headers in Singular/
#include "attrib.h"
#include "bbcone.h"
#include "bigintm.h"
#include "blackbox.h"
#include "Cache.h"
#include "CacheImplementation.h"
#include "cntrlc.h"
#include "dbm_sl.h"
#include "distrib.h"
#include "eigenval_ip.h"
#include "feOpt.h"
#include "gms.h"
#include "grammar.h"
#include "interpolation.h"
#include "ipconv.h"
#include "ipid.h"
#include "ipprint.h"
#include "ipshell.h"
#include "janet.h"
#include "libparse.h"
#include "lists.h"
#include "locals.h"
#include "maps_ip.h"
#include "Minor.h"
#include "MinorInterface.h"
#include "MinorProcessor.h"
#include "minpoly.h"
#include "misc_ip.h"
#include "mpsr_Get.h"
#include "mpsr.h"
#include "mpsr_Put.h"
#include "mpsr_sl.h"
#include "mpsr_Timer.h"
#include "mpsr_Tok.h"
#include "ndbm.h"
#include "newstruct.h"
#include "omSingularConfig.h"
#include "pcv.h"
#include "pipeLink.h"
#include "pyobject_setup.h"
#include "run.h"
#include "sdb.h"
#include "silink.h"
#include "sing_dbm.h"
#include "sing_win.h"
#include "slInit.h"
#include "ssiLink.h"
#include "static.h"
#include "stype.h"
#include "subexpr.h"
#include "table.h"
#include "tok.h"
#include "utils.h"
#include "walk.h"


// Source files in Singular/
/*
#include "attrib.cc"
#include "bbcone.cc"
#include "bbfan.cc"
#include "bigintm.cc"
#include "blackbox.cc"
#include "calcSVD.cc"
#include "claptmpl.cc"
#include "cntrlc.cc"
#include "denom_list.cc"
#include "eigenval_ip.cc"
#include "emacs.cc"
#include "extra.cc"
#include "fehelp.cc"
#include "feOpt.cc"
#include "fglm.cc"
#include "gentable2.cc"
#include "gentable.cc"
#include "gms.cc"
#include "grammar.cc"
#include "interpolation.cc"
#include "iparith.cc"
#include "ipassign.cc"
#include "ipconv.cc"
#include "ipid.cc"
#include "iplib.cc"
#include "ipprint.cc"
#include "ipshell.cc"
#include "janet.cc"
#include "libparse.cc"
#include "linearAlgebra_ip.cc"
#include "lists.cc"
#include "maps_ip.cc"
#include "Minor.cc"
#include "MinorInterface.cc"
#include "MinorProcessor.cc"
#include "minpoly.cc"
#include "misc_ip.cc"
#include "mpsr_Error.cc"
#include "mpsr_Get.cc"
#include "mpsr_GetMisc.cc"
#include "mpsr_GetPoly.cc"
#include "mpsr_Put.cc"
#include "mpsr_PutPoly.cc"
#include "mpsr_sl.cc"
#include "mpsr_Timer.cc"
#include "mpsr_Tok.cc"
#include "ndbm.cc"
#include "newstruct.cc"
#include "pcv.cc"
#include "pipeLink.cc"
#include "pyobject.cc"
#include "pyobject_setup.cc"
#include "scanner.cc"
#include "sdb.cc"
#include "silink.cc"
#include "sing_dbm.cc"
#include "sing_win.cc"
#include "slInit_Dynamic.cc"
#include "slInit_Static.cc"
#include "ssiLink.cc"
#include "subexpr.cc"
#include "test.cc"
#include "tesths.cc"
#include "utils.cc"
#include "walk.cc"
#include "walk_ip.cc"
#include "wrapper.cc"
*/

int main( int, char *argv[] ) 
{
  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintS(StringAppendS("\n"));
   
  return 0;
}

