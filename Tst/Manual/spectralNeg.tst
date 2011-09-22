LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring R=0,(x,y,z),dp;
ideal I=x3+y4+z5;
list L=resolve(I,"K");
spectralNeg(L);
LIB"gmssing.lib";
ring r=0,(x,y,z),ds;
poly f=x3+y4+z5;
spectrum(f);
tst_status(1);$
