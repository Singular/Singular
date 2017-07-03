LIB "tst.lib";
tst_init();

// output was: (4a2+2): multiplied twice with den.
ring R=(0,a),(x,y),dp;
minpoly = 1/2+a^2;
minpoly;

tst_status(1);$
