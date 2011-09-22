LIB "tst.lib"; tst_init();
LIB "curvepar.lib";
ring r=0,(t),ds;
list X=t4,t5+t11,t9+2*t7;
list L=WSemigroup(X,30);
L;
tst_status(1);$
