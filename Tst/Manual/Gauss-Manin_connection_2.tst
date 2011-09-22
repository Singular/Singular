LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),ds;
LIB "spectrum.lib";
poly f=x^7+y^7+z^7;
list s1=spectrumnd( f );
s1;
tst_status(1);$
