LIB "tst.lib"; tst_init();
ring R=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;  d[1,3]=2x;  d[2,3]=-2y;
def RS=nc_algebra(1,d); //U(sl_2)
setring RS;
option(redSB); option(redTail);
ideal I=x3,y3,z3-z;
I=std(I);
I;
vdim(I);

tst_status(1);$
