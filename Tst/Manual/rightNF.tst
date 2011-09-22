LIB "tst.lib"; tst_init();
LIB "nctools.lib";
LIB "ncalg.lib";
ring r = 0,(x,d),dp;
def S = nc_algebra(1,1); setring S; // Weyl algebra
ideal I = x; I = std(I);
poly  p = x*d+1;
NF(p,I); // left normal form
rightNF(p,I); // right normal form
tst_status(1);$
