  ring r=0,(x,y,z),dp;
  poly f=x2+y2+z2+x+y+z;
  subst(f,x,3/2);
  int a=1;
  subst(f,y,a);
  subst(f,y,z);
$
