LIB "tst.lib"; tst_init();
LIB "random.lib";
ring r=0,(x,y,z),dp;
randomid(maxideal(2),2,9);
module m=[x,0,1],[0,y2,0],[y,0,z3];
show(randomid(m));
tst_status(1);$
