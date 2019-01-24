/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
 * ABSTRACT: fractal walk stuff
 *
*/
#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"
#include "misc/int64vec.h"

#include "polys/prCopy.h"
#include "polys/matpol.h"
#include "polys/monomials/ring.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/groebner_walk/walkMain.h"
#include "kernel/groebner_walk/walkSupport.h"
#include "kernel/GBEngine/kstd1.h"

#include <string.h>

///////////////////////////////////////////////////////////////////
//Groebner Walk and Fractal Walk
///////////////////////////////////////////////////////////////////
//v1.3 2004-11-15
///////////////////////////////////////////////////////////////////
//implemented by Henrik Strohmayer
///////////////////////////////////////////////////////////////////
//in C++:
///////////////////////////////////////////////////////////////////


VAR int overflow_error; //global variable

/*
overflow_error table
 1: Miv64DotProduct mult
 2: Miv64DotProduct add
 3: gett64 zaehler mult
 4: gett64 zaehler add
 5: gett64 nenner mult
 6: gett64 nenner add
 7: nextw64 mult a
 8: nextw64 mult b
 9: nextw64 add
10: getinveps64 mult
11: getinveps64 add
12: getTaun64 mult
13: getTaun64 add
*/

///////////////////////////////////////////////////////////////////
//fistWalkStep64
///////////////////////////////////////////////////////////////////
//Description: adapts currRing for the algorithm and moves G into
//it, calculating the appropriate GB if currw not is inside of
//the current groebner cone
///////////////////////////////////////////////////////////////////
//Assumes: that the option redSB is turned off
///////////////////////////////////////////////////////////////////
//Uses: init64, rCopy0AndAddA, rComplete, rChangeCurrRing, matIdLift,
//mpMult, idInterRed, idStd, idCopy, idrMoveR,currwOnBorder64
///////////////////////////////////////////////////////////////////

