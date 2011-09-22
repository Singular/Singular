LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring r = 0,(x,y,z),dp;
ideal I=x2+y2+z11;
list L=resolve(I);
list absR=abstractR(L);
absR[1];
absR[2];
tst_status(1);$
