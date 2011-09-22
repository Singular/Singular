LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r  = 32003,(x,y,z),ds;
ideal i = x5+y6+z6,x2+2y2+3z2;
dim_slocus(i);
tst_status(1);$
