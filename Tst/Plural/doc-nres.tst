LIB "tst.lib";
tst_init();
ring A=0,(x,y,z),Dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
system("PLURAL",1,d);
ideal i=x,y,z;
i=std(i);
resolution F=nres(i,0);
F;
for (int a=1;a<=size(list(F));a++)
{ 
printf("Module: %s",a);
print(matrix(F[a]));
}
tst_status(1);$
