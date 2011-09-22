LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring R=0,(x,y),ds;
list sp=list(ideal(-1/2,-3/10,-3/10,-1/10,-1/10,0,1/10,1/10,3/10,3/10,1/2));
spprint(spnf(sp));
tst_status(1);$
