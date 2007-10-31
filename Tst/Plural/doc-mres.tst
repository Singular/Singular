LIB "tst.lib";
tst_init();
ring A=0,(x,y,z),Dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
def S=nc_algebra(1,d); setring S;
ideal i=x,y,z;
i=std(i);
resolution M=mres(i,0);
M;
for (int a=1;a<=size(list(M));a++)
{ 
printf("Module: %s",a);
print(matrix(M[a]));
}
tst_status(1);$
