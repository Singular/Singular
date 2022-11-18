LIB"tst.lib";
tst_init();
ring r=ZZ,(x,y),dp;
NF(3x,2x); // reduce leading cf.
NF(y2+3x,2x); // reduce non-leading cf
tst_status(1);$
