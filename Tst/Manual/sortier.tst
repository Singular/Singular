LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring q=0,(x,y,z,u,v,w),dp;
ideal i=w,x,z,y,v;
sortier(i);
tst_status(1);$
