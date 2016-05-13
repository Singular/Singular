LIB "tst.lib";
tst_init();

// rsum with rational functions

LIB "ncfactor.lib";
ring R = (0,q),(x,d),dp;
def r = nc_algebra(1/q,1);
setring r;
ring S = (0,q),(a(1..2),b(1..3)),dp;
def W = r + S;
setring W;
W;
ringlist(W);

tst_status(1);$
