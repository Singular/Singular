  int ro = 3;
  ring r = 32003,(x,y,z),dp;
  poly f=xyz;
  poly g=z*f;
  ideal i=f,g,g^2;
  matrix m[ro][3] = 1x3y4, 0, i, f ; // a 3 x 3 matrix
  m;
  print(m);
  matrix A;   // the 1 x 1 zero matrix
  matrix C=m; // defines C as a 3 x 3 matrix equal to m
  print(C);
$