WalkState firstWalkStep64(ideal & G,int64vec* currw64, ring destRing){
  WalkState state=WalkOk;
  /* OLDRING **************************************************** */
  ideal nextG;

  if (currwOnBorder64(G,currw64))
  {
    ideal Gw=init64(G,currw64);
    ring oldRing=currRing;
  /* NEWRING **************************************************** */
    ring rnew=rCopy0AndAddA(destRing,currw64);
    rComplete(rnew);
    rChangeCurrRing(rnew);

    ideal newGw=idrMoveR(Gw, oldRing, rnew);


 //HIER GEANDERT
  matrix L=mpNew(1,1);
  idLiftStd(newGw,&L);

//       //turn off bucket representation of polynomials and on redSB
//     optionState=test;
//     //test|=Sy_bit(OPT_NOT_BUCKETS);
//     test|=Sy_bit(OPT_REDSB);

//     ideal newStdGw=idStd(newGw);

//    //turn on bucket representation of polynomials and off redSB
//    test=optionState;

//     matrix L=matIdLift(newGw,newStdGw);
//     idDelete(&newStdGw);

    idDelete(&newGw);

    nextG=idrMoveR(G,oldRing, rnew); idTest(nextG);

    matrix nextGmat=(matrix)nextG;

    matrix resMat=mp_Mult(nextGmat,L,rnew);
    idDelete((ideal *)&nextGmat);
    idDelete((ideal *)&L);

    nextG=(ideal)resMat;

    BITSET save1,save2;
    SI_SAVE_OPT(save1,save2);
    si_opt_1|=Sy_bit(OPT_REDSB);
    nextG = idInterRed(nextG);
    SI_RESTORE_OPT(save1,save2);
  }
  else
  {
    ring oldRing=currRing;
    ring rnew=rCopy0AndAddA(destRing,currw64);
    rComplete(rnew);
    rChangeCurrRing(rnew);
    nextG=idrMoveR(G,oldRing, rnew);
  }

  G=nextG;
  return(state);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//walkStep64
///////////////////////////////////////////////////////////////////
//Description: one step in the groebner walk
///////////////////////////////////////////////////////////////////
//Assumes: that the option redSB is turned off
///////////////////////////////////////////////////////////////////
//Uses: init, rCopyAndChangeA, matIdLift, mpMult,
//idInterRed, mStd, idCopy, idrMoveR
///////////////////////////////////////////////////////////////////

WalkState walkStep64(ideal & G,int64vec* currw64)
{
  WalkState state=WalkOk;

/* OLDRING ****************************************************** */
  ideal Gw=init64(G,currw64);

  ring oldRing=currRing;

/* NEWRING ****************************************************** */
  rCopyAndChangeA(currw64);

  ideal newGw=idrMoveR(Gw, oldRing,currRing);

  //HIER GEANDERT
  matrix L=mpNew(1,1);
  ideal newStdGw=idLiftStd(newGw,&L);

//what it looked like before idStd and idLift were replaced
//by idLiftStd
//   optionState=test;
//   test|=Sy_bit(OPT_REDSB);
//   test|=Sy_bit(OPT_NOT_BUCKETS);

//   //PrintS("  new initial forms:\n");
//   for (int ii=0; ii <IDELEMS(newGw); ii++) pCleardenom(newGw->m[ii]);
//   //idShow(newGw);
//   ideal newStdGw=idStd(newGw);
//     PrintS("  std for initial forms done\n");

//   test=optionState;

//  matrix L=matIdLift(newGw,newStdGw);

//  idDelete(&newStdGw);

  idDelete(&newGw);
    //PrintS("  lift for initial forms done\n");

  ideal nextG=idrMoveR(G,oldRing,currRing);
  rDelete(oldRing);

  matrix nextGmat=(matrix)nextG;

  matrix resMat=mp_Mult(nextGmat,L,currRing);
  idDelete((ideal *)&nextGmat);
  idDelete((ideal *)&L);
    //PrintS("  lift done\n");

  nextG=(ideal)resMat;

  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);
  si_opt_1|=Sy_bit(OPT_REDSB);
  nextG = idInterRed(nextG);
  SI_RESTORE_OPT(save1,save2);

  G=nextG;
  return(state);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//walk64
///////////////////////////////////////////////////////////////////
//Description: the main function of groebner walk, keeping
//everything together
///////////////////////////////////////////////////////////////////
//Uses: mStd, getnthrow, firstwalkStep64, nextt64,
//nextw64, walkStep64, changecurrRing
///////////////////////////////////////////////////////////////////

WalkState walk64(ideal I,int64vec* currw64,ring destRing,
int64vec* destVec64,ideal  & destIdeal,BOOLEAN sourceIsSB)
{
  //some initializations
  WalkState state=WalkOk;
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);

  si_opt_1|=Sy_bit(OPT_REDTAIL);
  overflow_error=FALSE;
  int step=0;
  ideal G=I;

  si_opt_1|=Sy_bit(OPT_REDSB);
  if(!sourceIsSB)
  {
    ideal GG=idStd(G);
    idDelete(&G); G=GG;
  }
  else
    G=idInterRed(G);
  SI_RESTORE_OPT(save1,save2);

  ideal nextG;
  state=firstWalkStep64(G,currw64,destRing);
  nextG=G;

  if(overflow_error)
  {
    state=WalkOverFlowError;
    return(state);
  }

  int64 nexttvec0,nexttvec1;
  //int64vec* nexttvec64=nextt64(nextG,currw64,destVec64);
  nextt64(nextG,currw64,destVec64,nexttvec0,nexttvec1);

  //while(0<t<=1) ( t=((*nexttvec64)[0])/((*nexttvec64)[1]) )
  //while( (*nexttvec64)[0]<=(*nexttvec64)[1] ) {
  while (nexttvec0<=nexttvec1 )
  {
    step=step+1;

    //int64vec *tt=nextw64(currw64,destVec64,nexttvec64);
    int64vec *tt=nextw64(currw64,destVec64,nexttvec0,nexttvec1);
    delete currw64; currw64=tt; tt=NULL;

    if (TEST_OPT_PROT)
    {
      PrintS("walk step:"); currw64->show(); PrintLn();
    }

    state=walkStep64(nextG,currw64);
    //uppdates nextG if all is OK

    if(overflow_error)
      return(WalkOverFlowError);

    //delete nexttvec64;
    //nexttvec64=nextt64(nextG,currw64,destVec64);
    nextt64(nextG,currw64,destVec64,nexttvec0,nexttvec1);

  }

  destIdeal=sortRedSB(nextG);
  return(state);
}

