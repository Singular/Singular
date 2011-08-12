#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#ifdef HAVE_FACTORY
#include <factory/factory.h> // :(
#endif

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
#endif


#include <coeffs/numbers.h>
#include <polys/polys.h>

#include "mod2.h"
#include "structs.h"


// HEADERS:
#include "hutil.h"
//#include "idrec.h" // moved to Singular
#include "stairc.h"
#include "ideals.h"
#include "syz.h"
#include "fast_maps.h"
#include "febase.h"
#include "walkProc.h"
#include "walkMain.h"
#include "walkSupport.h"
#include "khstd.h"
/// #include "sparsmat.h" // TODO: install polys/this!
//+

#include "fglm.h"
#include "kstd1.h"
#include "fglmgauss.h"
#include "fglmvec.h"
#include "kstdfac.h"
#include "kmatrix.h"
#include "GMPrat.h"
#include "multicnt.h"
#include "npolygon.h"
#include "semic.h"
#include "spectrum.h"
#include "splist.h"
#include "multicnt.h"
#include "eigenval.h"
#include "units.h"
#include "fegetopt.h"
#include "ratgring.h"
#include "shiftgb.h"
#include "mmalloc.h"
#include "gfan.h"

#include "kutil.h"

// #include "dbm_sl.h" // TODO: needs si_link// already moved to Singular/!

// #include "CCRing.h" // Too old!
#include "digitech.h"
#include "eigenval.h"
#include "fast_maps.h"
#include "fast_mult.h"
#include "febase.h"
#include "fegetopt.h"

#include "fglmgauss.h"
#include "fglm.h"
#include "fglmvec.h"

////////#include "F5cData.h"
#include "f5c.h"
#include "f5data.h"
#include "f5gb.h"
#include "f5lists.h"
////////#include "F5cLists.h"

#include "gfan.h"


#include "GMPrat.h"

// #include "htmlhelp.h" // For Windows // 
#include "hutil.h"
// #include "Ideal.h" // Too old?


#include "ideals.h"

#include "kmatrix.h"
#include "kstd1.h"
#include "kstdfac.h"
#include "khstd.h"

#include "linearAlgebra.h"



// #include "lplist.h" // Too old!
#include "mmalloc.h"
#include "multicnt.h"
#include "npolygon.h"
// #include "Number.h" // Too old?
#include "page.h"
// #include "Poly.h" // Too old?
// #include "PowerSeries.h" // Too old?

#include "preimage.h"

#include "nc.h"

#include "ratgring.h"
#include "ringgb.h"
#include "run.h"
#include "semic.h"
#include "shiftgb.h"
// #include "si_gmp.h" // remove it...
#include "spectrum.h"
#include "splist.h"
#include "stairc.h"
#include "structs.h"
#include "syz.h"
// #include "testpoly.h" // Too old?

#include "tgbgauss.h"
#include "tgb.h"

#include "timer.h"

#include "units.h"
#include "walkMain.h"
#include "walkProc.h"
#include "walkSupport.h"


// #include <polys/clapconv.h> // due to factory? :(
// #include "tgb_internal.h" // :(
// #include "F4.h" // uses tgb_internal // :(
// #include "IIntvec.h" // :(




// Sources:
/*
#include "febase.cc"
#include "feread.cc"
#include "hdegree.cc"
#include "hilb.cc"
#include "hutil.cc"
#include "gr_kstd2.cc"
#include "ideals.cc"
#include "int64vec.cc"
#include "khstd.cc"
#include "kstdfac.cc"
#include "kstd1.cc"
#include "kstd2.cc"
#include "kutil.cc"
// #include "maps.cc" // moved to polys
// #include "matpol.cc" // same!
#include "misc.cc"
// #include "sparsmat.cc" // same!
#include "fast_maps.cc"
#include "fglmzero.cc" // looks like <factory/templates/ftmpl_list.h> must be installed!
#include "fglmvec.cc"
#include "fglmgauss.cc"
#include "fglmhom.cc"
#include "fglmcomb.cc"
// #include "numbers.cc" // moved
// #include "polys.cc" // same
// #include "p_polys.cc" // same
// #include "polys0.cc" // same
// #include "polys1.cc" // same
// #include "polys-impl.cc" // same
#include "kspoly.cc"
#include "syz.cc"
#include "syz0.cc"
#include "syz1.cc"
#include "syz2.cc"
#include "syz3.cc"
#include "timer.cc"
#include "GMPrat.cc"
#include "multicnt.cc"
#include "npolygon.cc"
#include "semic.cc"
#include "spectrum.cc"
#include "splist.cc"
#include "walkProc.cc"
#include "walkMain.cc"
#include "walkSupport.cc"
#include "eigenval.cc"
#include "units.cc"
#include "fast_mult.cc"
#include "digitech.cc"
#include "tgb.cc"
#include "tgbgauss.cc"
#include "ringgb.cc"
#include "f5data.cc"
#include "f5lists.cc"
#include "f5gb.cc"
#include "f5c.cc"
#include "F5cLists.cc"
#include "ratgring.cc"
#include "shiftgb.cc"
#include "gfan.cc"
#include "linearAlgebra.cc"

  
// TODO: looks like <coeffs/mpr_complex.h> must be installed!

// Now ALL the sources from here...
#include "digitech.cc"
#include "eigenval.cc"
#include "F4.cc"
#include "f5c.cc"
#include "F5cData.cc"
#include "F5cLists.cc"
#include "f5data.cc"
#include "f5gb.cc"
#include "f5lists.cc"
#include "fast_maps.cc"
#include "fast_mult.cc"
#include "febase.cc"
#include "feread.cc"
#include "fglmcomb.cc"
#include "fglmgauss.cc"
#include "fglmhom.cc"
#include "fglmvec.cc"
#include "fglmzero.cc"
#include "gfan.cc"
#include "GMPrat.cc"
#include "gr_kstd2.cc"
#include "hdegree.cc"
#include "hilb.cc"
#include "hutil.cc"
#include "ideals.cc"
#include "int64vec.cc"
#include "khstd.cc"
#include "kInline.h"
#include "kpolys.cc"
#include "kspoly.cc"
#include "kstd1.cc"
#include "kstd2.cc"
#include "kstdfac.cc"
#include "kutil.cc"
#include "linearAlgebra.cc"
#include "misc.cc"
#include "mmalloc.cc"
#include "multicnt.cc"
#include "npolygon.cc"
#include "ratgring.cc"
#include "ringgb.cc"
#include "semic.cc"
#include "shiftgb.cc"
#include "spectrum.cc"
#include "splist.cc"
#include "syz0.cc"
#include "syz1.cc"
#include "syz2.cc"
#include "syz3.cc"
#include "syz.cc"
#include "tgb.cc"
#include "tgbgauss.cc"
#include "timer.cc"
#include "units.cc"
#include "walkMain.cc"
#include "walkProc.cc"
#include "walkSupport.cc"
*/



int main( int, char *argv[] ) 
{
  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintS(StringAppendS("\n"));
   
  return 0;
}
