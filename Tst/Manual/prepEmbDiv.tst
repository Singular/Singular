LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring R=0,(x,y,z),dp;
ideal I=x2+y2+z11;
list L=resolve(I);
prepEmbDiv(L);
tst_status(1);$
