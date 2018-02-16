LIB "tst.lib";
tst_init();

// wrong test for sca
LIB "nctools.lib";
ring A = 0,(a,b),dp;
def S = nc_algebra(-1,0);
setring S;
b^2==0;
ideal I = a^2;
qring R = twostd(I);
setring R;
b^2==0; // should not be true
b*b,b^2;

tst_status(1);$
