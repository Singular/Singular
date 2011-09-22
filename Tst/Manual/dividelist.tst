LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list L1=1,2,3;
list L2=5,9,7;
dividelist(L1,L2);
tst_status(1);$
