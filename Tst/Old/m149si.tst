  ring R=32003,(x,y,z),ds;
  ideal i=x2+y^12,y13;
  std(i);
  noether = x^11;
  std(i);
  noether = 0;//disables noether
$
