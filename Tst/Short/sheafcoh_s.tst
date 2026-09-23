LIB "tst.lib";
tst_init();

 LIB "sheafcoh.lib";

// Kohomologie der Strukturgarbe von P^5:
//----------------------------------------
ring r=0,x(1..6),dp;
module M=0;
def A=sheafCoh(M,-10,5);
displayCohom(A,-10,5,5);
A=sheafCoh(M,-10,5,"sres");
displayCohom(A,-10,5,5);
def B=sheafCohBGG(M,-10,5);
displayCohom(A,-10,5,5);
displayCohom(B,-10,5,5);
// Kohomologie der getwisteten Strukturgarbe von P^5:
//----------------------------------------------------
attrib(M,"isHomog",intvec(-2));
A=sheafCoh(M,-10,3);
displayCohom(A,-10,3,5);
A=sheafCoh(M,-10,3,"sres");
displayCohom(A,-10,3,5);
B=sheafCohBGG(M,-10,3);
displayCohom(B,-10,3,5);
// Kohomologie direkter Summen getwisteter Strukturgarben von P^5:
//----------------------------------------------------------------
matrix MM[3][1];
M=MM;
attrib(M,"isHomog",intvec(-1,0,2));
A=sheafCoh(M,-9,5);
displayCohom(A,-9,5,5);
A=sheafCoh(M,-9,4,"sres");
displayCohom(A,-9,4,5);
B=sheafCohBGG(M,-8,3);
displayCohom(B,-8,3,5);
// Kohomologie von O(-2)+O(-1)+(O/x(1))(1) = O_5(-2)+o_5(-1)+O_4(1):
//-------------------------------------------------------------------
MM=0,x(1),0;
M=MM;
attrib(M,"isHomog",intvec(-2,1,-1));
A=sheafCoh(M,-8,4);
displayCohom(A,-8,4,5);
A=sheafCoh(M,-8,4,"sres");
displayCohom(A,-8,4,5);
B=sheafCohBGG(M,-8,3);
displayCohom(B,-8,3,5);
kill r;

// Kohomologie der Idealgarbe der Veronese Flaeche in $\P^3$:
//------------------------------------------------------------
 ring S = 32003, x(0..4), dp;
 module MI=maxideal(1);
 attrib(MI,"isHomog",intvec(-1));
 resolution kos = nres(MI,0);
 print(betti(kos),"betti");
 matrix alpha0 = random(32002,10,3);
 module pres = module(alpha0)+kos[3];
 attrib(pres,"isHomog",intvec(1,1,1,1,1,1,1,1,1,1));
 resolution fcokernel = mres(pres,0);
 print(betti(fcokernel),"betti");
 module dir = transpose(pres);
 attrib(dir,"isHomog",intvec(-1,-1,-1,-2,-2,-2,
                             -2,-2,-2,-2,-2,-2,-2));
 resolution fdir = mres(dir,2);
 print(betti(fdir),"betti");
 ideal I = groebner(flatten(fdir[2]));
 resolution FI = mres(I,0);
 print(betti(FI),"betti");
 module F=FI[2];
 A=sheafCoh(F,-4,4);
 displayCohom(A,-4,4,4);
 A=sheafCoh(F,-4,4,"sres");
 displayCohom(A,-4,4,4);
 B=sheafCohBGG(F,-4,2);
 displayCohom(B,-4,2,4);

 dimH(3,F,-4);
 dimH(1,F,1);

 A=sheafCoh(F,-1,1);
 displayCohom(A,-1,1,4);
 kill S;

// --------------------------------------------------
// Test of truncate:
   ring R=0,(x,y,z),dp;
   module M=x2,y3,z4;
   homog(M);
   // compute presentation matrix for truncated module (R/<x2,y3,z4>)_(>=2)
   module M2=truncate(M,2);
   print(M2);
   dimGradedPart(M2,1);
   dimGradedPart(M,1);
   dimGradedPart(M2,2);
   // this should coincide with:
   dimGradedPart(M,2);
   dimGradedPart(M,3);
   // shift grading by -1:
   intvec v=-1;
   attrib(M,"isHomog",v);
   M2=truncate(M,2);
   print(M2);
   dimGradedPart(M2,2);

// Representatives for H^0 from the regular multiplication tail:
//---------------------------------------------------------------
   kill R;
   ring R=0,(x,y,z),dp;
   module O=0;
   attrib(O,"isHomog",intvec(0));
   list SB=sheafSectionBasis(O,2,2);
   size(SB[1]);
   SB[1];
   SB[2];
   SB[3];

// Mixed shifts of a free module must contribute their maximum to regularity;
// this is also a regression for the projective-dimension BGG window.
   matrix FreePresentation[3][1];
   module MixedFree=FreePresentation;
   attrib(MixedFree,"isHomog",intvec(-2,0,3));
   CM_regularity(MixedFree);
   list MixedFreeSections=sheafSectionBasis(MixedFree,0);
   size(MixedFreeSections[1]);
   SectionSpace EmptySections=lineBundleSectionBasis(O,-1,ideal(0));
   typeof(EmptySections);
   size(EmptySections);
   // Direct irrelevant saturation gives the same typed interface, including
   // the negative-degree empty space:
   SectionSpace DirectO1=lineBundleSectionBasisDirect(O,1,ideal(0));
   size(DirectO1);
   DirectO1.basis;
   DirectO1.denom;
   DirectO1.tailDegree;
   SectionSpace DirectEmpty=lineBundleSectionBasisDirect(O,-1,ideal(0));
   size(DirectEmpty);
   DirectEmpty.denom;
   DirectEmpty.tailDegree;
   SB=sheafSectionBasis(O,0,2,x2,1);
   SB[1];
   SB[2];
   SB[3];

// Finite-length torsion at the regularity boundary must disappear:
//-----------------------------------------------------------------
   module T=x*gen(2),y*gen(2),z*gen(2);
   attrib(T,"isHomog",intvec(0,0));
   SB=sheafSectionBasis(T,0);
   size(SB[1]);
   SB[1];
   SB[3];
   // The unique sheaf section uses only the first presentation generator.
   // Its raw module must nevertheless remember both ambient rows.
   attrib(SB[1],"rank");
   nrows(matrix(SB[1]));
   list TBGG=sheafSectionBasisBGG(T,0);
   size(TBGG[1]);
   attrib(TBGG[1],"rank");
   nrows(matrix(TBGG[1]));
   list TEmpty=sheafSectionBasis(T,-1);
   size(TEmpty[1]);
   attrib(TEmpty[1],"rank");
   nrows(matrix(TEmpty[1]));

