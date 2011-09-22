LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
poly p = x5-x4+x-3/2;
list l = sturmhaseq(p);
l;
tst_status(1);$
