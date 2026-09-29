#include "kernel/mod2.h"

#include "resources/feFopen.h"
#include "resources/feResource.h"

#include "factory/factory.h" // :(

#include "misc/intvec.h"
#include "misc/int64vec.h"
#include "misc/mylimits.h"
#include "misc/options.h"

#include "reporter/reporter.h"

#include "coeffs/si_gmp.h"
#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

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


#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hilb.h"
#include "kernel/GBEngine/syz.h"
#include "kernel/GBEngine/khstd.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kstdfac.h"
#include "kernel/GBEngine/units.h"
#include "kernel/GBEngine/ratgring.h"
#include "kernel/GBEngine/shiftgb.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/nc.h"
#include "kernel/GBEngine/ratgring.h"
#include "kernel/GBEngine/ringgb.h"
#include "kernel/GBEngine/shiftgb.h"
#include "kernel/GBEngine/syz.h"
#include "kernel/GBEngine/tgbgauss.h"
#include "kernel/GBEngine/tgb.h"
#include "kernel/GBEngine/units.h"
#include "kernel/GBEngine/janet.h"
#include "Singular/ipid.h"

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
  rRingOrder_t *order = (rRingOrder_t *) omAlloc0(D* sizeof(rRingOrder_t));
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
    ideal G = kStd(I, currRing->qideal, testHomog, NULL);

#ifdef PDEBUG
    PrintS("GB: ");
    idShow(G, R, R, 0);
#endif

    id_Delete( &G, R);
  }

  {
    intvec *weights = NULL;
    ideal SYZ = idSyzygies(I, testHomog, &weights);

#ifdef PDEBUG
    PrintS("SYZ: ");
    idShow(SYZ, R, R, 0);
#endif

    id_Delete(&SYZ, R);
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




  id_Delete( &I, R);
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

  PrintS("p: "); p_Write0(p, R); Print(", deg(p): %ld", p_Totaldegree(p, R)); assume( 1 == p_Totaldegree(p, R) );

  PrintS("; p*p : "); p_Write0(pp, R); Print("deg(pp): %ld\n", p_Totaldegree(pp, R)); assume( 2 == p_Totaldegree(pp, R) );


  p_Delete(&p, R);

  assume( p_GetExp(pp,1, R) == 2 );

  p_Delete(&pp, R);


//  rDelete(R);

  // make R the default ring:
  rChangeCurrRing(R);

  // create the polynomial 1
  poly p1=pISet(1);

  // create the polynomial 2*x^3*z^2
  poly p2=p_ISet(2,R);
  pSetExp(p2,1,3);
  pSetExp(p2,3,2);
  pSetm(p2);

  // print p1 + p2
  PrintS("p1: "); pWrite0(p1);
  PrintS(" + p2: "); pWrite0(p2);
  PrintS("  ---- >>>> ");

  // compute p1+p2
  p1=p_Add_q(p1,p2,R); p2=NULL;
  pWrite(p1);

  // clean up:
//  pDelete(&p1);

  rDelete(R); // should cleanup every belonging polynomial, right!?
}

static poly hilb64Binomial(int aVar, int aExp, int bVar, int bExp,
                           const ring R)
{
  poly a=p_ISet(1,R);
  p_SetExp(a,aVar,aExp,R);
  p_Setm(a,R);
  poly b=p_ISet(-1,R);
  p_SetExp(b,bVar,bExp,R);
  p_Setm(b,R);
  return p_Add_q(a,b,R);
}

