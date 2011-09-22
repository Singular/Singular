LIB "tst.lib"; tst_init();
LIB "modstd.lib";
list L = 3,5,11,13,181,32003;
ring r = 0,(x,y,z,t),dp;
ideal I = 3x3+x2+1,11y5+y3+2,5z4+z2+4;
I = homog(I,t);
ideal J = modS(I,L);
J;
tst_status(1);$
