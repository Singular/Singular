LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = 0,(x,y),dp;
isCommutative();
def D = Weyl(); setring D;
isCommutative();
setring r;
def R = nc_algebra(1,0); setring R;
isCommutative();
tst_status(1);$
