LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=0,(x,y,z,w),dp;
ideal i = x3+y2+yw2,xz+z2,xyz-w2;
sortvars(i,0,xy,1,zw);
tst_status(1);$