// Verification is opt-in and performs one check without changing the tail:
//-------------------------------------------------------------------------
   poly unitDenominator=1;
   // The default and v=0 return the deliberately unstable candidate without
   // causing sectionBasisBGGDimension to run.
   SB=sheafSectionBasis(T,0,0,unitDenominator);
   size(SB[1]);
   SB=sheafSectionBasis(T,0,0,unitDenominator,0);
   size(SB[1]);
   SB=sheafSectionBasis(T,0,0,unitDenominator,1);

// Genuine BGG/Tate representatives, rather than a dimension check:
//------------------------------------------------------------------
   kill R;
   ring Rbgg=0,(x,y,z),dp;
   proc compareBGGAndMultTable(module N,int twist,int tail,poly evalForm,
                               int expectedDimension)
   {
     list multTable=sheafSectionBasis(N,twist,tail,evalForm,0);
     list bgg=sheafSectionBasisBGG(N,twist,tail,evalForm);
     return((size(multTable[1])==expectedDimension) &&
            (size(bgg[1])==expectedDimension) &&
            (size(NF(multTable[1],std(bgg[1])))==0) &&
            (size(NF(bgg[1],std(multTable[1])))==0));
   }
   // mres changes the source basis for this quotient; the BGG comparison
   // matrix must recover the original degree-tail coordinates.
   module BGGQuotient=x*gen(1)+y*gen(2)+z*gen(3);
   attrib(BGGQuotient,"isHomog",intvec(0,0,0));
   compareBGGAndMultTable(BGGQuotient,0,2,z2,3);
   // Several Tate cohomology strands occur, but source weight -d selects
   // exactly H^0. The four sections come from the shifts 0 and -1.
   matrix BGGFreeMatrix[3][1];
   module BGGMixedFree=BGGFreeMatrix;
   attrib(BGGMixedFree,"isHomog",intvec(0,2,-1));
   compareBGGAndMultTable(BGGMixedFree,0,3,z3,4);

   // Typed rank-one, line-bundle, cached-frame, and one-call linear-frame
   // entry points all expose the same genuine BGG section algorithm.
   module BGGPointIdeal=y*gen(1)-x*gen(2);
   attrib(BGGPointIdeal,"isHomog",intvec(1,1));
   RankOneSheaf BGGPointSheaf=rankOneSheafLinear(BGGPointIdeal,ideal(0));
   SectionSpace BGGPointSections=rankOneSheafSectionBasisBGG(
                                  BGGPointSheaf,1,2,z);
   SectionSpace BGGPointLines=lineBundleSectionBasisBGG(
                               BGGPointSheaf,1,2,z);
   SectionSpace BGGPointOneCall=rankOneSheafSectionBasisBGGLinear(
                                 BGGPointIdeal,1,ideal(0),2,z);
   SectionSpace BGGPointLineOneCall=lineBundleSectionBasisBGGLinear(
                                     BGGPointIdeal,1,ideal(0),2,z);
   size(BGGPointSections);
   size(BGGPointLines)==size(BGGPointSections);
   size(BGGPointOneCall)==size(BGGPointSections);
   size(BGGPointLineOneCall)==size(BGGPointSections);
   SectionSpace BGGPointMultTable=rankOneSheafSectionBasis(
                                   BGGPointSheaf,1,2,z,0);
   ideal BGGCoordinates=BGGPointSections.basis*BGGPointMultTable.denom;
   ideal multTableCoordinates=BGGPointMultTable.basis*BGGPointSections.denom;
   size(NF(BGGCoordinates,std(multTableCoordinates)));
   size(NF(multTableCoordinates,std(BGGCoordinates)));

// Characteristic two may omit resolution weight attributes; manual weight
// propagation and linear-strand extraction must still produce representatives.
//-----------------------------------------------------------------------------
   ring Rbgg2=2,(x,y,z),dp;
   module BGGCharTwo=x*gen(1)+y*gen(2)+z*gen(3);
   attrib(BGGCharTwo,"isHomog",intvec(0,0,0));
   compareBGGAndMultTable(BGGCharTwo,0,2,z2,3);

// Qrings and P^0 use the same representative construction without a special
// Betti-table window.
//--------------------------------------------------------------------------
   ring RbggQ=0,(u,w),dp;
   qring Qbgg=std(u*w);
   module BGGQStructure=0;
   attrib(BGGQStructure,"isHomog",intvec(0));
   list BGGQSections=sheafSectionBasisBGG(BGGQStructure,0);
   size(BGGQSections[1]);
   ring RbggP0=0,t,dp;
   module BGGP0Structure=0;
   attrib(BGGP0Structure,"isHomog",intvec(0));
   list BGGP0Sections=sheafSectionBasisBGG(BGGP0Structure,0,2,t2);
   size(BGGP0Sections[1]);