void TestHilbert64()
{
  char** names=(char**)omAlloc(3*sizeof(char*));
  names[0]=omStrDup("x");
  names[1]=omStrDup("y");
  names[2]=omStrDup("z");
  rRingOrder_t* order=(rRingOrder_t*)omAlloc(2*sizeof(rRingOrder_t));
  int* block0=(int*)omAlloc0(2*sizeof(int));
  int* block1=(int*)omAlloc0(2*sizeof(int));
  order[0]=ringorder_dp;
  order[1]=ringorder_no;
  block0[0]=1;
  block1[0]=3;
  coeffs cf=nInitChar(n_Zp,(void*)(long)32003);
  ring R=rDefault(cf,3,names,2,order,block0,block1,NULL,
                  (unsigned long)LONG_MAX);
  rChangeCurrRing(R);

  ideal I=idInit(2,1);
  I->m[0]=hilb64Binomial(1,2,2,1,R);
  I->m[1]=hilb64Binomial(1,1,3,1,R);
  int64vec weights64(3);
  weights64[0]=INT64_C(3000000000);
  weights64[1]=INT64_C(6000000000);
  weights64[2]=INT64_C(3000000000);
  assume(id_HomIdealW64(I,NULL,&weights64,R));

  ideal known=kStd_internal64(I,NULL,isHomog,NULL,NULL,0,0,&weights64,NULL);
  ring Qt=hHilbertSeriesRing();
  poly hs=hFirstSeries0p64(known,NULL,&weights64,R,Qt);
  assume((hs!=NULL) && (p_GetExp(hs,1,Qt)>INT_MAX));
  intvec* moduleWeights=NULL;
  ideal G64=kStdPoly64(I,NULL,isHomog,&moduleWeights,hs,Qt,0,
                       0,0,&weights64,NULL);
  idSkipZeroes(G64);
  assume(IDELEMS(G64)==2);

  if (coeffs_BIGINT==NULL) coeffs_BIGINT=nInitChar(n_Z,NULL);
  intvec weights32(3);
  weights32[0]=1;
  weights32[1]=2;
  weights32[2]=1;
  bigintmat* hdense=hFirstSeries0b(known,NULL,&weights32,NULL,R,coeffs_BIGINT);
  ideal G32=kStd2(I,NULL,isHomog,&moduleWeights,hdense,0,0,&weights32,NULL);
  idSkipZeroes(G32);
  assume(IDELEMS(G32)==IDELEMS(G64));

  int64vec autoWeights(3);
  autoWeights[0]=INT64_C(3000000000);
  autoWeights[1]=INT64_C(6000000000);
  autoWeights[2]=1;
  ring RA=rCopy0AndAddA(R,&autoWeights,TRUE,TRUE);
  rComplete(RA);
  rChangeCurrRing(RA);
  ideal IA=idInit(2,1);
  IA->m[0]=hilb64Binomial(1,2,2,1,RA);
  IA->m[1]=p_ISet(1,RA);
  p_SetExp(IA->m[1],1,1,RA);
  p_SetExp(IA->m[1],2,1,RA);
  p_Setm(IA->m[1],RA);
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);
  si_opt_2|=Sy_bit(V_STDHILB);
  ideal Gauto=kTryHilbstd(IA,NULL);
  SI_RESTORE_OPT(save1,save2);
  assume(Gauto!=NULL);
  idSkipZeroes(Gauto);
  assume(IDELEMS(Gauto)==3);

  // Exercise the nonhomogeneous route as well: homogenizing the constant in
  // x^2-y+1 requires an exponent of 6,000,000,000 in the added variable.
  ideal Inon=idInit(2,1);
  Inon->m[0]=p_Add_q(hilb64Binomial(1,2,2,1,RA),p_ISet(1,RA),RA);
  Inon->m[1]=p_ISet(1,RA);
  p_SetExp(Inon->m[1],1,1,RA);
  p_SetExp(Inon->m[1],2,1,RA);
  p_Setm(Inon->m[1],RA);
  ideal Gplain=kStd_internal64(Inon,NULL,testHomog,NULL,NULL,0,0,NULL,NULL);
  SI_SAVE_OPT(save1,save2);
  si_opt_2|=Sy_bit(V_STDHILB);
  ideal Gnon=kTryHilbstd(Inon,NULL);
  SI_RESTORE_OPT(save1,save2);
  assume((Gplain!=NULL) && (Gnon!=NULL));
  idSkipZeroes(Gplain);
  idSkipZeroes(Gnon);
  ideal remainder=kNF(Gnon,NULL,Gplain,0,0);
  assume(idIs0(remainder));
  id_Delete(&remainder,RA);
  remainder=kNF(Gplain,NULL,Gnon,0,0);
  assume(idIs0(remainder));
  id_Delete(&remainder,RA);

  id_Delete(&Gnon,RA);
  id_Delete(&Gplain,RA);
  id_Delete(&Inon,RA);
  id_Delete(&Gauto,RA);
  id_Delete(&IA,RA);
  rChangeCurrRing(R);
  rDelete(RA);
  id_Delete(&G64,R);
  id_Delete(&G32,R);
  id_Delete(&known,R);
  id_Delete(&I,R);
  p_Delete(&hs,Qt);
  delete hdense;
  if (moduleWeights!=NULL) delete moduleWeights;
  rDelete(R);
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
  TestHilbert64();

  return 0;
}
