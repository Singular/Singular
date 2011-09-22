LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  poly f=2x3y+3z5;
  diff(f,x);
  vector v=[f,y2+z];
  diff(v,z);
  ideal j=x2-yz,xyz;
  ideal i=x2,x2+yz,xyz;
  // corresponds to differential operators
  // d2/dx2, d2/dx2+d2/dydz, d3/dxdydz:
  print(diff(i,j));
tst_status(1);$
