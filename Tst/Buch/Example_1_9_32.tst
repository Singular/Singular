LIB "tst.lib";
tst_init();

============================= example 1.9.32  ==============================

LIB "ncalg.lib";
def A = makeUsl2();  setring A;
option(redSB);  option(redTail);
ideal I = e2,f;
ideal J = groebner(I);
resolution F = nres(J,0);
F;
print(matrix(F[1]));             // F[1] is the left map
print(matrix(F[2]));             // F[2] is the middle map
print(matrix(F[3]));             // F[3] is the right map

resolution MF = minres(F);
print(matrix(MF[1]));
print(matrix(MF[2]));
print(matrix(MF[3]));

tst_status(1);$

