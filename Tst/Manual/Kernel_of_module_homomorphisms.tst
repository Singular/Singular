LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),(c,dp);
  matrix A[2][2]=x,y,z,1;
  matrix B[2][2]=x2,y2,z2,xz;
  print(B);
  def C=modulo(A,B);
  print(C);            // matrix of generators for the kernel
  print(A*matrix(C));  // should be in Im(B)
tst_status(1);$
