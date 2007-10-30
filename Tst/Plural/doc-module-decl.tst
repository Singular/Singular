LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),(c,dp);
matrix D[3][3];
D[1,2]=-z;
D[1,3]=y;
D[2,3]=x;
def S=ncalgebra(1,D); setring S;
// it is U(so_3)
vector s1 = [x2,y3,z];
vector s2 = [xy,1,0];
vector s3 = [0,x2-y2,z];
poly   f  = xyz;
module m = s1, s2-s1,f*(s3-s1);
m;
// show m in matrix format (columns generate m)
print(m);
tst_status(1);$
