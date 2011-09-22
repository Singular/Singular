LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),(c,dp);
ideal i=0,2x,2x,4x,3x+y,5x2;
simplify(i,1);
simplify(i,2);
simplify(i,4);
simplify(i,8);
simplify(i,16);
simplify(i,32);
simplify(i,32+2+1);
matrix A[2][3]=x,0,2x,y,0,2y;
simplify(A,2+8); // by automatic conversion to module
tst_status(1);$
