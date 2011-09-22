LIB "tst.lib"; tst_init();
  // the following expressions are in any ring int expressions
  2 / 3;
  4/ 8;
  2 /2;   // the notation of / for div might change in the future
  ring r0=0,x,dp;
  2/3, 4/8, 2/2 ; // are numbers

  poly f = 2x2 +1;
  leadcoef(f);
  typeof(_);
  ring rr =real,x,dp;
  1.7e-2; 1.7e+2; // are valid (but  1.7e2 not), if the field is `real`
  ring rp = (31,t),x,dp;
  2/3, 4/8, 2/2 ; // are numbers
  poly g = (3t2 +1)*x2 +1;
  leadcoef(g);
  typeof(_);
  par(1);
  typeof(_);
tst_status(1);$
