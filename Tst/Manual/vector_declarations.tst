LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),(c,dp);
  poly s1 = x2;
  poly s2 = y3;
  poly s3 = z;
  vector v = [s1, s2-s1, s3-s1]+ s1*gen(5);
  // v is a vector in the free module of rank 5
  v;
tst_status(1);$