///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//FRACTAL WALK PART BEGINS HERE
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//firstFractalWalkStep64
///////////////////////////////////////////////////////////////////
//Description: called once before fractalRec64 is called, to set up
//the ring and move G into it, has two different strategies
///////////////////////////////////////////////////////////////////
//Uses: firstWalkStep64,currwOnBorder64,getTaun64,rCopy0AndAddA,
//getiv64,iv64Size,rComplete,rChangeCurrRing,idrMoveR,idStd,matIdLift
///////////////////////////////////////////////////////////////////


//unperturbedStartVectorStrategy IS NOW NOT ALLWAYS AS DEFAULT SET
//TO TRUE BUT IS INPUT FROM fractalWalk64
WalkState firstFractalWalkStep64(ideal & G,int64vec* & currw64,
intvec* currMat, ring destRing,
BOOLEAN unperturbedStartVectorStrategy){

    //This strategy Uses the ordinary walk for the first step
    if(unperturbedStartVectorStrategy){
      return(unperturbedFirstStep64(G,currw64,destRing));
    //here G is updated since its adress is given as argument
    }

    //This strategy makes sure that the start vector lies inside the start cone
    //thus no step is needed within the start cone.
    else{
      if(currwOnBorder64(G,currw64))
      {
        int64 dummy64;
        getTaun64(G,currMat,iv64Size(currw64),&currw64,dummy64);
        //currw64=getiv64(getTaun64(G,currMat,iv64Size(currw64)));
      }
      ring oldRing=currRing;
      ring newRing=rCopy0AndAddA(destRing,currw64);
      rComplete(newRing);
      rChangeCurrRing(newRing);
      G=idrMoveR(G,oldRing,newRing);
    }

  return(WalkOk);
}

///////////////////////////////////////////////////////////////////



//DIESE FUNKTION ERSETZT firstWalkStep FUR fractalWalk
///////////////////////////////////////////////////////////////////
//unperturbedFirstStep64
///////////////////////////////////////////////////////////////////
//Description: adapts currRing for the algorithm and moves G into
//it, calculating the appropriate GB if currw not is inside of
//the current groebner cone
///////////////////////////////////////////////////////////////////
//Assumes: that the option redSB is turned off
///////////////////////////////////////////////////////////////////
//Uses: init64, rCopy0AndAddA, rComplete, rChangeCurrRing, matIdLift,
//mpMult, idInterRed, idStd, idCopy, idrMoveR,currwOnBorder64
///////////////////////////////////////////////////////////////////

WalkState unperturbedFirstStep64(ideal & G,int64vec* currw64, ring destRing)
{
  WalkState state=WalkOk;
  /* OLDRING **************************************************** */
  ideal nextG;
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);

  if (currwOnBorder64(G,currw64))
  {
    ideal Gw=init64(G,currw64);
    ring oldRing=currRing;
  /* NEWRING **************************************************** */
    ring rnew=rCopy0AndAddA(destRing,currw64);
    rComplete(rnew);
    rChangeCurrRing(rnew);

    ideal newGw=idrMoveR(Gw, oldRing,rnew);

      //turn off bucket representation of polynomials and on redSB
    //si_opt_1|=Sy_bit(OPT_NOT_BUCKETS);
    si_opt_1|=Sy_bit(OPT_REDSB);

    ideal newStdGw=idStd(newGw);

    //turn on bucket representation of polynomials and off redSB
    SI_RESTORE_OPT(save1,save2);

    matrix L=matIdLift(newGw,newStdGw);
    idDelete(&newStdGw);
    idDelete(&newGw);

    nextG=idrMoveR(G,oldRing,rnew); idTest(nextG);

    matrix nextGmat=(matrix)nextG;

    matrix resMat=mp_Mult(nextGmat,L,rnew);
    idDelete((ideal *)&nextGmat);
    idDelete((ideal *)&L);

    nextG=(ideal)resMat;

    si_opt_1|=Sy_bit(OPT_REDSB);
    nextG = idInterRed(nextG);
    SI_RESTORE_OPT(save1,save2);
  }
  else
  {
    ring oldRing=currRing;
    ring rnew=rCopy0AndAddA(destRing,currw64);
    rComplete(rnew);
    rChangeCurrRing(rnew);
    nextG=idrMoveR(G,oldRing,rnew);
  }

  G=nextG;
  return(state);
}

