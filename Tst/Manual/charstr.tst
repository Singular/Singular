LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),dp;
  charstr(r);
  ring s=0,(x,y),dp;
  charstr(s);
  ring ra=(7,a),(x,y),dp;
  minpoly=a^3+a+1;
  charstr(ra);
  ring rp=(49,a),(x,y),dp;
  charstr(rp);
  ring rr=real,x,dp;
  charstr(rr);
tst_status(1);$
