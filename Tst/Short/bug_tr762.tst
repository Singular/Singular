LIB "tst.lib";
tst_init();

// hres(ideal(1),0):

ring r = 0, x, dp;
ideal I = 0,1;
hres(I,0);
list(hres(I,0));

tst_status(1);$
