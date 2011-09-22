LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring r;
matrix n[2][3]=x,y,5,z,77,33;
print(kohom(n,3));
tst_status(1);$
