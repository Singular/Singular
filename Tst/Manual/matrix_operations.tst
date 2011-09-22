LIB "tst.lib"; tst_init();
  ring r=32003,x,dp;
  matrix A[3][3] = 1,3,2,5,0,3,2,4,5; // define a matrix
  print(A); // nice printing of small matrices
  A[2,3];   // matrix entry
  A[2,3] = A[2,3] + 1; // change entry
  A[2,1..3] = 1,2,3;   // change 2nd row
  print(A);
  matrix E[3][3]; E = E + 1;  // the unit matrix
  matrix B =x*E - A;
  print(B);
  // the same (but x-A does not work):
  B = -A+x;
  print(B);
  det(B);        // the characteristic polynomial of A
  A*A*A - 8 * A*A - 2*A == E;  // Cayley-Hamilton
  vector v =[x,-1,x2];
  A*v; // multiplication of matrix and vector
  matrix m[2][2]=1,2,3;
  print(m-transpose(m));
tst_status(1);$
