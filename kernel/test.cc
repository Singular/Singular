#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include "mod2.h"

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <factory/factory.h> // :(

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


// // TODO: DUE to the use of HALT in npolygon.cc :(((
extern "C" {void m2_end(int i){exit(i);}}

// // TODO: DUE to its use in kutil.cc :(((
char * showOption(){return NULL;}

// // TODO: DUE to its use in feread.cc :(((
char *iiArithGetCmd(int nPos){return NULL; }


#include <coeffs/numbers.h>

#include "structs.h"


// HEADERS:
#include <kernel/hutil.h>
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

#include <kernel/fglm/fglm.h>
#include <kernel/kstd1.h>
#include <kernel/fglm/fglmgauss.h>
#include <kernel/fglm/fglmvec.h>
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
#include <kernel/ratgring.h>
#include <kernel/shiftgb.h>


#include <kernel/kutil.h>

// #include "CCRing.h" // Too old!
#include <kernel/digitech.h>
#include <kernel/eigenval.h>
#include <kernel/fast_maps.h>
#include <kernel/fast_mult.h>
#include <kernel/febase.h>

#include <kernel/fglm/fglmgauss.h>
#include <kernel/fglm/fglm.h>
#include <kernel/fglm/fglmvec.h>

////////#include <kernel/F5cData.h>
#include <kernel/f5c.h>
#include <kernel/f5data.h>
#include <kernel/f5gb.h>
#include <kernel/f5lists.h>
////////#include <kernel/F5cLists.h>


// #include "Ideal.h" // Too old?


#include <kernel/ideals.h>

#include <kernel/kmatrix.h>
#include <kernel/kstd1.h>
#include <kernel/kstdfac.h>
#include <kernel/khstd.h>

#include <kernel/linearAlgebra.h>



// #include <kernel/lplist.h> // Too old!
#include <kernel/multicnt.h>
#include <kernel/npolygon.h>
// #include "Number.h" // Too old?
// #include "Poly.h" // Too old?
// #include "PowerSeries.h" // Too old?

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

#include <kernel/janet.h>
#include <kernel/interpolation.h>
#include <kernel/minpoly.h>

#include <kernel/Minor.h>
#include <kernel/MinorInterface.h>
#include <kernel/MinorProcessor.h>
#include <kernel/Cache.h>
#include <kernel/CacheImplementation.h>

// #include <polys/clapconv.h> // due to factory? :(
// #include <kernel/tgb_internal.h> // :(
// #include <kernel/F4.h> // uses tgb_internal // :(
// #include <kernel/IIntvec.h> // :(




// #include <kernel/fglm/fglmzero.cc> // looks like <factory/templates/ftmpl_list.h> must be installed!
// TODO: looks like <coeffs/mpr_complex.h> must be installed!



#include <kernel/polys.h>

