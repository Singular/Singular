  ring r1=32003,(x,y,z),dp;
  ideal i=std(ideal(x,y));
  qring q=i;
  basering;
  std(ideal(x+y,x+y+z,z));
$
