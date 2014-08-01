LIB "tst.lib"; tst_init();
LIB "crypto.lib";
bigintmat M[3][3]=1,2,3,4,5,6,7,6,5;
solutionsMod2(M);
tst_status(1);$
