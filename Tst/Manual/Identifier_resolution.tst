LIB "tst.lib"; tst_init();
ring r=0,(x,y),dp;
int x;
x*y; // resolved product int*poly, i.e., 0*y
xy; // "xy" is one identifier and resolved to monomial xy
tst_status(1);$
