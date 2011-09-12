#include "mod2.h"

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
// The following are needed due to FACTORY (e.g. initCanonicalForm)
int initializeGMP(){ return 1; } 
int mmInit(void) {return 1; }
#endif


#include <coeffs/numbers.h>

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
// #include "mmalloc.h" // move to Singular!?
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
// #include "run.h" // moved to Singular
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




// #include "fglmzero.cc" // looks like <factory/templates/ftmpl_list.h> must be installed!
// TODO: looks like <coeffs/mpr_complex.h> must be installed!



#include "polys.h"

int main( int, char *argv[] ) 
{
  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintS(StringAppendS("\n"));

   
  // Libpolys tests:
   
  // construct the ring Z/32003[x,y,z]
  // the variable names
  char **n=(char**)omalloc(3*sizeof(char*));
  n[0]=omStrDup("x");
  n[1]=omStrDup("y");
  n[2]=omStrDup("z2");

  ring R = rDefault(32003,3,n); //  ring R = rDefault(0,3,n);

  rWrite(R); PrintLn();

#ifdef RDEBUG
  rDebugPrint(R);
#endif

  
  poly p = p_ISet(1,R); p_SetExp(p,1,1, R); p_Setm(p, R);

  assume( p_GetExp(p,1, R) == 1 );
  
  poly pp = pp_Mult_qq( p, p, R);

  Print("p: "); p_Write0(p, R);
  Print("; p*p : "); p_Write(pp, R); 

  p_Delete(&p, R);
  
  assume( p_GetExp(pp,1, R) == 2 );
  
  p_Delete(&pp, R);
  
  
//  rDelete(R);
   
  // make R the default ring:
  rChangeCurrRing(R);
  
  // create the polynomial 1
  poly p1=pISet(1);

  // create tthe polynomial 2*x^3*z^2
  poly p2=p_ISet(2,R);
  pSetExp(p2,1,3);
  pSetExp(p2,3,2);
  pSetm(p2);

  // print p1 + p2
  Print("p1: "); pWrite0(p1); 
  Print(" + p2: "); pWrite0(p2); 
  Print("  ---- >>>> ");

  // compute p1+p2
  p1=p_Add_q(p1,p2,R); p2=NULL;
  pWrite(p1); 

  // clean up:
//  pDelete(&p1);
   
  rDelete(R); // should cleanup every belonging polynomial, right!?
   
   
  return 0;
}
