LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
int d =4; // degree bound
def R = makeLetterplaceRing(d);
setring R;
ideal I = x(1)*y(2) + y(1)*z(2), x(1)*x(2) + x(1)*y(2) - y(1)*x(2) - y(1)*y(2);
option(redSB); option(redTail);
ideal J = letplaceGBasis(I);
J;
// ----------------------------------
lp2lstr(J,r); // export an object called @code{@LN} to the ring r
setring r;  // change to the ring r
lst2str(@LN,1); // output the string presentation
tst_status(1);$
