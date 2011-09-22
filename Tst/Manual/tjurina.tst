LIB "tst.lib"; tst_init();
LIB "sing.lib";
ring r=32003,(x,y,z),(c,ds);
ideal j=x2+y2+z2,x2+2y2+3z2;
tjurina(j);
tst_status(1);$
