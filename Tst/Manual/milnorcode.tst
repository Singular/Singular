LIB "tst.lib"; tst_init();
LIB "classify.lib";
ring r=0,(x,y,z),ds;
poly f=x2y+y3+z2;
milnorcode(f);
milnorcode(f,2);  // a big second argument may result in memory overflow
tst_status(1);$
