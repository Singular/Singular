LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
int d =5; // degree
def R = makeLetterplaceRing(d);
setring R;
ideal I = y(1)*x(2)*y(3) - z(1)*y(2)*z(3), x(1)*y(2)*x(3) - z(1)*x(2)*y(3), z(1)*x(2)*z(3) - y(1)*z(2)*x(3), x(1)*x(2)*x(3) + y(1)*y(2)*y(3) + z(1)*z(2)*z(3) + x(1)*y(2)*z(3);
ideal J = letplaceGBasis(I); // compute a Letterplace Groebner basis
poly p = y(1)*x(2)*y(3)*z(4)*y(5) - y(1)*z(2)*z(3)*y(4) + z(1)*y(2)*z(3);
poly q = z(1)*x(2)*z(3)*y(4)*z(5) - y(1)*z(2)*x(3)*y(4)*z(5);
lpNF(p,J);
lpNF(q,J);
tst_status(1);$
