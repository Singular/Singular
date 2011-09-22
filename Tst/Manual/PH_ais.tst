LIB "tst.lib"; tst_init();
LIB "phindex.lib";
ring r=0,(x,y,z),ds;
ideal I=x3-3xy2,-y3+3yx2,z3;
PH_ais(I);
tst_status(1);$
