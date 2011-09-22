LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 2;
ring r     = 32003,(x,y,z),ds;
ideal j    = x5+y6+z6,x2+2y2+3z2,xyz+yx;
milnor(j);
poly f     = x7+y7+(x-y)^2*x2y2+z2;
milnor(f);
printlevel = p;
tst_status(1);$
