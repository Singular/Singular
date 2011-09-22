LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  ideal I=x2-y,x3;
  dim(std(I));
  dim(std(ideal(1)));
tst_status(1);$
