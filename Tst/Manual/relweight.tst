LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=32003,(x,y,z),ds;
matrix N[2][3]=z,0,y,x,x^3,y;
intmat W[2][3]=1,1,1,1,1,1;
intvec a=1,1,1;
relweight(N,W,a);
tst_status(1);$