///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
//fractalRec64
///////////////////////////////////////////////////////////////////
//Description: the recursion function used in fractalWalk64
//(see appropriate par of thesis for more details)
///////////////////////////////////////////////////////////////////
//Assumes: that the option redSB is turned off
///////////////////////////////////////////////////////////////////
//Uses: getTaun64,getint64,getiv64,invEpsOk64,nextt64,nextw64,
//init64,idCopy,noPolysOfMoreThanTwoTerms,rCopy0,rComplete,
//rChangeCurrRing,rSetWeightVec,idrMoveR,mpMult,idInterRed
///////////////////////////////////////////////////////////////////

WalkState fractalRec64(ideal  & G,int64vec* currw64, intvec* destMat,
                       int level,int step)
{

if (TEST_OPT_PROT)
{  PrintS("fractal walk, weights");currw64->show();PrintLn(); }
WalkState state=WalkOk;
BITSET save1,save2;
SI_SAVE_OPT(save1,save2);

//1
int64vec* w=(currw64);
int64vec* old_w=(currw64);
int64vec* sigma=(currw64);

//lists taunpair64=getTaun64(G,destMat,level);
//int64vec* w2=getiv64(taunpair64);
//int64 inveps64=getint64(taunpair64);
int64vec* w2;
int64 inveps64;
getTaun64(G,destMat,level,&w2,inveps64);

//2
while(1){

  //int64vec* tvec64=nextt64(G,w,w2);
  int64 tvec0,tvec1;
  nextt64(G,w,w2,tvec0,tvec1);

  if(overflow_error){
    return WalkOverFlowError;
  }

  //tvec[1]>tvec[2] iff t>1 or t ist undefined i.e.is (2,0)
  //if ((*tvec64)[0]>(*tvec64)[1])
  if (tvec0>tvec1)
  {
    if(invEpsOk64(G,destMat,level,inveps64)) {
      return(state);
  }
  else
  {
      //taunpair64=getTaun64(G,destMat,level);
      //w2=getiv64(taunpair64);
      //inveps64=getint64(taunpair64);
      delete w2;
      getTaun64(G,destMat,level,&w2,inveps64);

      //tvec64=nextt64(G,w,w2);
      nextt64(G,w,w2,tvec0,tvec1);

      if(overflow_error){
        return WalkOverFlowError;
      }

      //if((*tvec64)[0]>(*tvec64)[1])
      if(tvec0>tvec1)
      {
        return(state);
      }
    }
  }

  //i.e. t=1 and we have reached the target vector
  //if( ((*tvec64)[0]==(*tvec64)[1]) && (level!=iv64Size(w)) )
  if( (tvec0==tvec1) && (level!=iv64Size(w)) )
  {
    state=fractalRec64(G,old_w,destMat,level+1,step);
    return(state);
  }
  else
  {
    w=nextw64(w,w2,tvec0,tvec1);

//3
    ideal Gw=init64(G,w);  //finding initial ideal for new w

    ring oldRing=currRing;

    ideal GwCp=idCopy(Gw);
    ideal GCp=idCopy(G);

    ideal newGw;
    ideal newStdGw;
    ideal newG;

//4
    if ( level==iv64Size(w) || noPolysWithMoreThanTwoTerms(Gw) ){

//5
/*NEWRING**********************************************************/

      //this assumes order has A-vector as first vector
      ring newring=rCopy0(currRing);
      rComplete(newring);
      rSetWeightVec(newring,w->iv64GetVec());
      rChangeCurrRing(newring);
      //rSetWeightVec(newring,w->iv64GetVec());
      //rComplete(newring,1);

      newGw=idrMoveR(GwCp,oldRing,newring);

      si_opt_1|=Sy_bit(OPT_REDSB);
      newStdGw=idStd(newGw); //computes new reduced GB of Gw
      SI_RESTORE_OPT(save1,save2);
    }
    else
    {
//7
/*THE RING FROM THE RECURSION STEP BELOW***************************/
      //Here we can choose whether to call fractalrec with old_w,
      //which is the last w from the top level, or with sigma,
      //the original start vector. The impact on the algorithm is not obvious.

      state=fractalRec64(Gw,sigma,destMat,level+1,step);

      //The resulting GB is Gw since its adress is given as argument.
      ideal recG=Gw;
      ring temp=currRing;

/*NEWRING**********************************************************/

      //this assumes order has A-vector as first vector
      ring newring=rCopy0(currRing);
      rComplete(newring);
      rChangeCurrRing(newring);
      rSetWeightVec(currRing,w->iv64GetVec());
      rComplete(newring,1);

      newGw=idrMoveR(GwCp,oldRing,newring);

      newStdGw=idrMoveR(recG,temp,newring);
    }

//8
    //lifting comes either after level=nvars(ring), after Gw has
    //no poly with more than two terms or after
    //fractalRec64(Gw,level+1) has returned

    //si_opt_1|=Sy_bit(OPT_NOT_BUCKETS);
    matrix L=matIdLift(newGw,newStdGw);
    SI_RESTORE_OPT(save1,save2);

    newG=idrMoveR(GCp,oldRing,currRing);
    matrix MG=(matrix)newG;

    matrix resMat=mp_Mult(MG,L,currRing);
    idDelete((ideal *)&MG);
    idDelete((ideal *)&L);
    G=(ideal)resMat;

//9

    si_opt_1|=Sy_bit(OPT_REDSB);
    G=idInterRed(G);
    SI_RESTORE_OPT(save1,save2);

    old_w=iv64Copy(w);
    if(level==1) step=step+1;

  }

}
}


