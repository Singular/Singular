LIB "tst.lib"; tst_init();
LIB "kskernel.lib";
int p=5; int q=14;
int i=2; int j=9;
StringF(i,j,p,q);
F;
tst_status(1);$
