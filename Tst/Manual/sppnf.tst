LIB "tst.lib"; tst_init();
LIB "gmssing.lib";
ring R=0,(x,y),ds;
list sp=list(ideal(-1/2,-3/10,-3/10,-1/10,-1/10,0,1/10,1/10,3/10,3/10,1/2),
intvec(2,1,1,1,1,1,1,1,1,1,0));
sppprint(sppnf(sp));
tst_status(1);$
