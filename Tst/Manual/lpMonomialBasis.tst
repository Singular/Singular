LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),Dp;
def R = freeAlgebra(r,7); setring R;
ideal J = x*y*x - y*x*y;
option(redSB); option(redTail);
J = letplaceGBasis(J);
J;
lpMonomialBasis(2,1,ideal(0));
lpMonomialBasis(2,0,ideal(0));
lpMonomialBasis(3,1,J);
lpMonomialBasis(3,0,J);
tst_status(1);$
