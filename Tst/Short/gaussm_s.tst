LIB "tst.lib";
tst_init();

LIB "gaussman.lib";

ring R=0,(x,y),ds;
poly f=x5+x2y2+y5;

tst_gaussm(f);

tst_status(1);
$
