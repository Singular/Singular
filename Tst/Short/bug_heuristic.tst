LIB "tst.lib";
tst_init();

// time to factorize( LucksWangSparseHeuristic)
ring r2=2,(x,y,z),dp;
factorize((x+y+z)^8+(x+y+z));
factorize((x+y+z)^16+(x+y+z));
ring r0=0,(x,y,z),dp;
factorize((x+y+z)^8+(x+y+z));
factorize((x+y+z)^16+(x+y+z));
ring r3=3,(x,y,z),dp;
factorize((x+y+z)^8+(x+y+z));
factorize((x+y+z)^16+(x+y+z));
ring r101=101,(x,y,z),dp;
factorize((x+y+z)^8+(x+y+z));

tst_status(1);$

