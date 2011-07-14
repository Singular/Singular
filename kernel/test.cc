#include <omalloc/omalloc.h>

#include <misc/auxiliary.h>
#include <misc/intvec.h>
#include <misc/mylimits.h>
#include <misc/options.h>

#include <reporter/reporter.h>

#include <resources/feFopen.h>
#include <resources/feResource.h>

#include <coeffs/coeffs.h>

// #include <coeffs/si_gmp.h>

#include <polys/clapconv.h>
#include <polys/clapsing.h>
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

#include <polys/operations/pShallowCopyDelete.h>

#include <polys/templates/p_MemAdd.h>
#include <polys/templates/p_Procs.h>

extern ring currRing;

//#ifdef HAVE_FACTORY
//int initializeGMP(){ return 1; } // NEEDED FOR MAIN APP. LINKING!!!
//#endif


#include <coeffs/numbers.h>
#include <polys/polys.h>

#include "mod2.h"

  
// HEADERS:

#include "hutil.h"
#include "idrec.h"
#include "stairc.h"
#include "ideals.h"
#include "structs.h"
#include "int64vec.h"
#include "syz.h"
#include "fast_maps.h"
#include "febase.h"
#include "walkProc.h"
#include "walkMain.h"
#include "walkSupport.h"
#include "khstd.h"
/// #include "sparsmat.h" // TODO: install polys/this!
#include "fglm.h"
#include "kstd1.h"
#include "fglmgauss.h"
#include "fglmvec.h"
#include "kstdfac.h"
#include "kmatrix.h"
#include "dbm_sl.h"
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

// #include "kutil.h" // ???


/* ALL: */

// #include "CCRing.h" // Too old!
#include "dbm_sl.h"
#include "digitech.h"
#include "eigenval.h"
// #include "F4.h" // uses tgb_internal
#include "F5cData.h"
#include "f5c.h"
#include "F5cLists.h"
#include "f5data.h"
#include "f5gb.h"
#include "f5lists.h"
#include "fast_maps.h"
#include "fast_mult.h"
#include "febase.h"
#include "fegetopt.h"
#include "fglmgauss.h"
#include "fglm.h"
#include "fglmvec.h"
#include "gfan.h"
#include "GMPrat.h"
// #include "htmlhelp.h" // For Windows // 
#include "hutil.h"
// #include "Ideal.h" // Too old?
#include "ideals.h"
#include "idrec.h"
#include "IIntvec.h"
#include "int64vec.h"
#include "khstd.h"
#include "kmatrix.h"
#include "kstd1.h"
#include "kstdfac.h"

#include "linearAlgebra.h"
// #include "lplist.h" // Too old!
#include "mmalloc.h"
#include "multicnt.h"
#include "npolygon.h"
// #include "Number.h" // Too old?
#include "page.h"
// #include "Poly.h" // Too old?
// #include "PowerSeries.h" // Too old?
#include "ratgring.h"
#include "ringgb.h"
#include "run.h"
#include "semic.h"
#include "shiftgb.h"
#include "si_gmp.h"
#include "spectrum.h"
#include "splist.h"
#include "stairc.h"
#include "structs.h"
#include "syz.h"
// #include "testpoly.h" // Too old?

#include "tgbgauss.h"
#include "tgb.h"
// #include "tgb_internal.h" // uses kutil :(
#include "timer.h"
#include "units.h"
#include "walkMain.h"
#include "walkProc.h"
#include "walkSupport.h"

/* */
