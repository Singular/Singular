LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  factorize(9*(x-1)^2*(y+z));
  factorize(9*(x-1)^2*(y+z),1);
  factorize(9*(x-1)^2*(y+z),2);
  ring rQ=0,x,dp;
  poly f = x2+1;            // irreducible in Q[x]
  factorize(f);
  ring rQi = (0,i),x,dp;
  minpoly = i2+1;
  poly f = x2+1;            // splits into linear factors in Q(i)[x]
  factorize(f);
tst_status(1);$
