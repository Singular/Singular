LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r=0,(x,y,z),dp;
matrix D[3][3]=0;
D[1,2]=-z;
D[1,3]=2*x;
D[2,3]=-2*y;
def S = nc_algebra(1,D); setring S;
S; // this is U(sl_2)
poly c = 4*x*y+z^2-2*z;
printlevel = 0;
isCentral(c);
poly h = x*c;
printlevel = 1;
isCentral(h);
tst_status(1);$
