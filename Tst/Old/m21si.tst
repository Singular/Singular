  ring r0=32003,x,dp;
  number n=2/3; n;
  ring r1=0,x,dp;
  number n=2/3; n;
  ring r2=(0,a),(x,y,z),dp;
  number n=2/3; n;
  n=a+a^2;
  poly f=n*x+2;f;
LIB "tst.lib";tst_status(1);$
