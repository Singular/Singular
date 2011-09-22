LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),dp;
  char(r);
  ring s=0,(x,y),dp;
  char(s);
  ring ra=(7,a),(x,y),dp;
  minpoly=a^3+a+1;
  char(ra);
  ring rp=(49,a),(x,y),dp;
  char(rp);
  ring rr=real,x,dp;
  char(rr);
tst_status(1);$
