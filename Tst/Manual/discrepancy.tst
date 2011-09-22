LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring R=0,(x,y,z),dp;
ideal I=x2+y2+z3;
list re=resolve(I);
discrepancy(re);
tst_status(1);$
