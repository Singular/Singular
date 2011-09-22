LIB "tst.lib"; tst_init();
LIB "classify.lib";
ring r=0,(x,y,z),ds;
export r;
init_debug(1);
poly f=(x2+3y-2z)^2+xyz-(x-y3+x2*z3)^3;
poly g=morsesplit(f);
g;
tst_status(1);$
