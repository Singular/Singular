LIB "tst.lib";
tst_init();
ring A=0,(x,y,z),Dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
def S=nc_algebra(1,d); setring S;
module m=[x,y],[0,0,z];
print(m*(x+y));
print((x+y)*m);
tst_status(1);$



