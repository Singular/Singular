  ring r=32003,(x,y,z),(c,ds);
  poly f=x2+y+z3;
  vector v=[x^10,f];
  ideal i=f,z;
  module m=v,[0,0,1+x];
  lead(f);
  lead(v);
  lead(i);
  lead(m);
$
