  ring r= 0,(x,y,z),dp;
  poly f=2*x^3*y+3*z^5;
  diff(f,x);
  diff(f,z);
  vector v=[f,y2+z];
  diff(v,z);
  ideal i=x2,x2+yz,xyz;
  ideal j=x2-yz,xyz;
  print(diff(i,j));
LIB "tst.lib";tst_status(1);$
