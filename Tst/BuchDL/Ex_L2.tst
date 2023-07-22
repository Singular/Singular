LIB "tst.lib";
tst_init();


//======================  Example 2.28 =============================
//================== Output depends on random ======================
ring R = 0, x(0..3), dp;
matrix A[4][1] = x(0),x(1),0,0;
LIB "random.lib";  // loads other libraries incl. matrix.lib
                   // and elim.lib, too
matrix B = randommat(4,2,maxideal(1),100);


matrix M = concat(B,A);  // from matrix.lib
print(M);

ideal I = minor(M,3);
ideal GI = groebner(I);
int codimI = nvars(R)-dim(GI);
codimI;
//->    2

ideal singI = groebner(minor(jacob(GI),codimI) + I);
nvars(R) - dim(singI);
//->    3

print(betti(singI,0),"betti");
//->               0     1
//->    ------------------
//->        0:     1     -
//->        1:     -     -
//->        2:     -     4
//->        3:     -    20
//->    ------------------
//->    total:     1    24

ideal singI_sat = sat(singI,maxideal(1)); // from elim.lib
print(betti(singI_sat,0),"betti");
//->               0     1
//->    ------------------
//->        0:     1     2
//->        1:     -     1
//->    ------------------
//->    total:     1     3

singI_sat;
//->    singI_sat[1]=x(1)
//->    singI_sat[2]=x(0)
//->    singI_sat[3]=3297*x(2)^2-2680*x(2)*x(3)-5023*x(3)^2

ideal IL = x(0),x(1);
reduce(I,groebner(IL),1);
//->   _[1]=0
//->   _[2]=0
//->   _[3]=0
//->   _[4]=0

ideal I' = sat(I,IL);   // result is Groebner basis

degree(GI);
//->   // dimension (proj.)  = 1
//->   // degree (proj.)   = 6

degree(I');
//->   // dimension (proj.)  = 1
//->   // degree (proj.)   = 5

int codimI' = nvars(R)-dim(I');
ideal singI' = minor(jacob(I'),codimI') + I';
nvars(R) - dim(groebner(singI'));
//->    4


kill R,codimI,codimI';
//==================  Example 2.33 (New Session)  =========================
ring P1P3 = 0, (s,t,w,x,y,z), (dp(2),dp);
ideal J = w-s3, x-s2t, y-st2, z-t3;
J = groebner(J);
J;
//->   J[1]=y2-xz
//->   J[2]=xy-wz
//->   J[3]=x2-wy
//->   J[4]=sz-ty
//->   [...]
//->   J[10]=s3-w

ring P1 = 0, (s,t), dp;
ideal ZERO;
ideal PARA = s3, s2t, st2, t3;
ring P3 = 0, (w,x,y,z), dp;
ideal IC = preimage(P1,PARA,ZERO);
print(IC);
//->   y2-xz,
//->   xy-wz,
//->   x2-wy

ideal P =  w-y, x, z;
size(reduce(IC,groebner(P),1));  // ideal membership test
//->   2

ring P2 = 0, (a,b,c), dp;
ideal PIC = preimage(P3,P,IC);
PIC;
//->   PIC[1]=b3-a2c-2b2c+bc2

setring P3;
ideal Q = w, y, z;
size(reduce(IC,groebner(Q),1));   // check: Q not on C
//->   1
setring P2;
ideal QIC = preimage(P3,Q,IC);
QIC;
//->   QIC[1]=b3-ac2


kill P1,P3,P1P3,P2;
//==================  Example 2.34 (New Session)  =========================
ring S2 = 0, x(1..3), dp;
ideal SPHERE = x(1)^2+x(2)^2+x(3)^2-1;
ideal MAP = x(1)*x(2), x(1)*x(3), x(2)*x(3);
ring R3 = 0, y(1..3), dp;
ideal ST = preimage(S2, MAP, SPHERE);
print(ST);
//->   y(1)^2*y(2)^2+y(1)^2*y(3)^2+y(2)^2*y(3)^2-y(1)*y(2)*y(3)



tst_status(1);$

