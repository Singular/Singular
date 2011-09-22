LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z),dp;
poly f = x^3+y^3+z^3;
def A = deRhamCohom(f); // we see that the K-dim is 2
setring A;
DR;
tst_status(1);$
