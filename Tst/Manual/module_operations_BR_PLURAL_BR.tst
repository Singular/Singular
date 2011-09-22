LIB "tst.lib"; tst_init();
ring A=0,(x,y,z),Dp;
matrix D[3][3];
D[1,2]=-z;  D[1,3]=y;  D[2,3]=x;  // this algebra is U(so_3)
def B=nc_algebra(1,D);
setring B;
module M = [x,y],[0,0,x*z];
module N = matrix((x+y-z)*M) - matrix(M*(x+y-z)); // no - for type module
print(N);
tst_status(1);$
