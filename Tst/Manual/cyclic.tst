LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=0,(u,v,w,x,y,z),lp;
cyclic(nvars(basering));
homog(cyclic(5),z);
tst_status(1);$
