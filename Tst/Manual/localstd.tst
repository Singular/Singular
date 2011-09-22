LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring R = 0,(x,y,z),ds;
ideal i  = xyz+z5,2x2+y3+z7,3z5+y5;
localstd(i);
tst_status(1);$
