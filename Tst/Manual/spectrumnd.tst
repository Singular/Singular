LIB "tst.lib"; tst_init();
LIB "spectrum.lib";
ring R=0,(x,y),ds;
poly f=x^31+x^6*y^7+x^2*y^12+x^13*y^2+y^29;
list s=spectrumnd(f);
size(s[1]);
s[1][22];
s[2][22];
tst_status(1);$
