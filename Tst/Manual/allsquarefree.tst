LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring exring=7,(x,y),dp;
poly f=(x+y)^7*(x-y)^8;
poly g=squarefree(f);
g;                      // factor x+y lost, since characteristic=7
allsquarefree(f,g);     // all factors (x+y)*(x-y) found
tst_status(1);$
