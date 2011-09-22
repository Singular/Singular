LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring   r = (0,q),(x,y,z),dp;
matrix C = UpOneMatrix(3);
C[1,3]   = q;
print(C);
def S = nc_algebra(C,0); setring S;
S;
tst_status(1);$
