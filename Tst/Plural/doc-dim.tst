LIB "tst.lib";
tst_init();
ring r=0,(x,y,Dx,Dy),dp;
matrix M[4][4]; M[1,3]=1;M[2,4]=1;
def R = nc_algebra(1,M); // 2nd Weyl algebra
setring R;
dim(std(0)); // the GK dimension of the ring itself
ideal I=x*Dy^2-2*y*Dy^2+2*Dy, Dx^3+3*Dy^2;
dim(std(I)); // the GK dimension of the module R/I 
module T = (x*Dx -2)*gen(1), Dx^3*gen(1), (y*Dy +3)*gen(2);
dim(std(T)); // the GK dimension of the module  R^2/T
tst_status(1);$
