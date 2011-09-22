LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4),x(5..7),y(8)),dp;
identifyvar();
tst_status(1);$
