LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
int d =5; // degree
def R = makeLetterplaceRing(d);
setring R;
ideal I = y*x*y - z*y*z, x*y*x - z*x*y, z*x*z - y*z*x, x*x*x + y*y*y + z*z*z + x*y*z;
ideal J = std(I); // compute a Letterplace Groebner basis
poly p = y*x*y*z*y - y*z*z*y + z*y*z;
poly q = z*x*z*y*z - y*z*x*y*z;
lpNF(p,J);
reduce(p,J);
lpNF(q,J);
reduce(q,J);
tst_status(1);$
