LIB "tst.lib";
tst_init();

LIB "sheafcoh.lib";

// Kohomologie der Strukturgarbe von P^7:
//----------------------------------------
ring r=0,x(1..8),dp;
module M=0;
def A=sheafCoh(0,-10,5);
def B=sheafCohBGG(0,-10,3);
displayCohom(A,-10,5,7);
displayCohom(B,-10,3,7);
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
 A=sheafCoh(F,-6,6);
 displayCohom(A,-6,6,nvars(basering)-1);
 B=sheafCohBGG(F,-6,6);
 displayCohom(B,-6,6,nvars(basering)-1);

 dimH(3,F,-4);
 dimH(1,F,1);

 kill S;

// Kohomologie der Idealgarbe einer Flaeche in $\P^4$:
//------------------------------------------------------------
 ring S = 32003, x(0..4), dp;
 resolution kos = nres(maxideal(1),0);
 betti(kos);
 matrix kos5 = kos[5];
 matrix tphi = transpose(dsum(kos5,kos5));
 matrix kos3 = kos[3];
 matrix psi = dsum(kos3,kos3);
 matrix beta1 = random(32002,20,2);
 matrix tbeta1tilde = transpose(psi*beta1);
 matrix tbeta0 = lift(tphi,tbeta1tilde);
 matrix kos4 = kos[4];
 matrix tkos4pluskos4 = transpose(dsum(kos4,kos4));
 matrix tgammamin1 = random(32002,20,1);
 matrix tgamma0 = tkos4pluskos4*tgammamin1;
 matrix talpha0 = concat(tbeta0,tgamma0);
 matrix zero[20][1];
 matrix tpsi = transpose(psi);
 matrix tpresg = concat(tpsi,zero);
 matrix pres = module(transpose(talpha0))
                    + module(transpose(tpresg));
 module dir = transpose(pres);
 dir = prune(dir);
 homog(dir);
 intvec deg_dir = attrib(dir,"isHomog");
 attrib(dir,"isHomog",deg_dir-2);        // set degrees
 resolution fdir = mres(prune(dir),2);
 print(betti(fdir),"betti");
 ideal I = groebner(flatten(fdir[2]));
 resolution FI = mres(I,0);

 module F=FI[2];
 def A1=sheafCoh(F,-4,6);
 displayCohom(A1,-4,6,nvars(basering)-1);
 A1=sheafCoh(F,-3,6,"sres");
 displayCohom(A1,-3,6,nvars(basering)-1);
 def A2=sheafCohBGG(F,-4,6);
 displayCohom(A2,-4,6,nvars(basering)-1);

kill S;

// Kohomologie des Morrocks-Mumford-Buendels (Macaulay-Beispiel von Wolfram):
// ----------------------------------------------------------------------------

ring R=0,x(0..4),dp;

matrix FHM[19][35] =
 -x(2), 0,    -x(1), -x(4), 0,    0,    0,     0,     0,      0,      0,
0,      0,     0,     0,      0,      0,       0,       0,      0,      0,
0,     0,     -x(0),   0,      0,       0,       0,      0,      -x(4),  0,
0,      -x(3),    -x(2),    0,

0,    0,    x(0),  0,    0,    0,    0,     0,     -x(3),   0,      0,      0,
0,     0,     -x(1),   0,      0,       0,       0,      x(4),    0,      0,
0,     0,      0,      0,       0,       0,      0,      0,     -x(3),  -x(2),
0,       0,       0,

0,    x(3),  0,    0,    0,    0,    0,     0,     0,      0,      0,      0,
0,     0,     0,      0,      0,       0,       0,      0,      x(2),    0,
0,     x(1),    0,      0,       0,       0,      -x(0),   0,     0,     -x(4),
0,       0,       -x(3),

0,    0,    0,    -x(2), -x(3), 0,    -x(3),  0,     0,      0,      0,
-x(1),   0,     0,     x(0),    0,      0,       0,       0,      0,
-x(4),   0,     0,     0,      0,      0,       0,       0,      0,      -x(2),
0,     0,      0,       0,       0,

x(4),  0,    0,    0,    0,    0,    0,     0,     0,      0,      0,
x(0),    0,     0,     0,      0,      0,       0,       -x(3),   x(2),    0,
0,     0,     0,      0,      0,       0,       0,      -x(1),   0,     0,
0,      0,       x(4),     0,

x(1),  0,    0,    0,    0,    0,    0,     0,     0,      x(2),    0,      0,
0,     0,     0,      0,      0,       0,       0,      0,      -x(0),   0,
0,     0,      x(4),    0,       0,       -x(3),   0,      0,     0,     0,
0,       0,       0,

