LIB "tst.lib";
tst_init();

==========================  example 1.9.4   ================================
ring s  = (0,Q),(x,y,z),dp;
matrix C[3][3];
C[1,2] = Q2;
C[1,3] = 1/Q2;
C[2,3] = Q2;
matrix D[3][3];
D[1,2] = -Q*z;
D[1,3] = 1/Q*y;
D[2,3] = -Q*x;
ncalgebra(C,D);
s;

tst_status(1);$

