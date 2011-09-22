LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=32003,(x,y,z),lp;
print(xyz*unitmat(4));
print(unitmat(5));
tst_status(1);$
