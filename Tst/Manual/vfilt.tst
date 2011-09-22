LIB "tst.lib"; tst_init();
LIB "gmssing.lib";
ring R=0,(x,y),ds;
poly t=x5+x2y2+y5;
vfilt(t);
tst_status(1);$