0,    0,    0,    x(0),  0,    0,    0,     0,     0,      -x(4),   x(2),    0,
-x(4),  0,     0,      -x(3),   0,       0,       0,      -x(1),   0,      0,
0,     0,      0,      0,       0,       0,      0,      0,     0,     0,
0,       0,       0,

0,    x(4),  -x(3), 0,    -x(2), 0,    0,     0,     0,      -x(0),   0,
0,      0,     0,     0,      0,      0,       0,       0,      0,      0,
0,     -x(3),  0,      0,      0,       -x(1),    0,      0,      0,     0,
0,      0,       0,       -x(4),

0,    -x(2), 0,    0,    0,    0,    0,     0,     x(4),    0,      -x(1),
x(3),    0,     0,     0,      0,      0,       0,       0,      0,      0,
0,     0,     0,      -x(0),   0,       0,       0,      0,      0,     0,
0,      0,       0,       0,

0,    0,    0,    0,    x(0),  0,    0,     0,     0,      0,      0,      0,
0,     0,     0,      -x(2),   0,       0,       -x(1),   0,      0,
-x(1),  0,     0,      0,      0,       0,       -x(4),   0,      0,     0,
x(3),    0,       0,       0,

0,    0,    -x(4), 0,    0,    -x(1), 0,     0,     0,      0,      0,      0,
0,     -x(1),  0,      x(0),    0,       0,       0,      0,      0,      0,
-x(4),  0,      -x(3),   0,       0,       0,      -x(2),   0,     0,     0,
0,       0,       0,

0,    x(1),  0,    x(3),  0,    x(2),  0,     0,     0,      0,      0,      0,
0,     x(2),   0,      0,      0,       0,       x(0),    0,      0,      0,
0,     0,      0,      -x(4),    0,       0,      0,      0,     0,     0,
0,       0,       0,

-x(3), 0,    0,    0,    x(1),  x(4),  x(1),   0,     x(0),    0,      0,
0,      0,     0,     0,      0,      0,       -x(2),    0,      0,      0,
0,     0,     0,      0,      0,       0,       0,      0,      0,     0,
0,      0,       0,       0,

0,    0,    0,    0,    0,    0,    0,     x(3),   x(2),    -x(1),   0,      0,
-x(1),  0,     0,      0,      -x(0),    0,       -x(4),   0,      0,
-x(4),  0,     x(3),    0,      0,       0,       0,      0,      0,     x(2),
0,      0,       0,       0,

0,    0,    0,    0,    0,    x(0),  0,     -x(2),  0,      0,      -x(3),   0,
0,     0,     -x(4),   0,      0,       0,       0,      0,      0,      0,
0,     -x(2),   0,      0,       0,       x(1),    0,      0,     0,     0,
0,       0,       0,

0,    0,    0,    0,    0,    0,    0,     0,     0,      x(2)*x(4), 0,      0,
0,     0,     0,      x(2)*x(3), 0,       0,       x(1)*x(3), 0,      0,
0,     0,     0,      x(4)^2,  0,       0,       0,      x(1)^2,  0,     0,
0,      0,       -x(1)*x(4), 0,

0,    0,    0,    0,    0,    0,    0,     0,     x(3)*x(4), 0,      0,
x(3)^2,  0,     0,     x(1)*x(4), 0,      0,       0,       0,      0,
x(2)^2,  0,     0,     x(1)*x(2), 0,      0,       0,       0,      0,      0,
0,     0,      0,       0,       -x(2)*x(3),

0,    0,    0,    0,    0,    0,    x(4)^2, 0,     0,      0,      x(0)*x(4),
0,      x(3)^2, 0,     x(2)*x(3), 0,      -x(1)*x(4), x(1)*x(3),  0,      0,
0,      x(2)^2, 0,     0,      x(1)*x(2), -x(0)*x(3), x(0)*x(2),  0,
x(3)*x(4), x(1)^2, 0,     x(0)*x(1), -x(2)*x(4), x(0)^2,   0,

0,    0,    0,    0,    0,    0,    0,     x(4)^2, 0,      0,      0,
x(2)*x(4), 0,     x(3)^2, 0,      x(1)*x(4), x(2)*x(3),  -x(0)*x(4), 0,
x(0)*x(3), x(1)*x(3), 0,     x(2)^2, 0,      0,      x(1)*x(2),  -x(3)*x(4),
x(0)*x(2), 0,      0,     x(1)^2, 0,      x(0)*x(1),  0,       x(0)^2;


def M=module(FHM);
attrib(M,"isHomog",intvec(4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3));
A1=sheafCohBGG(M,-10,7);
displayCohom(A1,-10,7,nvars(basering)-1);
A2=sheafCoh(M,-4,3);
displayCohom(A2,-4,3,nvars(basering)-1);

tst_status(1);$

