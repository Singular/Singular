LIB "tst.lib"; tst_init();
  chinrem(intvec(2,-3),intvec(7,11));
  ring r=0,(x,y),dp;
  ideal i1=5x+2y,x2+3y2+xy;
  ideal i2=2x-3y,2x2+4y2+5xy;
  chinrem(list(i1,i2),intvec(7,11));
  chinrem(list(i1,i2),list(bigint(7),bigint(11)));
tst_status(1);$
