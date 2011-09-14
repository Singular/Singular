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
#include "mmalloc.h"
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
#include "mmalloc.h"
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


void siInit(char *);

int main( int, char *argv[] ) 
{
  // init path names etc.
//  feInitResources(argv[0]); //???
  siInit(argv[0]); // ?

  // Libpolys tests:
   
  // construct the ring Z/32003[x,y,z]
  // the variable names
  char **n=(char**)omalloc(3*sizeof(char*));
  n[0]=omStrDup("x");
  n[1]=omStrDup("y");
  n[2]=omStrDup("z2");

   
/*
  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintS(StringAppendS("\n"));

   
   
  ring R=rDefault(32003,3,n);
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
  pWrite(p1); printf(" + \n"); pWrite(p2); printf("\n");

  // compute p1+p2
  p1=p_Add_q(p1,p2,R); p2=NULL;
  pWrite(p1); 

  // clean up:
  pDelete(&p1);
   
  rDelete(R);
  rChangeCurrRing(NULL);
   
*/
   
   
   
  currentVoice=feInitStdin(NULL);
   
  int err=iiEStart(omStrDup("ring R = (0, a), x, dp; R; system(\"r\", R); minpoly=a*a+1; R; system(\"r\", R); kill R; return();\n"),NULL);
  
  printf("interpreter returns %d\n",err);
  if (err) 
     errorreported = 0; // reset error handling
   
  assume( err == 0 );
   
   
   
  // hook for error handling:
  // WerrorS_callback=......; of type p(const char *)
  err=iiEStart(omStrDup("int ver=system(\"version\");export ver;return();\n"),NULL);
  
  printf("interpreter returns %d\n",err);
  if (err) 
     errorreported = 0; // reset error handling
   
  assume( err == 0 );

  idhdl h=ggetid("ver");
   
  if (h != NULL)
    printf("singular variable ver of type %d contains %d\n",h->typ,(int)(long)IDDATA(h));
  else
    printf("variable ver does not exist\n");
   
  assume( h != NULL );

   
  err = iiEStart(
		 omStrDup("system(\"--version\");return();\n"),
		 NULL);
   
  printf("interpreter returns %d\n",err);
  if (err) 
     errorreported = 0; // reset error handling   
   
  assume( err == 0 );
   
  // calling a singular-library function
  idhdl datetime=ggetid("datetime");
  if (datetime==NULL)
    printf("datetime not found\n");
  else
  {
    leftv res=iiMake_proc(datetime,NULL,NULL);
    if (res==NULL) { printf("datetime return an error\n"); errorreported = 0; }
    else           printf("datetime returned type %d, >>%s<<\n",res->Typ(),(char *)res->Data());
  }

  // changing a ring for the interpreter
  // re-using n and R from above
  ring R = rDefault(32003, 3, n);
  idhdl newRingHdl=enterid("R" /* ring name*/,
                           0, /*nesting level, 0=global*/
                           RING_CMD,
                           &IDROOT,
                           FALSE);
  
  IDRING(newRingHdl)=R;
  // make R the default ring (include rChangeCurrRing):
  rSetHdl(newRingHdl);
  err=iiEStart(omStrDup("R; system(\"r\", R); poly p=x; p; system(\"p\", p); \"\"; poly pp = p * p; pp; listvar(); return();\n"),NULL);

  // calling a kernel function via the interpreter interface
  sleftv r1; memset(&r1,0,sizeof(r1));
  sleftv arg; memset(&arg,0,sizeof(r1));
  arg.rtyp=STRING_CMD;
  arg.data=omStrDup("huhu");
  err=iiExprArith1(&r1,&arg,TYPEOF_CMD);
  
  printf("interpreter returns %d\n",err);
  if (err) 
     errorreported = 0; // reset error handling   
  else 
     printf("typeof returned type %d, >>%s<<\n",r1.Typ(),r1.Data());
   
  // clean up r1:
  r1.CleanUp();
   
  return 0;
}

