LIB "tst.lib";
tst_init();

ring r=0,(x,y,z),dp;
sres(maxideal(1),10);
qring q=std(xy);
def L=sres(maxideal(1),4);
betti(L);
L;
list(L);
def LL=sres(maxideal(1),0);
betti(LL);
LL;
list(LL);
//
tst_status(1);$
