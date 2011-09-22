LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = (0,q1,q2),(x,y,z),dp;
matrix C[3][3];
C[1,2]=q2; C[1,3]=q1; C[2,3]=1;
matrix D[3][3];
D[1,2]=x; D[1,3]=z;
def S = nc_algebra(C,D); setring S;
S;
ideal j=ndcond(); // the silent version
j;
printlevel=1;
ideal i=ndcond(); // the verbose version
i;
tst_status(1);$
