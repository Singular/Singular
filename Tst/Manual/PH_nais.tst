LIB "tst.lib"; tst_init();
LIB "phindex.lib";
ring r=0,(x,y,z),ds;
ideal I=x5-2x3y2-3xy4+x3z2-3xy2z2,-3x4y-2x2y3+y5-3x2yz2+y3z2,x2z3+y2z3+z5;
PH_nais(I);
tst_status(1);$
