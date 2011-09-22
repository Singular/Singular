LIB "tst.lib"; tst_init();
LIB "gmssing.lib";
ring R=0,(x,y),ds;
list sp=list(ideal(-1/2,-3/10,-1/10,0,1/10,3/10,1/2),intvec(1,2,2,1,2,2,1));
spprint(sp);
spprint(spmul(sp,2));
list sp1=list(ideal(-1/6,1/6),intvec(1,1));
spprint(sp1);
list sp2=list(ideal(-1/3,0,1/3),intvec(1,2,1));
spprint(sp2);
spprint(spmul(list(sp1,sp2),intvec(1,2)));
tst_status(1);$
