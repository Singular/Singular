LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,x,dp;
list l = list(1,2,3,4,5);
poly p = univarpoly(l);
p;
tst_status(1);$
