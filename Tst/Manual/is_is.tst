LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 32003,(x,y,z),ds;
ideal i    = x2y,x4+y5+z6,yx2+xz2+zy7;
is_is(i);
poly f     = xy+yz;
is_is(f);
printlevel = p;
tst_status(1);$
