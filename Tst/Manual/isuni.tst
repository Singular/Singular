LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,(x,y),dp;
poly p = 6x7-3x2+2x-15/7;
isuni(p);
isuni(p*y);
tst_status(1);$
