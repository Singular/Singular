LIB "tst.lib";
tst_init();

// rad_con in local orderings

LIB"polylib.lib";
ring r=0,(x,y),ds;
ideal i=(1+x)*y2;
rad_con(y,i);
// should be 1

tst_status(1);$
