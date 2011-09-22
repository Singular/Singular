LIB "tst.lib"; tst_init();
LIB "crypto.lib";
list L=primList(100);
size(L);
L[size(L)];
tst_status(1);$
