LIB "tst.lib"; tst_init();
ring R=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;  d[1,3]=2x;  d[2,3]=-2y;
def U=nc_algebra(1,d); // this algebra is U(sl_2)
setring U;
option(redSB); option(redTail);
ideal I=x3,y3,z3-z;
I=std(I);
I;
resolution resI = mres(I,0);
resI;
list l = resI;
// The matrix A_1 is given by
print(matrix(l[1]));
// We see that the columns of A_1 generate I.
// The matrix A_2 is given by
print(matrix(l[2]));
ideal tst; // now let us show that the resolution is exact
matrix TST;
TST = transpose(l[3])*transpose(l[2]); // 2nd term
size(ideal(TST));
TST = transpose(l[2])*transpose(l[1]); // 1st term
size(ideal(TST));
tst_status(1);$
