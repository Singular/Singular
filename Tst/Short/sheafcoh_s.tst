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

// --------------------------------------------------
// Test of globalSectionsBLH and its degree cutoff:
   kill R;
   ring Q=0,(x,y,z,u),dp;
   resolution T=mres(maxideal(1),0);
   module C=T[3];
   intvec cv=2,2,2,2,2,2;
   attrib(C,"isHomog",cv);
   module G0=globalSectionsBLH(C,2,0,cv);
   print(betti(mres(G0,0)),"betti");
   int d;
   intvec h0;
   for (d=0;d<=5;d++)
   {
     h0[d+1]=dimGradedPart(G0,d);
   }
   h0;
   // Minimize the presentation without shifting its graded generators.
   module PG0=pruneGraded(G0);
   print(string(nrows(PG0))+"x"+string(ncols(PG0)));
   attrib(PG0,"isHomog");
   print(betti(mres(PG0,0)),"betti");
   intvec ph0;
   for (d=0;d<=5;d++)
   {
     ph0[d+1]=dimGradedPart(PG0,d);
   }
   ph0;
   // Computing directly at a higher cutoff agrees with truncating G0.
   module G3=globalSectionsBLH(C,2,3,cv);
   module TG3=truncate(G0,3);
   intvec h3direct;
   intvec h3truncated;
   for (d=2;d<=5;d++)
   {
     h3direct[d-1]=dimGradedPart(G3,d);
     h3truncated[d-1]=dimGradedPart(TG3,d);
   }
   h3direct;
   h3truncated;

// A finite-length module sheafifies to zero, including at its regularity.
   kill Q;
   ring L=0,(x,y),dp;
   module Finite=x,y;
   intvec finiteDegrees=0;
   module HFinite=globalSectionsBLH(Finite,0,0,finiteDegrees);
   intvec hFinite;
   int dFinite;
   for (dFinite=0;dFinite<=2;dFinite++)
   {
     hFinite[dFinite+1]=dimGradedPart(HFinite,dFinite);
   }
   hFinite;

// Recover nonzero sections below regularity for the sheaf of a point.
   kill L;
   ring P=0,(x,y,z),dp;
   module Point=x,y;
   intvec pointDegrees=0;
   module HPoint=globalSectionsBLH(Point,0,-2,pointDegrees);
   intvec hPoint;
   int dPoint;
   for (dPoint=-3;dPoint<=2;dPoint++)
   {
     hPoint[dPoint+4]=dimGradedPart(HPoint,dPoint);
   }
   hPoint;

// A free summand plus irrelevant torsion checks that the irrelevant part is
// discarded by the section construction; the associated sheaf is just O.
   module Mixed=x*gen(2),y*gen(2),z*gen(2);
   intvec mixedDegrees=0,0;
   module HMixed=globalSectionsBLH(Mixed,0,-2,mixedDegrees);
   intvec hMixed;
   int dMixed;
   for (dMixed=-3;dMixed<=4;dMixed++)
   {
     hMixed[dMixed+4]=dimGradedPart(HMixed,dMixed);
   }
   hMixed;

// Repeated unit eliminations used to delete the wrong component weights.
   kill P;
   ring W=0,(x,y),dp;
   matrix WA[3][2]=1,0,
                    0,1,
                    0,0;
   module WM=WA;
   attrib(WM,"isHomog",intvec(3,3,2));
   module WP=pruneGraded(WM);
   WP;
   attrib(WP,"isHomog");
   intvec hWM;
   intvec hWP;
   int dW;
   for (dW=1;dW<=4;dW++)
   {
     hWM[dW]=dimGradedPart(WM,dW);
     hWP[dW]=dimGradedPart(WP,dW);
   }
   hWM;
   hWP;

// Preserve weights for zero relations and use the canonical zero weight if
// every ambient generator is cancelled.
   matrix WZ[2][1];
   module WFree=WZ;
   attrib(WFree,"isHomog",intvec(4,7));
   module WPFree=pruneGraded(WFree);
   nrows(WPFree);
   ncols(WPFree);
   size(WPFree);
   attrib(WPFree,"isHomog");
   module WAll=unitmat(2);
   attrib(WAll,"isHomog",intvec(3,5));
   module WPAll=pruneGraded(WAll);
   nrows(WPAll);
   size(WPAll);
   attrib(WPAll,"isHomog");
   module WTAll=truncate(WPAll,0);
   nrows(WTAll);
   size(WTAll);

// Nonconsecutive eliminations and pivot order must select the surviving
// original component in every fibre of the kernel component map.
   module WN=gen(2),gen(4),x11*gen(3)+x*gen(5);
   attrib(WN,"isHomog",intvec(0,5,10,15,20));
   module WPN=pruneGraded(WN);
   attrib(WPN,"isHomog");
   module WNR=gen(4),gen(2),x11*gen(3)+x*gen(5);
   attrib(WNR,"isHomog",intvec(0,5,10,15,20));
   module WPNR=pruneGraded(WNR);
   attrib(WPNR,"isHomog");

tst_status(1);$
