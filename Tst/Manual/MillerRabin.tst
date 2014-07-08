LIB "tst.lib"; tst_init();
LIB "crypto.lib";
bigint x=2;
x=x^787-1;
MillerRabin(x,3);
tst_status(1);$
