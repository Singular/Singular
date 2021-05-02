LIB "tst.lib";
tst_init();

LIB"sing.lib";
ring r = 0,(x,y),dp;
ideal i1 = x;
ideal i2 = x2, xy;
codim(std(i1),std(i2));

tst_status(1);$
