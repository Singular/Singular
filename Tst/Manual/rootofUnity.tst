LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r = (0,q),(x,y,z),dp;
rootofUnity(6);
rootofUnity(7);
minpoly = rootofUnity(8);
r;
tst_status(1);$