// Scalarization of a presented rank-one module:
//------------------------------------------------
   kill RbggP0;
   ring R=0,(x,y,z),dp;
   list SB;
   module P=-x*gen(1)+gen(2);
   attrib(P,"isHomog",intvec(-1,0));
   list ST=trivializedSectionBasis(P,0,ideal(1,x),1,ideal(0));
   ST[1];
   ST[2];
   ST[3];
   list DirectST=trivializedSectionBasisDirect(P,0,ideal(1,x),1,ideal(0));
   DirectST[1];
   DirectST[2];
   DirectST[3];
   // The same frame may be supplied with a nonconstant common denominator.
   list DirectScaled=trivializedSectionBasisDirect(P,0,
                                                   ideal(z,x*z),z,ideal(0));
   DirectScaled[1];
   DirectScaled[2];
   DirectScaled[3];
   SectionSpace DirectP=rankOneSheafSectionBasisDirect(P,0,ideal(0));
   size(DirectP);
   DirectP.basis;
   DirectP.denom;
   sectionNumerator(DirectP[1]);
   sectionDenominator(DirectP[1]);
   SectionSpace LB=lineBundleSectionBasis(P,0,ideal(0),0,1,1);
   typeof(LB);
   size(LB);
   LB.basis;
   LB.denom;
   LB.trivializationImages;
   LB.trivializationDenom;
   LB.twist;
   LB.trivializationShift;
   LB.tailDegree;
   Section firstSection=LB[1];
   typeof(firstSection);
   sectionNumerator(firstSection);
   sectionDenominator(firstSection);
   list LBasis=sectionBasis(LB);
   size(LBasis);
   rationalMapFromSections(LB);
   rationalMapFromSections(list(LB[1],LB[3]));
   rationalMapFromSections(ST);
   rationalMapFromSections(list(ST[1],1,ST[3]));
   rationalMapFromSections(ST[1]);
   rationalMapFromSections(list(y,z,x));
   ideal SlotMap=rationalMapFromSections(list(0,x,0));
   ncols(matrix(SlotMap));
   SlotMap;

// The rational functional can also be chosen from the graded dual:
//-----------------------------------------------------------------
   list RT=rankOneTrivialization(P,ideal(0));
   RT[1];
   RT[2];
   RT[3];
   SectionSpace RS=rankOneSheafSectionBasis(P,0,ideal(0),0,1,1);
   RS.basis;
   RS.denom;
   RS.tailDegree;

// Sections from different frames or graded sheaves cannot be mixed:
//-----------------------------------------------------------------
   ideal Iframes=x*y;
   module FrameA=y*gen(1),x*gen(2);
   attrib(FrameA,"isHomog",intvec(0,0));
   SectionSpace SpaceA=rankOneSheafSectionBasis(FrameA,0,Iframes,2,z2,0);
   // This is the same relation submodule, but has a different rational frame.
   module FrameB=y*gen(1),y*gen(1)+x*gen(2);
   attrib(FrameB,"isHomog",intvec(0,0));
   SectionSpace SpaceB=rankOneSheafSectionBasis(FrameB,0,Iframes,2,z2,0);
   rationalMapFromSections(list(SpaceA[1],SpaceB[2]));
   // This has the same relation submodule and frame degree, but another grading.
   module GradedB=y*gen(1),x*gen(2);
   attrib(GradedB,"isHomog",intvec(1,0));
   SectionSpace SpaceC=rankOneSheafSectionBasis(GradedB,0,Iframes,2,z2,0);
   rationalMapFromSections(list(SpaceA[2],SpaceC[1]));

// Dual generators which vanish on the support must not raise the shift:
//-----------------------------------------------------------------------
   ideal IXredundant=x;
   module Redundant=x*gen(1),gen(2);
   attrib(Redundant,"isHomog",intvec(0,-5));
   RT=rankOneTrivialization(Redundant,IXredundant);
   RT[1];
   RT[2];
   RT[3];

// Support is a sheaf condition, so irrelevant torsion is saturated away:
//-----------------------------------------------------------------------
   module SaturatedSupport=x2*gen(1),x*y*gen(1),x*z*gen(1);
   attrib(SaturatedSupport,"isHomog",intvec(0));
   RT=rankOneTrivialization(SaturatedSupport,ideal(x));
   RT[1];
   RT[2];
   RT[3];

// A presentation generator may have zero image under a trivialization:
//---------------------------------------------------------------------
   module Z=gen(2);
   attrib(Z,"isHomog",intvec(0,0));
   ST=trivializedSectionBasis(Z,0,ideal(1,0),1,ideal(0),0,1,1);
   size(ST[1]);
   ST[1];
   ST[2];
   ST[3];
   // The automatic checked-frame path scalarizes in bulk and must restore a
   // trailing zero row dropped from the vector basis.
   SectionSpace TrustedZ=rankOneSheafSectionBasis(Z,0,ideal(0),0,1,0);
   size(TrustedZ);
   TrustedZ.basis;
   TrustedZ.denom;
   TrustedZ.trivializationImages;

// The regularity boundary itself need not yet give the stable Hom tail:
//-----------------------------------------------------------------------
   module L=(x+y)*gen(1);
   attrib(L,"isHomog",intvec(0));
   SB=sheafSectionBasis(L,-1);
   size(SB[1]);
   SB[3];

// A syzygy presentation of m^2 sheafifies to the structure sheaf:
//-----------------------------------------------------------------
   ideal J=maxideal(2);
   module MJ=syz(J);
   attrib(MJ,"isHomog",intvec(2,2,2,2,2,2));
   ST=trivializedSectionBasis(MJ,0,J,1,ideal(0));
   size(ST[1]);
   ST[1];
   ST[2];
   ST[3];

// Reducible support needs a denominator regular on every component:
//------------------------------------------------------------------
   kill R;
   ring Rtail=0,(u,w),dp;
   module XY=u*w*gen(1);
   attrib(XY,"isHomog",intvec(0));
   list SBTail=sheafSectionBasis(XY,0);
   size(SBTail[1]);
   SBTail[1];
   SBTail[2];
   SBTail[3];

// The same computation accepts a module directly over a qring:
//--------------------------------------------------------------
   ideal I=u*w;
   // Neither dual generator alone covers both components; their sum does.
   module Split=w*gen(1),u*gen(2);
   attrib(Split,"isHomog",intvec(0,0));
   list RTSplit=rankOneTrivialization(Split,I);
   RTSplit[1];
   RTSplit[2];
   RTSplit[3];
   SectionSpace STSplit=rankOneSheafSectionBasis(Split,0,I);
   STSplit.basis;
   STSplit.denom;
   STSplit.tailDegree;
   rationalMapFromSections(STSplit);
   // Direct saturation must not replace this non-locally-free sheaf by a
   // purification or reflexive hull:
   SectionSpace DirectSplit=rankOneSheafSectionBasisDirect(Split,0,I);
   size(DirectSplit);
   DirectSplit.basis;
   DirectSplit.denom;
   rationalMapFromSections(DirectSplit);
   qring Qtail=std(I);
   module SplitQ=w*gen(1),u*gen(2);
   attrib(SplitQ,"isHomog",intvec(0,0));
   SectionSpace DirectSplitQ=rankOneSheafSectionBasisDirect(SplitQ,0,
                                                            ideal(0));
   size(DirectSplitQ);
   DirectSplitQ.basis;
   DirectSplitQ.denom;
   module OQ=0;
   attrib(OQ,"isHomog",intvec(0));
   list SBQ=sheafSectionBasis(OQ,0);
   size(SBQ[1]);
   SBQ[1];
   SBQ[2];
   SBQ[3];
   SectionSpace LQ=lineBundleSectionBasis(OQ,0,ideal(0));
   size(LQ);
   LQ.basis;
   LQ.denom;
   rationalMapFromSections(LQ);

