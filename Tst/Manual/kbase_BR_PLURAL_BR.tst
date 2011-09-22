LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;  d[1,3]=2x;  d[2,3]=-2y;
def R=nc_algebra(1,d); // this algebra is U(sl_2)
setring R;
ideal i=x2,y2,z2-1;
i=std(i);
print(matrix(i));  // print a compact presentation of i
kbase(i);
vdim(i);
ideal j=x,z-1;
j=std(j);
kbase(j,3);
tst_status(1);$
