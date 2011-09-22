LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,x,dp;
ideal I = x,2x;
linSyzSolve(I);
ideal J = x,x2;
linSyzSolve(J);
tst_status(1);$