// Qring lifting also preserves twists and stabilization:
//--------------------------------------------------------
   ring Rlinear=0,(x,y,z),dp;
   ideal K=x+y;
   qring Qlinear=std(K);
   module OH=0;
   attrib(OH,"isHomog",intvec(0));
   SectionSpace DirectQLine=lineBundleSectionBasisDirect(OH,1,ideal(0));
   size(DirectQLine);
   DirectQLine.basis;
   DirectQLine.denom;
   DirectQLine.tailDegree;
   list SBLinear=sheafSectionBasis(OH,-1);
   size(SBLinear[1]);
   SBLinear[3];

// Default-off verification and explicit opt-in survive qring lifting:
//-------------------------------------------------------------------
   ring Rverify=0,(x,y,z),dp;
   qring Qverify=std(z);
   module Tverify=x*gen(2),y*gen(2);
   attrib(Tverify,"isHomog",intvec(0,0));
   poly qUnitDenominator=1;
   list SBVerify=sheafSectionBasis(Tverify,0,0,qUnitDenominator,0);
   size(SBVerify[1]);
   attrib(SBVerify[1],"rank");
   nrows(matrix(SBVerify[1]));
   SBVerify=sheafSectionBasis(Tverify,0,0,qUnitDenominator);
   size(SBVerify[1]);
   SBVerify=sheafSectionBasis(Tverify,0,0,qUnitDenominator,1);

// On P^0, constructing T^d needs one output column to the right:
//----------------------------------------------------------------
   ring RP0=0,t,dp;
   module OP0=0;
   attrib(OP0,"isHomog",intvec(0));
   list P0Sections=sheafSectionBasis(OP0,4);
   size(P0Sections[1]);
   module ZP0=t;
   attrib(ZP0,"isHomog",intvec(0));
   list P0ZeroSections=sheafSectionBasis(ZP0,1);
   size(P0ZeroSections[1]);

// A supplied frame is validated once and then reused without rediscovery:
//-----------------------------------------------------------------------
   kill RP0;
   ring Rcached=2,(x,y,z),dp;
   proc cachedFrameEntriesEqual(ideal A,ideal B)
   {
     if (ncols(matrix(A))!=ncols(matrix(B))) { return(0); }
     int cachedFrameIndex;
     for (cachedFrameIndex=1;
          cachedFrameIndex<=ncols(matrix(A)); cachedFrameIndex++)
     {
       if (A[cachedFrameIndex]!=B[cachedFrameIndex]) { return(0); }
     }
     return(1);
   }
   poly cachedA=x*y*z;
   poly cachedB=(x+y)*(x+z)*(x+y+z);
   ideal cachedSupport=cachedA*cachedB;
   module cachedPresentation=cachedA*gen(1),cachedB*gen(2);
   attrib(cachedPresentation,"isHomog",intvec(1,0));
   ideal cachedImages=(y+z)*cachedB,cachedA;
   // Automatic frame recovery cannot find the regular factor y+z here:
   // over F_2 its finite search only encounters the other six linear forms,
   // all zero divisors on cachedSupport. Hence these calls also regress that
   // an explicitly cached frame is never rediscovered internally.
   RankOneSheaf cachedSheaf=rankOneSheaf(cachedPresentation,cachedSupport,
                                         cachedImages,1);
   SectionSpace cachedMinus=rankOneSheafSectionBasis(cachedSheaf,-1);
   SectionSpace cachedZero=rankOneSheafSectionBasis(cachedSheaf,0);
   SectionSpace cachedOne=rankOneSheafSectionBasis(cachedSheaf,1);
   size(cachedMinus);
   size(cachedZero);
   size(cachedOne);
   cachedFrameEntriesEqual(cachedMinus.trivializationImages,cachedImages);
   cachedFrameEntriesEqual(cachedZero.trivializationImages,cachedImages);
   cachedFrameEntriesEqual(cachedOne.trivializationImages,cachedImages);
   cachedMinus.trivializationDenom==1;
   cachedZero.trivializationDenom==1;
   cachedOne.trivializationDenom==1;
   cachedZero.trivializationShift;
   SectionSpace cachedMultTable=rankOneSheafSectionBasisMultTable(
                                  cachedSheaf,0);
   size(cachedMultTable);
   ideal cachedMultTableCoordinates=cachedMultTable.basis*cachedZero.denom;
   ideal cachedDefaultCoordinates=cachedZero.basis*cachedMultTable.denom;
   size(NF(cachedMultTableCoordinates,
           std(cachedDefaultCoordinates+cachedSupport)));
   size(NF(cachedDefaultCoordinates,
           std(cachedMultTableCoordinates+cachedSupport)));
   SectionSpace cachedDirect=rankOneSheafSectionBasisDirect(cachedSheaf,0);
   size(cachedDirect);
   cachedFrameEntriesEqual(cachedDirect.trivializationImages,cachedImages);
   // The old explicit-frame API remains equivalent to the cached one.
   list cachedOld=trivializedSectionBasis(cachedPresentation,0,cachedImages,
                                          1,cachedSupport);
   ideal cachedNewCoordinates=cachedZero.basis*cachedOld[2];
   ideal cachedOldCoordinates=cachedOld[1]*cachedZero.denom;
   size(NF(cachedNewCoordinates,std(cachedOldCoordinates+cachedSupport)));
   size(NF(cachedOldCoordinates,std(cachedNewCoordinates+cachedSupport)));

