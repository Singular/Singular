  ring r = 32003, (x,y,z), ds;
  poly s1  = 1x3y2 + 151x5y + 169x2y4 + 151x2yz3 + 186xy6 + 169y9;
  poly s2  = 1x2y2z2 + 3z8;
  poly s3  = 5x4y2 + 4xy5 + 2x2y2z3 + 1y7 + 11x10;
  ideal i = s1, s2, s3;
  // compute standard basis j
  ideal j = std(i);
LIB "tst.lib";tst_status(1);$
