LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def U = makeUsl2(); // this algebra is U(sl_2)
setring U;
ideal i= e^3, f^3, h^3 - 4*h;
option(redSB);
option(redTail);
ideal I = std(i);
print(matrix(I)); // print a compact presentation of I
ideal J = twostd(i);
// print a compact presentation of J:
print(matrix(ideal(J[1..6]))); // first 6 gen's
print(matrix(ideal(J[7..size(J)]))); // the rest of gen's
// compute the set of elements present in J but not in I
ideal K = NF(J,I);
K = K+0; // simplify K
print(matrix(K));
tst_status(1);$
