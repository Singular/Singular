  ring r;
  ideal m=3x2+yz,7y6+2x2y+5xz;
  poly f=y7+x3+xyz+z2;
  ideal i=jacob(f);
  matrix T=lift(i,m);
  matrix(m)-matrix(i)*T;
LIB "tst.lib";tst_status(1);$
