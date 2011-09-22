LIB "tst.lib"; tst_init();
  ring r1=32003,(x,y),dp;
  ideal i=x5+xy4,x3+x2y+xy2+y3;
  resolution rs=lres(i,0);
  rs;
  list(rs);
  minres(rs);
  list(rs);
tst_status(1);$
