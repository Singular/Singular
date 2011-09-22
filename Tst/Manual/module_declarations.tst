LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),(c,dp);
  vector s1 = [x2,y3,z];
  vector s2 = [xy,1,0];
  vector s3 = [0,x2-y2,z];
  poly   f  = xyz;
  module m = s1, s2-s1,f*(s3-s1);
  m;
  // show m in matrix format (columns generate m)
  print(m);
tst_status(1);$
