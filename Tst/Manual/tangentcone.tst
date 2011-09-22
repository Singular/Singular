LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring R = 0,(x,y,z),ds;
ideal i  = 7xyz+z5,x2+y3+z7,5z5+y5;
tangentcone(i);
tst_status(1);$