// Automatic recovery is performed by the constructor and reused for twists:
//--------------------------------------------------------------------------
   kill Rcached;
   ring Rautomatic=0,(x,y,z),dp;
   module automaticPresentation=-x*gen(1)+gen(2);
   attrib(automaticPresentation,"isHomog",intvec(-1,0));
   RankOneSheaf automaticSheaf=rankOneSheaf(automaticPresentation,ideal(0));
   ideal automaticImages=automaticSheaf.trivializationImages;
   poly automaticDenom=automaticSheaf.trivializationDenom;
   int automaticShift=automaticSheaf.trivializationShift;
   list automaticCachedFrame=rankOneTrivialization(automaticSheaf);
   cachedFrameEntriesEqual(automaticCachedFrame[1],automaticImages);
   automaticCachedFrame[2]==automaticDenom;
   automaticCachedFrame[3]==automaticShift;
   SectionSpace automaticZero=rankOneSheafSectionBasis(automaticSheaf,0);
   SectionSpace automaticOne=rankOneSheafSectionBasis(automaticSheaf,1);
   SectionSpace automaticZeroAgain=rankOneSheafSectionBasis(automaticSheaf,0);
   size(automaticZero);
   size(automaticOne);
   size(automaticZeroAgain);
   cachedFrameEntriesEqual(automaticZero.trivializationImages,automaticImages);
   cachedFrameEntriesEqual(automaticOne.trivializationImages,automaticImages);
   cachedFrameEntriesEqual(automaticZeroAgain.trivializationImages,
                           automaticImages);
   automaticZero.trivializationDenom==automaticDenom;
   automaticOne.trivializationDenom==automaticDenom;
   automaticZeroAgain.trivializationDenom==automaticDenom;
   automaticZero.trivializationShift==automaticShift;
   automaticOne.trivializationShift==automaticShift;
   // The module-input API is retained and gives the same rational span.
   SectionSpace automaticOld=rankOneSheafSectionBasis(
                               automaticPresentation,0,ideal(0));
   ideal automaticNewCoordinates=automaticZero.basis*automaticOld.denom;
   ideal automaticOldCoordinates=automaticOld.basis*automaticZero.denom;
   size(NF(automaticNewCoordinates,std(automaticOldCoordinates)));
   size(NF(automaticOldCoordinates,std(automaticNewCoordinates)));
   // lineBundle is the intended-use alias, not a second cached data model.
   RankOneSheaf automaticLineBundle=lineBundle(automaticPresentation,
                                                ideal(0),ideal(z,x*z),z);
   typeof(automaticLineBundle);
   SectionSpace automaticLineSections=lineBundleSectionBasis(
                                        automaticLineBundle,0);
   size(automaticLineSections);
   cachedFrameEntriesEqual(automaticLineSections.trivializationImages,
                           ideal(z,x*z));
   automaticLineSections.trivializationDenom==z;
   SectionSpace automaticLineMultTable=lineBundleSectionBasisMultTable(
                                         automaticLineBundle,0);
   size(automaticLineMultTable);
   ideal automaticLineCoordinates=automaticLineSections.basis*
                                   automaticLineMultTable.denom;
   ideal automaticLineMultTableCoordinates=automaticLineMultTable.basis*
                                            automaticLineSections.denom;
   size(NF(automaticLineCoordinates,std(automaticLineMultTableCoordinates)));
   size(NF(automaticLineMultTableCoordinates,std(automaticLineCoordinates)));

// Cached qring frames retain zero image rows and also handle empty H^0:
//---------------------------------------------------------------------
   kill Rautomatic;
   ring RcachedQAmbient=0,(x,y,z),dp;
   ideal cachedQEquation=z;
   qring RcachedQ=std(cachedQEquation);
   module cachedQPresentation=gen(2);
   attrib(cachedQPresentation,"isHomog",intvec(0,0));
   ideal cachedQImages=1,0;
   RankOneSheaf cachedQSheaf=rankOneSheaf(cachedQPresentation,ideal(0),
                                          cachedQImages,1);
   SectionSpace cachedQZero=rankOneSheafSectionBasis(cachedQSheaf,0);
   SectionSpace cachedQOne=rankOneSheafSectionBasis(cachedQSheaf,1);
   SectionSpace cachedQMinus=rankOneSheafSectionBasis(cachedQSheaf,-1);
   size(cachedQZero);
   size(cachedQOne);
   size(cachedQMinus);
   cachedFrameEntriesEqual(cachedQZero.trivializationImages,cachedQImages);
   cachedFrameEntriesEqual(cachedQOne.trivializationImages,cachedQImages);
   cachedFrameEntriesEqual(cachedQMinus.trivializationImages,cachedQImages);
   cachedQZero.trivializationImages;
   cachedQZero.trivializationDenom!=0;
   cachedQMinus.trivializationDenom!=0;
   SectionSpace cachedQDirect=rankOneSheafSectionBasisDirect(cachedQSheaf,0);
   size(cachedQDirect);
   cachedFrameEntriesEqual(cachedQDirect.trivializationImages,cachedQImages);