void TestGBEngine()
{

  //  R = MPolynomialRing_polydict(QQ,5,'w,x,y,z,C', order='degrevlex')
  //  J = (w*w - x*z, w*x - y*z, x*x - w*y, x*y - z*z, y*y - w*z)

  const short w = 1;
  const short x = 2;
  const short y = 3;
  const short z = 4;

  const short N = (z - w + 1);

  char **n=(char**)omalloc(N*sizeof(char*));


  n[w-1]=omStrDup("w");
  n[x-1]=omStrDup("x");
  n[y-1]=omStrDup("y");
  n[z-1]=omStrDup("z");


  const int D = 3;
  int *order = (int *) omAlloc0(D* sizeof(int));
  int *block0 = (int *)omAlloc0(D * sizeof(int));
  int *block1 = (int *)omAlloc0(D * sizeof(int));

  order[0]  = ringorder_dp;
  block0[0] = 1;
  block1[0] = N;

  order[1]  = ringorder_C;
  block0[1] = 1;
  block1[1] = N;

  ring R = rDefault(0, N, n, D, order, block0, block1);

//   ring R = rDefault(0, N, n);

  rWrite(R); PrintLn();

#ifdef RDEBUG
  rDebugPrint(R);
#endif

  ideal I = idInit(5, 1);

  int gen = 0;

  {
    // -xz
    poly p = p_ISet(-1,R);

    p_SetExp(p, x, 1, R);
    p_SetExp(p, z, 1, R);
    p_Setm(p, R);

    assume( p_GetExp(p, x, R) == 1 );
    assume( p_GetExp(p, z, R) == 1 );
    assume( p_GetExp(p, w, R) == 0 );
    assume( p_GetExp(p, y, R) == 0 );

    // +w2
    poly lp = p_ISet(1,R);
    p_SetExp(lp, w, 2, R);
    p_Setm(lp, R);

    assume( p_GetExp(lp, w, R) == 2 );
    assume( p_GetExp(lp, x, R) == 0 );
    assume( p_GetExp(lp, y, R) == 0 );
    assume( p_GetExp(lp, z, R) == 0 );

    MATELEM(I, 1, ++gen) = p_Add_q(lp, p, R); // w2 - xz
  }

  {
    // -yz
    poly p = p_ISet(-1,R);

    p_SetExp(p, y, 1, R);
    p_SetExp(p, z, 1, R);
    p_Setm(p, R);

    assume( p_GetExp(p, y, R) == 1 );
    assume( p_GetExp(p, z, R) == 1 );
    assume( p_GetExp(p, w, R) == 0 );
    assume( p_GetExp(p, x, R) == 0 );

    // +wx
    poly lp = p_ISet(1,R);
    p_SetExp(lp, w, 1, R);
    p_SetExp(lp, x, 1, R);
    p_Setm(lp, R);

    assume( p_GetExp(lp, w, R) == 1 );
    assume( p_GetExp(lp, x, R) == 1 );
    assume( p_GetExp(lp, y, R) == 0 );
    assume( p_GetExp(lp, z, R) == 0 );

    MATELEM(I, 1, ++gen) = p_Add_q(lp, p, R); // wx - yz
  }


  {
    // -wy
    poly p = p_ISet(-1,R);

    p_SetExp(p, y, 1, R);
    p_SetExp(p, w, 1, R);
    p_Setm(p, R);

    assume( p_GetExp(p, y, R) == 1 );
    assume( p_GetExp(p, w, R) == 1 );
    assume( p_GetExp(p, z, R) == 0 );
    assume( p_GetExp(p, x, R) == 0 );

    // +x2
    poly lp = p_ISet(1,R);
    p_SetExp(lp, x, 2, R);
    p_Setm(lp, R);

    assume( p_GetExp(lp, w, R) == 0 );
    assume( p_GetExp(lp, x, R) == 2 );
    assume( p_GetExp(lp, y, R) == 0 );
    assume( p_GetExp(lp, z, R) == 0 );

    MATELEM(I, 1, ++gen) = p_Add_q(lp, p, R); // x2 - wy
  }


  {
    // -z2
    poly p = p_ISet(-1,R);

    p_SetExp(p, z, 2, R);
    p_Setm(p, R);

    assume( p_GetExp(p, y, R) == 0 );
    assume( p_GetExp(p, w, R) == 0 );
    assume( p_GetExp(p, z, R) == 2 );
    assume( p_GetExp(p, x, R) == 0 );

    // +xy
    poly lp = p_ISet(1,R);
    p_SetExp(lp, x, 1, R);
    p_SetExp(lp, y, 1, R);
    p_Setm(lp, R);

    assume( p_GetExp(lp, w, R) == 0 );
    assume( p_GetExp(lp, x, R) == 1 );
    assume( p_GetExp(lp, y, R) == 1 );
    assume( p_GetExp(lp, z, R) == 0 );

    MATELEM(I, 1, ++gen) = p_Add_q(lp, p, R); // xy - z2
  }


  {
    // -wz
    poly p = p_ISet(-1,R);

    p_SetExp(p, w, 1, R);
    p_SetExp(p, z, 1, R);
    p_Setm(p, R);

    assume( p_GetExp(p, y, R) == 0 );
    assume( p_GetExp(p, w, R) == 1 );
    assume( p_GetExp(p, z, R) == 1 );
    assume( p_GetExp(p, x, R) == 0 );

    // +y2
    poly lp = p_ISet(1,R);
    p_SetExp(lp, y, 2, R);
    p_Setm(lp, R);

    assume( p_GetExp(lp, w, R) == 0 );
    assume( p_GetExp(lp, x, R) == 0 );
    assume( p_GetExp(lp, y, R) == 2 );
    assume( p_GetExp(lp, z, R) == 0 );

    MATELEM(I, 1, ++gen) = p_Add_q(lp, p, R); // y2 - wz
  }
#ifdef PDEBUG
  PrintS("I: ");
  idShow(I, R, R, 0);
#endif


//  ideal kStd(ideal F, ideal Q, tHomog h, intvec ** mw,intvec *hilb=NULL,
//             int syzComp=0,int newIdeal=0, intvec *vw=NULL);
  // make R the default ring:
  rChangeCurrRing(R);

  {
    ideal G = kStd(I, currQuotient, testHomog, NULL);

#ifdef PDEBUG
    PrintS("GB: ");
    idShow(G, R, R, 0);
#endif

    idDelete( &G, R);
  }

  {
    intvec *weights = NULL;
    ideal SYZ = idSyzygies(I, testHomog, &weights);

#ifdef PDEBUG
    PrintS("SYZ: ");
    idShow(SYZ, R, R, 0);
#endif

    idDelete(&SYZ, R);
    if (weights!=NULL) { PrintS("weights: "); weights->show(); delete weights; }
  }


  {
    PrintS("\n**********************************\n");
    PrintS("lres: \n");
    int dummy;
    syStrategy r = syLaScala3(I,&dummy);

    intvec *b = syBettiOfComputation(r, FALSE);
    PrintS("non-min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    r =  syMinimize(r); // syzstr->references ++ ==> memory leak :(((

    b = syBettiOfComputation(r, TRUE);
    PrintS("min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    syKillComputation(r, R);
  }

  {
    PrintS("\n**********************************\n");
    PrintS("sres: \n");
    const int maxl = rVar(R)-1; // +2*(1);

    syStrategy r = sySchreyer(I, rVar(R));

    intvec *b = syBettiOfComputation(r, FALSE);
    PrintS("non-min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    r =  syMinimize(r); // syzstr->references ++ ==> memory leak :(((

    b = syBettiOfComputation(r, TRUE);
    PrintS("min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    syKillComputation(r, R);
  }



  {
    PrintS("\n**********************************\n");
    PrintS("nres: \n");
    intvec *weights=NULL;
//    const int maxl = rVar(R)-1 + 2*(1);
    syStrategy r = syResolution(I, rVar(R)-1, weights, FALSE/*iiOp==MRES_CMD*/);

    intvec *b = syBettiOfComputation(r, FALSE);
    PrintS("non-min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    r =  syMinimize(r); // syzstr->references ++ ==> memory leak :(((

    b = syBettiOfComputation(r, TRUE);
    PrintS("min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    syKillComputation(r, R);
  }


  {
    PrintS("\n**********************************\n");
    PrintS("mres: \n");
    intvec *weights=NULL;
//    const int maxl = rVar(R)-1 + 2*(1);
    syStrategy r = syResolution(I, rVar(R)+1, weights, TRUE/*iiOp==MRES_CMD*/);

    intvec *b = syBettiOfComputation(r, FALSE);
    PrintS("non-min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    r =  syMinimize(r); // syzstr->references ++ ==> memory leak :(((

    b = syBettiOfComputation(r, TRUE);
    PrintS("min. betti: \n");    b->show();    PrintLn();
    delete b;

    Print("length: %d\n", sySize(r));

    syPrint(r, "R");

    syKillComputation(r, R);
  }




  idDelete( &I, R);
  rDelete(R); // should cleanup every belonging polynomial, right!?

}



void TestSimpleRingArithmetcs()
{
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

  Print("p: "); p_Write0(p, R); Print(", deg(p): %d", p_Totaldegree(p, R)); assume( 1 == p_Totaldegree(p, R) );

  Print("; p*p : "); p_Write0(pp, R); Print("deg(pp): %d\n", p_Totaldegree(pp, R)); assume( 2 == p_Totaldegree(pp, R) );


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
}


int main( int, char *argv[] )
{
  assume( sizeof(long) == SIZEOF_LONG );

  if( sizeof(long) != SIZEOF_LONG )
  {
    WerrorS("Bad config.h: wrong size of long!");

    return(1);
  }


  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);

  PrintLn();
  { char* s = StringEndS(); PrintS(s); omFree(s); }

  TestGBEngine();
  TestSimpleRingArithmetcs();

  return 0;
}
