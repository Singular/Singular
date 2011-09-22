LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),(Dp(2),dp(1));
list L = ringlist(r);
list N = convloc(L);
def rs = ring(N);
setring rs;
rs;
tst_status(1);$
