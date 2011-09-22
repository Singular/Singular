LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r;
poly f=x3+y5+z2;
intvec w=10,6,15;
qhspectrum(f,w);
// the spectrum numbers are:
// 1/30,7/30,11/30,13/30,17/30,19/30,23/30,29/30
tst_status(1);$
