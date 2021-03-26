LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring R=0,(x,y,z),dp;
ideal I=x2+y2,z2;
poly f=x4+y4;
rad_con(f,I);
ideal J=x2+y2,z2,x4+y4;
poly g=z;
rad_con(g,I);
tst_status(1);$
