  ring r=32003,(x,y,z),(c,dp);
  jet(1+x+x2+x3+x4,3);
  poly f=1+x+y+z+x2+xy+xz+y2+x3+y3+z4;
  jet(f,3);
  intvec iv=2,1,1;
  jet(f,3,iv);
  // the part of f with (total) degree >3:
  f-jet(f,3);
  // the homogeneous part of f of degree 2:
  jet(f,2)-jet(f,1);
  // the absolute term of f:
  jet(f,0);
  // now for other types:
  ideal i=f,x,f*f;
  jet(i,2);
  vector v=[f,1,x];
  jet(v,1);
  jet(v,0);
  v=[f,1,0];
  module m=v,v,[1,x2,z3,0,1];
  jet(m,2);
LIB "tst.lib";tst_status(1);$
