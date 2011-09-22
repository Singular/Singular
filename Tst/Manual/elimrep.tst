LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list L=4,5,2,5,7,8,6,3,2;
elimrep(L);
tst_status(1);$
