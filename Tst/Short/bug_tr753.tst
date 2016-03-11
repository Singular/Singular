LIB "tst.lib";
tst_init();

// non-commutative imap

ring R = 0,(x,z),dp;
def r = nc_algebra(1,-1);
setring r;
basering;
poly h = x3z2-2x2z+x2;
h;
ring R2 = 0,(z,x),dp;
def r2 = nc_algebra(1,1);
setring r2;
basering;
poly h = imap(r,h);
h;
map M=r,x,z;
M(h);
M(h)-h; // 0

tst_status(1);$
