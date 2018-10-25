LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
int d =4; // degree bound
def R = makeLetterplaceRing(d);
setring R;
ideal I = x*y + y*z, x*x + x*y - y*x - y*y;
option(redSB); option(redTail);
ideal J = std(I);
J;
// ----------------------------------
lp2lstr(J,r); // export an object called @code{@LN} to the ring r
setring r;  // change to the ring r
lst2str(@LN,1); // output the string presentation
tst_status(1);$
