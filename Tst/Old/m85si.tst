  ring R=0,(x,y,z),dp;
  poly f=x3+y7+z2+xyz;
  ideal i=jacob(f);
  matrix T;
  ideal sm=liftstd(i,T);
  sm;
  print(T);
  matrix(sm)-matrix(i)*T;
$