// Finite linear algebra can recover and cache a frame without graded Hom:
//-----------------------------------------------------------------------
   ring RlinearFrames=0,(x,y,z),dp;
   // The two relation columns have different weighted degrees.  This also
   // exercises the sparse constant nullspace over QQ.
   module weightedLinearPresentation=
     -x*gen(1)+gen(2),-y*gen(2)+gen(3);
   attrib(weightedLinearPresentation,"isHomog",intvec(-1,0,1));
   list weightedLinearFrame=rankOneTrivializationLinear(
                              weightedLinearPresentation,ideal(0));
   weightedLinearFrame[1];
   weightedLinearFrame[3];
   RankOneSheaf weightedLinearSheaf=rankOneSheafLinear(
                                      weightedLinearPresentation,ideal(0));
   RankOneSheaf weightedHomSheaf=rankOneSheaf(
                                   weightedLinearPresentation,ideal(0));
   RankOneSheaf weightedLinearBundle=lineBundleLinear(
                                       weightedLinearPresentation,ideal(0));
   typeof(weightedLinearBundle);
   SectionSpace weightedLinearMinus=rankOneSheafSectionBasis(
                                      weightedLinearSheaf,-2);
   SectionSpace weightedLinearZero=rankOneSheafSectionBasis(
                                     weightedLinearSheaf,0);
   SectionSpace weightedLinearPlus=rankOneSheafSectionBasis(
                                     weightedLinearSheaf,1);
   size(weightedLinearMinus);
   size(weightedLinearZero);
   size(weightedLinearPlus);
   SectionSpace weightedHomMinus=rankOneSheafSectionBasis(
                                   weightedHomSheaf,-2);
   SectionSpace weightedHomZero=rankOneSheafSectionBasis(
                                  weightedHomSheaf,0);
   SectionSpace weightedHomPlus=rankOneSheafSectionBasis(
                                  weightedHomSheaf,1);
   size(weightedLinearMinus)==size(weightedHomMinus);
   size(weightedLinearZero)==size(weightedHomZero);
   size(weightedLinearPlus)==size(weightedHomPlus);
   size(lineBundleSectionBasis(weightedLinearBundle,0));
   // Complete one-call variants use the same linear frame recovery, followed
   // by the multiplication-table section algorithm.
   SectionSpace weightedLinearOneCall=rankOneSheafSectionBasisLinear(
                                        weightedLinearPresentation,0,ideal(0));
   SectionSpace weightedLineOneCall=lineBundleSectionBasisLinear(
                                      weightedLinearPresentation,0,ideal(0),
                                      0,1,0);
   size(weightedLinearOneCall)==size(weightedLinearZero);
   size(weightedLineOneCall)==size(weightedLinearZero);
   ideal weightedOneCallCoordinates=weightedLinearOneCall.basis*
                                    weightedLinearZero.denom;
   ideal weightedCachedCoordinates=weightedLinearZero.basis*
                                   weightedLinearOneCall.denom;
   size(NF(weightedOneCallCoordinates,std(weightedCachedCoordinates)));
   size(NF(weightedCachedCoordinates,std(weightedOneCallCoordinates)));
   // Align denominators and compare the rational spans with both the old Hom
   // constructor and the direct saturation algorithm.
   ideal weightedLinearCoordinates=weightedLinearZero.basis*
                                   weightedHomZero.denom;
   ideal weightedHomCoordinates=weightedHomZero.basis*
                                weightedLinearZero.denom;
   size(NF(weightedLinearCoordinates,std(weightedHomCoordinates)));
   size(NF(weightedHomCoordinates,std(weightedLinearCoordinates)));
   SectionSpace weightedDirectZero=rankOneSheafSectionBasisDirect(
                                     weightedHomSheaf,0);
   ideal weightedLinearDirectCoordinates=weightedLinearZero.basis*
                                         weightedDirectZero.denom;
   ideal weightedDirectCoordinates=weightedDirectZero.basis*
                                   weightedLinearZero.denom;
   size(NF(weightedLinearDirectCoordinates,std(weightedDirectCoordinates)));
   size(NF(weightedDirectCoordinates,std(weightedLinearDirectCoordinates)));

// Degree slices must retain absent trailing generator rows, and free sheaves
// retain their assigned generator shift:
//--------------------------------------------------------------------------
   module trailingLinearPresentation=gen(1)-x*gen(2);
   attrib(trailingLinearPresentation,"isHomog",intvec(1,0));
   list trailingLinearFrame=rankOneTrivializationLinear(
                              trailingLinearPresentation,ideal(0));
   trailingLinearFrame[1];
   trailingLinearFrame[3];
   matrix weightedFreeMatrix[1][1];
   module weightedFreePresentation=weightedFreeMatrix;
   attrib(weightedFreePresentation,"isHomog",intvec(2));
   list weightedFreeFrame=rankOneTrivializationLinear(
                            weightedFreePresentation,ideal(0));
   weightedFreeFrame[1];
   weightedFreeFrame[3];
   module ungradedFreePresentation=0;
   RankOneSheaf ungradedFreeSheaf=rankOneSheafLinear(
                                    ungradedFreePresentation,ideal(0));
   typeof(attrib(ungradedFreeSheaf.presentation,"isHomog"))=="intvec";
   size(rankOneSheafSectionBasis(ungradedFreeSheaf,0));
   // Here coker(M)=0, so the bounded scan terminates without a functional.
   module zeroCokernelPresentation=gen(1),gen(2);
   attrib(zeroCokernelPresentation,"isHomog",intvec(0,0));
   rankOneTrivializationLinear(zeroCokernelPresentation,ideal(0));

// The same sparse nullspace construction works in positive characteristic:
//--------------------------------------------------------------------------
   ring RlinearFinite=5,(x,y,z),dp;
   ideal finiteSquare=maxideal(2);
   module finitePresentation=syz(finiteSquare);
   attrib(finitePresentation,"isHomog",intvec(2,2,2,2,2,2));
   list finiteLinearFrame=rankOneTrivializationLinear(finitePresentation,
                                                       ideal(0));
   finiteLinearFrame[1];
   finiteLinearFrame[3];
   RankOneSheaf finiteLinearSheaf=rankOneSheafLinear(finitePresentation,
                                                      ideal(0));
   RankOneSheaf finiteHomSheaf=rankOneSheaf(finitePresentation,ideal(0));
   SectionSpace finiteLinearMinus=rankOneSheafSectionBasis(
                                    finiteLinearSheaf,-1);
   SectionSpace finiteLinearZero=rankOneSheafSectionBasis(
                                    finiteLinearSheaf,0);
   SectionSpace finiteLinearPlus=rankOneSheafSectionBasis(
                                   finiteLinearSheaf,1);
   size(finiteLinearMinus);
   size(finiteLinearZero);
   size(finiteLinearPlus);
   SectionSpace finiteLinearOneCall=rankOneSheafSectionBasisLinear(
                                     finitePresentation,0,ideal(0));
   SectionSpace finiteLineOneCall=lineBundleSectionBasisLinear(
                                   finitePresentation,0,ideal(0));
   size(finiteLinearOneCall)==size(finiteLinearZero);
   size(finiteLineOneCall)==size(finiteLinearZero);
   SectionSpace finiteHomMinus=rankOneSheafSectionBasis(finiteHomSheaf,-1);
   SectionSpace finiteHomZero=rankOneSheafSectionBasis(finiteHomSheaf,0);
   SectionSpace finiteHomPlus=rankOneSheafSectionBasis(finiteHomSheaf,1);
   size(finiteLinearMinus)==size(finiteHomMinus);
   size(finiteLinearZero)==size(finiteHomZero);
   size(finiteLinearPlus)==size(finiteHomPlus);
   ideal finiteLinearCoordinates=finiteLinearZero.basis*finiteHomZero.denom;
   ideal finiteHomCoordinates=finiteHomZero.basis*finiteLinearZero.denom;
   size(NF(finiteLinearCoordinates,std(finiteHomCoordinates)));
   size(NF(finiteHomCoordinates,std(finiteLinearCoordinates)));

