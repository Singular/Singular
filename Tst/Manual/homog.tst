LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),ds;
  poly s1=x3y2+x5y+3y9;
  poly s2=x2y2z2+3z8;
  poly s3=5x4y2+4xy5+2x2y2z3+y7+11x10;
  ideal i=s1,s2,s3;
  homog(s2,z);
  homog(i,z);
  homog(i);
  homog(homog(i,z));
tst_status(1);$
