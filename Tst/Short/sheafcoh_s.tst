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

tst_status(1);$

