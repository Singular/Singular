LIB "tst.lib"; tst_init();
LIB "classify.lib";
intvec v1 = 1,3,5,5,2;
Hcode(v1);
intvec v2 = 1,2,3,4,4,4,4,4,4,4,3,2,1;
Hcode(v2);
tst_status(1);$
