  ring r=0,(x,y,z),(c,dp);
  simplify(2x,1);
  simplify([4x,2y],1);
  ideal i=x,0,2y,0,3z;
  simplify(i,1+2);
  matrix A[2][3]=x,0,2x,y,0,2y;
  simplify(A,2+8); // by automatic conversion to module
LIB "tst.lib";tst_status(1);$
