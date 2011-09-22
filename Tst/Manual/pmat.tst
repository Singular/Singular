LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring r=0,(x,y,z),ls;
ideal i= x,z+3y,x+y,z;
matrix m[3][3]=i^2;
pmat(m);
pmat(m,5);
tst_status(1);$
