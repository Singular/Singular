LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,x,dp;
poly F = x;
poly2list(F);
ring r2 = 0,(x,y),dp;
poly F = x2y+5xy2;
poly2list(F);
poly2list(0);
tst_status(1);$