// The explicit Linear command always uses Singular's exact generic kernel,
// including in characteristic two; it never probes or falls back from SpaSM:
//--------------------------------------------------------------------------
   ring RlinearGeneric=2,(x,y,z),dp;
   module genericPresentation=-x*gen(1)+gen(2);
   attrib(genericPresentation,"isHomog",intvec(-1,0));
   SectionSpace genericLinear=rankOneSheafSectionBasisLinear(
                               genericPresentation,0,ideal(0));
   SectionSpace genericHom=rankOneSheafSectionBasis(
                            genericPresentation,0,ideal(0));
   size(genericLinear);
   size(genericLinear)==size(genericHom);
   ideal genericLinearCoordinates=genericLinear.basis*genericHom.denom;
   ideal genericHomCoordinates=genericHom.basis*genericLinear.denom;
   size(NF(genericLinearCoordinates,std(genericHomCoordinates)));
   size(NF(genericHomCoordinates,std(genericLinearCoordinates)));

// The kernel nullspace also works over exact algebraic extensions:
//-------------------------------------------------------------------
   ring RlinearAlgebraic=(0,a),(x,y,z),dp;
   minpoly=a2+1;
   module algebraicPresentation=-a*x*gen(1)+gen(2);
   attrib(algebraicPresentation,"isHomog",intvec(-1,0));
   list algebraicFrame=rankOneTrivializationLinear(algebraicPresentation,
                                                    ideal(0));
   algebraicFrame[1];
   algebraicFrame[3];
   RankOneSheaf algebraicLinearSheaf=rankOneSheafLinear(
                                       algebraicPresentation,ideal(0));
   RankOneSheaf algebraicHomSheaf=rankOneSheaf(algebraicPresentation,
                                                ideal(0));
   SectionSpace algebraicLinearZero=rankOneSheafSectionBasis(
                                      algebraicLinearSheaf,0);
   SectionSpace algebraicHomZero=rankOneSheafSectionBasis(
                                   algebraicHomSheaf,0);
   size(algebraicLinearZero)==size(algebraicHomZero);
   ideal algebraicLinearCoordinates=algebraicLinearZero.basis*
                                    algebraicHomZero.denom;
   ideal algebraicHomCoordinates=algebraicHomZero.basis*
                                 algebraicLinearZero.denom;
   size(NF(algebraicLinearCoordinates,std(algebraicHomCoordinates)));
   size(NF(algebraicHomCoordinates,std(algebraicLinearCoordinates)));

// Qring lifting combines the ambient quotient with the explicit support.
// A reduced zero relation before a surviving relation and its trailing zero
// frame coordinate are both retained:
//--------------------------------------------------------------------------
   ring RlinearQAmbient=0,(w,x,y,z),dp;
   qring RlinearQ=std(w);
   ideal linearQSupport=z;
   module linearQPresentation=z*gen(1),gen(2);
   attrib(linearQPresentation,"isHomog",intvec(0,0));
   list linearQFrame=rankOneTrivializationLinear(linearQPresentation,
                                                 linearQSupport);
   linearQFrame[1];
   ncols(matrix(linearQFrame[1]));
   linearQFrame[3];
   RankOneSheaf linearQSheaf=rankOneSheafLinear(linearQPresentation,
                                                 linearQSupport);
   RankOneSheaf homQSheaf=rankOneSheaf(linearQPresentation,linearQSupport);
   SectionSpace linearQMinus=rankOneSheafSectionBasis(linearQSheaf,-1);
   SectionSpace linearQZero=rankOneSheafSectionBasis(linearQSheaf,0);
   SectionSpace linearQPlus=rankOneSheafSectionBasis(linearQSheaf,1);
   size(linearQMinus);
   size(linearQZero);
   size(linearQPlus);
   SectionSpace homQMinus=rankOneSheafSectionBasis(homQSheaf,-1);
   SectionSpace homQZero=rankOneSheafSectionBasis(homQSheaf,0);
   SectionSpace homQPlus=rankOneSheafSectionBasis(homQSheaf,1);
   size(linearQMinus)==size(homQMinus);
   size(linearQZero)==size(homQZero);
   size(linearQPlus)==size(homQPlus);
   SectionSpace directQPlus=rankOneSheafSectionBasisDirect(homQSheaf,1);
   ideal linearQCoordinates=linearQPlus.basis*directQPlus.denom;
   ideal directQCoordinates=directQPlus.basis*linearQPlus.denom;
   size(NF(linearQCoordinates,std(directQCoordinates+linearQSupport)));
   size(NF(directQCoordinates,std(linearQCoordinates+linearQSupport)));

