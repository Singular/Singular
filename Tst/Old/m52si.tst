  ring r;
  poly f=(x+y)^5;
  matrix M=coeffs(f,y);
  print(M);
  ideal i=f,xyz+z^10*y^7;
  print(coeffs(i,y));
$
