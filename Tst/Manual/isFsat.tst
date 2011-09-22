LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),dp;
poly G = x*(x-y)*y;
def A = annfs(G);
setring A;
poly F = x3-y2;
isFsat(LD,F);
ideal J = LD*F;
isFsat(J,F);
tst_status(1);$
