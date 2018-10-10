LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal G = x*x, y*y,x*y*x;
// G contains a Groebner basis
sickle(G,1,1,1); // computes mistletoes, K-dimension and the Hilbert series
sickle(G,1,0,0); // computes mistletoes only
sickle(G,0,1,0); // computes K-dimension only
sickle(G,0,0,1); // computes Hilbert series only
tst_status(1);$
