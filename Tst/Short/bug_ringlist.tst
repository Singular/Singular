LIB "tst.lib";
tst_init();

// ringlist with real/complex of given length
ring r=(real,50),(x,y,z),dp;
r;
def s=ring(ringlist(r));
s;
ring R=(complex,500,i),(x,y,z),dp;
R;
def S=ring(ringlist(R));
S;

tst_status(1);$