// The complete truncated section module is reconstructed from its certified
// linear tail.  The point tests genuine descent through negative twists, while
// O and O plus irrelevant torsion exercise the first vanishing lower space:
//----------------------------------------------------------------------------
   ring RsectionModule=0,(x,y,z),dp;
   module sectionPoint=x,y;
   attrib(sectionPoint,"isHomog",intvec(0));
   module pointTail=sheafSectionModuleLinear(sectionPoint,-2);
   homog(pointTail);
   intvec(dimGradedPart(pointTail,-3),dimGradedPart(pointTail,-2),
          dimGradedPart(pointTail,-1),dimGradedPart(pointTail,0),
          dimGradedPart(pointTail,1),dimGradedPart(pointTail,2));
   if (sheafSectionDimensionLinear(sectionPoint,-2)!=1)
   {
     ERROR("wrong dimension-only point section space");
   }
   if (sheafSectionDimension(sectionPoint,2)!=dimGradedPart(pointTail,2))
   {
     ERROR("automatic dimension-only result differs from the section module");
   }
   module highPointTail=sheafSectionModuleLinear(sectionPoint,4);
   intvec(dimGradedPart(highPointTail,3),dimGradedPart(highPointTail,4),
          dimGradedPart(highPointTail,5));
   matrix zeroPresentationMatrix[1][1];
   module sectionO=zeroPresentationMatrix;
   attrib(sectionO,"isHomog",intvec(0));
   module structureTail=sheafSectionModule(sectionO,-2);
   intvec(dimGradedPart(structureTail,-3),dimGradedPart(structureTail,-2),
          dimGradedPart(structureTail,-1),dimGradedPart(structureTail,0),
          dimGradedPart(structureTail,1),dimGradedPart(structureTail,2));
   module sectionTorsion=x*gen(2),y*gen(2),z*gen(2);
   attrib(sectionTorsion,"isHomog",intvec(0,0));
   module torsionTail=sheafSectionModuleLinear(sectionTorsion,-2);
   intvec(dimGradedPart(torsionTail,-1),dimGradedPart(torsionTail,0),
          dimGradedPart(torsionTail,1),dimGradedPart(torsionTail,2));

// A nonminimal presentation may reduce a nonstandard variable product to a
// different ambient generator.  The degree-(q+1) basis must therefore be the
// full standard-monomial slice, not merely the standard products of degree q.
// Here e_2=x^2 e_1, so coker(nonminimalO) is O although x^2 e_1 itself lies
// in the initial module.
   module nonminimalO=[x2,-1];
   attrib(nonminimalO,"isHomog",intvec(0,2));
   module nonminimalTail=sheafSectionModuleLinear(nonminimalO,0);
   intvec(dimGradedPart(nonminimalTail,0),
          dimGradedPart(nonminimalTail,1),
          dimGradedPart(nonminimalTail,2),
          dimGradedPart(nonminimalTail,3));
   if (sheafSectionDimensionLinear(nonminimalO,2)!=6)
   {
     ERROR("wrong section dimension for a nonminimal presentation");
   }

// A rank-two sheaf checks the full multiplication tables and agrees with the
// independent local-duality computation in every requested degree:
//--------------------------------------------------------------------------
   module tangentMinusOne=x*gen(1)+y*gen(2)+z*gen(3);
   attrib(tangentMinusOne,"isHomog",intvec(0,0,0));
   module tangentTail=sheafSectionModuleLinear(tangentMinusOne,-3);
   intvec(dimGradedPart(tangentTail,-3),dimGradedPart(tangentTail,-2),
          dimGradedPart(tangentTail,-1),dimGradedPart(tangentTail,0),
          dimGradedPart(tangentTail,1),dimGradedPart(tangentTail,2),
          dimGradedPart(tangentTail,3));
   intvec(dimH(0,tangentMinusOne,-3),dimH(0,tangentMinusOne,-2),
          dimH(0,tangentMinusOne,-1),dimH(0,tangentMinusOne,0),
          dimH(0,tangentMinusOne,1),dimH(0,tangentMinusOne,2),
          dimH(0,tangentMinusOne,3));
   if (sheafSectionDimensionLinear(tangentMinusOne,-2)
       !=dimH(0,tangentMinusOne,-2))
   {
     ERROR("dimension-only rank-two result differs from local duality");
   }

// Automatic backend selection remains exact when SpaSM is inapplicable:
//-----------------------------------------------------------------------
   ring RsectionModuleTwo=2,(x,y,z),dp;
   module sectionPointTwo=x,y;
   attrib(sectionPointTwo,"isHomog",intvec(0));
   module automaticTailTwo=sheafSectionModule(sectionPointTwo,-2);
   module linearTailTwo=sheafSectionModuleLinear(sectionPointTwo,-2);
   intvec(dimGradedPart(automaticTailTwo,-3),
          dimGradedPart(automaticTailTwo,-2),
          dimGradedPart(automaticTailTwo,-1),
          dimGradedPart(automaticTailTwo,0),
          dimGradedPart(automaticTailTwo,1));
   intvec(dimGradedPart(linearTailTwo,-3),dimGradedPart(linearTailTwo,-2),
          dimGradedPart(linearTailTwo,-1),dimGradedPart(linearTailTwo,0),
          dimGradedPart(linearTailTwo,1));
   if (sheafSectionDimension(sectionPointTwo,-1)
       !=sheafSectionDimensionLinear(sectionPointTwo,-1))
   {
     ERROR("automatic characteristic-two dimension backend differs from linear");
   }

// Homogeneous qrings and P^0 use the same public interface:
//-----------------------------------------------------------
   ring RsectionModuleQAmbient=0,(x,y,z),dp;
   qring RsectionModuleQ=std(x2+y2+z2);
   module conicO=0;
   attrib(conicO,"isHomog",intvec(0));
   module conicTail=sheafSectionModuleLinear(conicO,-2);
   intvec(dimGradedPart(conicTail,-2),dimGradedPart(conicTail,-1),
          dimGradedPart(conicTail,0),dimGradedPart(conicTail,1),
          dimGradedPart(conicTail,2),dimGradedPart(conicTail,3));
   if (sheafSectionDimensionLinear(conicO,1)
       !=dimGradedPart(conicTail,1))
   {
     ERROR("dimension-only qring result differs from the section module");
   }
   ring RsectionModulePZero=0,(x),dp;
   module pointSpaceO=0;
   attrib(pointSpaceO,"isHomog",intvec(0));
   module pointSpaceTail=sheafSectionModuleLinear(pointSpaceO,-3);
   intvec(dimGradedPart(pointSpaceTail,-4),dimGradedPart(pointSpaceTail,-3),
          dimGradedPart(pointSpaceTail,-2),dimGradedPart(pointSpaceTail,-1),
          dimGradedPart(pointSpaceTail,0),dimGradedPart(pointSpaceTail,1));
   if (sheafSectionDimensionLinear(pointSpaceO,-3)!=1)
   {
     ERROR("wrong dimension-only P^0 section space");
   }

tst_status(1);$
