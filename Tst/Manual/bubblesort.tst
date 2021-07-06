LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring r = 0,x,dp;
list L=-567,-233,446,12,-34,8907;
bubblesort(L);
tst_status(1);$
