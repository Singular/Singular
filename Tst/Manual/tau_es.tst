LIB "tst.lib"; tst_init();
LIB "equising.lib";
ring r=32003,(x,y),ds;
poly f=(x4-y4)^2-x10;
tau_es(f);
tst_status(1);$
