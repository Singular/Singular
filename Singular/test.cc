#include "kernel/mod2.h"

#include "omalloc/omalloc.h"

#include "factory/factory.h" // :(

#include "misc/intvec.h"
#include "misc/int64vec.h"
#include "misc/mylimits.h"
#include "misc/options.h"

#include "reporter/reporter.h"

#include "resources/feFopen.h"
#include "resources/feResource.h"

#include "coeffs/coeffs.h"

#include "coeffs/si_gmp.h"

#include "polys/kbuckets.h"
#include "polys/matpol.h"
#include "polys/mod_raw.h"
#include "polys/prCopy.h"
#include "polys/sbuckets.h"
#include "polys/simpleideals.h"
#include "polys/weight.h"

#include "polys/monomials/maps.h"
#include "polys/monomials/monomials.h"
#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"

#include "polys/nc/nc.h"
#include "polys/nc/ncSACache.h"
#include "polys/nc/ncSAFormula.h"
#include "polys/nc/ncSAMult.h"
#include "polys/nc/sca.h"
#include "polys/nc/summator.h"

#include "polys/templates/p_MemAdd.h"
#include "polys/templates/p_Procs.h"

#include "polys/operations/pShallowCopyDelete.h"

#include "polys/clapsing.h"

#include "coeffs/numbers.h"
#include "kernel/polys.h"


#include "kernel/structs.h"


// HEADERS:
#include "kernel/combinatorics/hutil.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/syz.h"
#include "kernel/maps/fast_maps.h"
#include "Singular/fevoices.h"
#include "kernel/groebner_walk/walkProc.h"
#include "kernel/groebner_walk/walkMain.h"
#include "kernel/groebner_walk/walkSupport.h"
#include "kernel/GBEngine/khstd.h"

#include "kernel/fglm/fglm.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/fglm/fglmgauss.h"
#include "kernel/fglm/fglmvec.h"
#include "kernel/GBEngine/kstdfac.h"
#include "kernel/spectrum/kmatrix.h"
#include "kernel/spectrum/GMPrat.h"
#include "kernel/spectrum/multicnt.h"
#include "kernel/spectrum/npolygon.h"
#include "kernel/spectrum/semic.h"
#include "kernel/spectrum/spectrum.h"
#include "kernel/spectrum/splist.h"
#include "kernel/spectrum/multicnt.h"
#include "kernel/linear_algebra/eigenval.h"
#include "kernel/GBEngine/units.h"
#include "kernel/GBEngine/ratgring.h"
#include "kernel/GBEngine/shiftgb.h"

#include "kernel/GBEngine/kutil.h"

#include "kernel/digitech.h"
#include "kernel/linear_algebra/eigenval.h"
#include "kernel/maps/fast_maps.h"
#include "kernel/fast_mult.h"
#include "kernel/oswrapper/feread.h"

#include "kernel/fglm/fglmgauss.h"
#include "kernel/fglm/fglm.h"
#include "kernel/fglm/fglmvec.h"

////////#include "F5cData.h"
#include "kernel/GBEngine/f5data.h"
#include "kernel/GBEngine/f5gb.h"
#include "kernel/GBEngine/f5lists.h"
////////#include "kernel/F5cLists.h"

#include "kernel/spectrum/GMPrat.h"
#include "kernel/combinatorics/hutil.h"

#include "kernel/ideals.h"

#include "kernel/spectrum/kmatrix.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kstdfac.h"
#include "kernel/GBEngine/khstd.h"

#include "kernel/linear_algebra/linearAlgebra.h"



#include "kernel/spectrum/multicnt.h"
#include "kernel/spectrum/npolygon.h"

#include "kernel/preimage.h"

#include "kernel/GBEngine/nc.h"

#include "kernel/GBEngine/ratgring.h"
#include "kernel/GBEngine/ringgb.h"
#include "kernel/spectrum/semic.h"
#include "kernel/GBEngine/shiftgb.h"

#include "kernel/spectrum/spectrum.h"
#include "kernel/spectrum/splist.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/syz.h"
// #include "kernel/testpoly.h" // Too old?

#include "kernel/GBEngine/tgbgauss.h"
#include "kernel/GBEngine/tgb.h"

#include "kernel/oswrapper/timer.h"

#include "kernel/GBEngine/units.h"
#include "kernel/groebner_walk/walkMain.h"
#include "kernel/groebner_walk/walkProc.h"
#include "kernel/groebner_walk/walkSupport.h"


// #include "polys/clapconv.h" // due to factory? :(
// #include "kernel/tgb_internal.h" // :(
// #include "kernel/F4.h" // uses tgb_internal // :(


// headers in Singular/
#include "Singular/mmalloc.h"
#include "Singular/attrib.h"
#include "Singular/blackbox.h"
#include "Singular/cntrlc.h"
#include "Singular/links/dbm_sl.h"
#include "Singular/distrib.h"
#include "Singular/eigenval_ip.h"
#include "Singular/gms.h"
#include "Singular/grammar.h"
#include "Singular/ipconv.h"
#include "Singular/ipid.h"
#include "Singular/ipprint.h"
#include "Singular/ipshell.h"
#include "Singular/libparse.h"
#include "Singular/lists.h"
#include "Singular/locals.h"
#include "Singular/maps_ip.h"
#include "Singular/misc_ip.h"
#include "Singular/links/ndbm.h"
#include "Singular/newstruct.h"
#include "Singular/pcv.h"
#include "Singular/links/pipeLink.h"
#include "Singular/sdb.h"
#include "Singular/links/silink.h"
#include "Singular/links/sing_dbm.h"
#include "Singular/links/slInit.h"
#include "Singular/links/ssiLink.h"
#include "Singular/stype.h"
#include "Singular/subexpr.h"
#include "Singular/tok.h"
#include "Singular/utils.h"
#include "Singular/walk.h"

#include "Singular/fegetopt.h"

void siInit(char *);

int main( int, char *argv[] )
{
  assume( sizeof(long) == SIZEOF_LONG );

  if( sizeof(long) != SIZEOF_LONG )
  {
     WerrorS("Bad config.h: wrong size of long!");

     return(1);
  }

   // init path names etc.
//  feInitResources(argv[0]); //???
  siInit(argv[0]); // ?

  if( char *s = versionString() )
  {
    PrintS(s);
    omFree(s);
  }



  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  StringAppendS("\n");
  if( char * s = StringEndS() )
  {
    PrintS(s);
    omFree(s);
  }




  // Libpolys tests:

  // construct the ring Z/32003[x,y,z]
  // the variable names
  char **n=(char**)omalloc(3*sizeof(char*));
  n[0]=omStrDup("x");
  n[1]=omStrDup("y");
  n[2]=omStrDup("z2");



/*
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
    const BOOLEAN res=iiMake_proc(datetime,NULL,NULL);
    if (res)
    {
      printf("iiMake_proc: datetime return an error\n");
      errorreported = 0;
    }
    else
    {
      printf("iiMake_proc: datetime returned type %d, >>%s<<\n", iiRETURNEXPR.Typ(), (char *)iiRETURNEXPR.Data());
      iiRETURNEXPR.CleanUp(); // calls Init afterwards
    }
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
     printf("typeof returned type %d, >>%s<<\n",r1.Typ(),(char*)r1.Data());

  // clean up r1:
  r1.CleanUp();

  return 0;
}

