LIB "tst.lib"; tst_init();
  ring r=2,(x,y,z),lp;
  poly s1=z*(x*y+1);
  poly s2=x2+x;
  poly s3=y2+y;
  ideal i=s1,s2,s3;
  slimgb(i);
tst_status(1);$
