  string s="hello";
  size(s);
  intvec iv=1,2;
  size(iv);
  ring r=0,(x,y,z),lp;
  poly f=x+y+z;
  size(f);
  vector v=[x+y,0,1];
  size(v);
  ideal i=f,y;
  size(i);
  module m=v,[0,1],[0,0,1],2*v;
  size(m);
  matrix mm[2][2];
  size(mm);
$
