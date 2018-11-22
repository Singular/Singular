LIB "tst.lib"; tst_init();
LIB"freegb.lib";
ring r = 0,(x,y,z),dp;
def R = makeLetterplaceRing(10); setring R;
ideal I = y*x*y - z*y*z, x*y*x - z*x*y, z*x*z - y*z*x, x*x*x + y*y*y + z*z*z + x*y*z;
ideal J = std(I);
poly p = y*x*y*z*y - y*z*z*y + z*y*z;
poly q = z*x*z*y*z - y*z*x*y*z;
reduce(p*p,J); // nonzero
reduce(p*p - reduce(p*p,J),J); // zero
reduce(q*q,J); // zero
reduce(q*p,J); // zero
reduce(q*p - p*q,J); // zero
tst_status(1);$
