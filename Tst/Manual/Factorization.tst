LIB "tst.lib"; tst_init();
  ring r = 0,(x,y),dp;
  poly f = 9x16-18x13y2-9x12y3+9x10y4-18x11y2+36x8y4
         +18x7y5-18x5y6+9x6y4-18x3y6-9x2y7+9y8;
  // = 9 * (x5-1y2)^2 * (x6-2x3y2-1x2y3+y4)
  factorize(f);
  // returns factors and multiplicities,
  // first factor is a constant.
  poly g = (y4+x8)*(x2+y2);
  factorize(g);
  // The same in characteristic 2:
  ring s = 2,(x,y),dp;
  poly g = (y4+x8)*(x2+y2);
  factorize(g);
  // factorization over algebraic extension fields
  ring rext = (0,i),(x,y),dp;
  minpoly = i2+1;
  poly g = (y4+x8)*(x2+y2);
  factorize(g);
tst_status(1);$
