LIB "tst.lib";
tst_init();
ring R=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
def S=nc_algebra(1,d); setring S; //U(sl_2)
ideal I=x3,y3,z3-z;
I=std(I);
I;
resolution resI = mres(I,0); 
resI;
// The matrix A_1 is given by
print(matrix(resI[1]));
// We see that the columns of A_1 generate I.
// The matrix A_2 is given by
print(matrix(resI[2]));
tst_status(1);$
