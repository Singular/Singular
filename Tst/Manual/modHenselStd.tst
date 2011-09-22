LIB "tst.lib"; tst_init();
LIB "modstd.lib";
ring r = 0,(x,y,z),dp;
ideal I = 3x3+x2+1,11y5+y3+2,5z4+z2+4;
ideal J = modHenselStd(I);
J;
tst_status(1);$
