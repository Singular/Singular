LIB "tst.lib"; tst_init();
LIB "poly.lib";
ring R = 0, (v,w,x,y,z), lp;
elemSymmId(3);
elemSymmId(nvars(basering));
tst_status(1);$
