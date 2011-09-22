LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;                // display comments
ring r     = 32003,(x,y,z),ds;
ideal i    = x4+y5+z6,xyz,yx2+xz2+zy7;
is_ci(i);
i          = xy,yz;
is_ci(i);
printlevel = p;
tst_status(1);$