///////////////////////////////////////////////////////////////////


//ANOTHER INPUT-VARIABLE ADDED: unperturbedStartVectorStrategy
//THIS SHOULD BE SET IN walkProc.cc BY THE USER
///////////////////////////////////////////////////////////////////
//fractalWalk64
///////////////////////////////////////////////////////////////////
//Description:
///////////////////////////////////////////////////////////////////
//Uses:fractalRec64,firstFractalWalkStep64,idStd,idInterRed,
//int64VecToIntVec,getNthRow,sortRedSB
///////////////////////////////////////////////////////////////////

WalkState fractalWalk64(ideal sourceIdeal,ring destRing,
ideal & destIdeal,BOOLEAN sourceIsSB,
BOOLEAN unperturbedStartVectorStrategy)
{

  overflow_error=FALSE; //global
  WalkState state=WalkOk;
  BITSET save1,save2;
  SI_SAVE_OPT(save1,save2);

  si_opt_1|= (Sy_bit(OPT_REDTAIL)|Sy_bit(OPT_REDSB));
  ideal G;

  if(!sourceIsSB)
  {
    G=idStd(sourceIdeal);
  }

  else
  {
    G=idInterRed(idCopy(sourceIdeal));
  }

  SI_RESTORE_OPT(save1,save2); //switches REDSB off

  //matrices for the orders of the rings
  intvec *destMat=int64VecToIntVec(rGetGlobalOrderMatrix(destRing));
  intvec *currMat=int64VecToIntVec(rGetGlobalOrderMatrix(currRing));

  int64vec* currw64=getNthRow64(currMat,1); //start vector

  state=firstFractalWalkStep64(G,currw64,currMat,destRing,
                               unperturbedStartVectorStrategy);
  delete currMat;

  state=fractalRec64(G,currw64,destMat,1,1);
  if(state==WalkOk)
    destIdeal=G;

  if(overflow_error)
    state=WalkOverFlowError;

  delete currw64;
  delete destMat;
  return state;
}


/////////////////////////////////////////////////////////////
