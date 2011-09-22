LIB "tst.lib"; tst_init();
LIB "surfacesignature.lib";
ring R = 0,x,dp;
signatureBrieskorn(11,3,5);
tst_status(1);$
