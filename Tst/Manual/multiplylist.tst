LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list L=1,2,3;
multiplylist(L,1/5);
tst_status(1);$
