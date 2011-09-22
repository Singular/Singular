LIB "tst.lib"; tst_init();
LIB "gkdim.lib";
ring R = 0,(x,y,z),Dp;
matrix C[3][3]=0,1,1,0,0,-1,0,0,0;
matrix D[3][3]=0,0,0,0,0,x;
def r = nc_algebra(C,D); setring r;
r;
ideal I=x;
GKdim(I);
ideal J=x2,y;
GKdim(J);
module M=[x2,y,1],[x,y2,0];
GKdim(M);
ideal A = x,y,z;
GKdim(A);
ideal B = 1;
GKdim(B);
GKdim(ideal(0)) == nvars(basering);  // should be true, i.e., evaluated to 1
tst_status(1);$
