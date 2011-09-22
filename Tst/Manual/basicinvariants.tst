LIB "tst.lib"; tst_init();
LIB "classify.lib";
ring r=0,(x,y,z),ds;
basicinvariants((x2+3y-2z)^2+xyz-(x-y3+x2*z3)^3);
tst_status(1);$
