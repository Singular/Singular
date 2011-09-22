LIB "tst.lib"; tst_init();
LIB "nctools.lib";
LIB "ncalg.lib";
def U = makeUsl(2);
// U is the U(sl_2) algebra
setring U;
ideal I = e^3, f^3, h^3-4*h;
option(redSB);
ideal J = slimgb(I);
J;
// compare slimgb with std:
ideal K = std(I);
print(matrix(NF(K,J)));
print(matrix(NF(J,K)));
// hence both Groebner bases are equal
;
// another example for exterior algebras
ring r;
def E = Exterior();
setring E; E;
slimgb(xy+z);
tst_status(1);$
