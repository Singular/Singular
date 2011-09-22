LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
list l = 1,2,3,4,5;
list rev = reverse(l);
l;
rev;
tst_status(1);$
