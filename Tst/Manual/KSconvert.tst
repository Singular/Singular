LIB "tst.lib"; tst_init();
LIB "kskernel.lib";
int p=6;
int q=7;
KSker(p,q);
setring KSring;
KSconvert(KSkernel);
setring KSring2;
print(KSkernel2);
tst_status(1);$
