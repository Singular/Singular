LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp(1),dp(2));
int degree_bound = 5;
def R = makeLetterplaceRing(5);
setring R;
ideal I = -x*y-7*y*y+3*x*x, x*y*x-y*x*y;
ideal J = std(I);
J;
tst_status(1);$
