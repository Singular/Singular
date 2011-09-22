LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring A = 0,x(0..4),dp;
// presentation matrix:
matrix M[2][4] = x(0),x(1),x(2),x(3),x(1),x(2),x(3),x(4);
list L = flatteningStrat(M);
L;
tst_status(1);$
