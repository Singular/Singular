LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r  = 32003,(x,y,z),ds;
ideal i = x4+y5+z6,xyz,yx2+xz2+zy7;
list L  = nres(i,0);
is_complex(L);
L[4]    = matrix(i);
is_complex(L);
tst_status(1);$
