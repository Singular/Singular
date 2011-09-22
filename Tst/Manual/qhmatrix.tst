LIB "tst.lib"; tst_init();
LIB "spcurve.lib";
ring r=0,(x,y,z),ds;
matrix M[3][2]=z,0,y,x,x^3,y;
qhmatrix(M);
pmat(M);
tst_status(1);$
