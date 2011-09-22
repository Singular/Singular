LIB "tst.lib"; tst_init();
  ring r=0,(x,y),dp;
  poly f= x3y2 + 2x2y2 +2;
  matrix H = jacob(jacob(f));    // the Hessian of f
  matrix mc = coef(f,y);
  print(mc);
  module MD = [x+y,1,x],[x+y,0,y];
  matrix M = MD;
  print(M);
tst_status(1);$
