LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=32003,(x,y,z),ds;
ideal i=xy,xz,yz;
CMtype(i);
tst_status(1);$
