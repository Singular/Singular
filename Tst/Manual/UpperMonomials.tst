LIB "tst.lib"; tst_init();
LIB "qhmoduli.lib";
ring B   = 0,(x,y,z), ls;
poly f = -z5+y5+x2z+x2y;
UpperMonomials(f);
tst_status(1);$
