LIB "tst.lib"; tst_init();
  LIB "primdec.lib";
  ring r = 0,(a,b,c,d,e,f),dp;
  ideal i= f3, ef2, e2f, bcf-adf, de+cf, be+af, e3;
  primdecGTZ(i);
  // We consider now the ideal J of the base space of the
  // miniversal deformation of the cone over the rational
  // normal curve computed in section *8* and compute
  // its primary decomposition.
  ring R = 0,(A,B,C,D),dp;
  ideal J = CD, BD+D2, AD;
  primdecGTZ(J);
  // We see that there are two components which are both
  // prime, even linear subspaces, one 3-dimensional,
  // the other 1-dimensional.
  // (This is Pinkhams example and was the first known
  // surface singularity with two components of
  // different dimensions)
  //
  // Let us now produce an embedded component in the last
  // example, compute the minimal associated primes and
  // the radical. We use the Characteristic set methods
  // from primdec.lib.
  J = intersect(J,maxideal(3));
  // The following shows that the maximal ideal defines an embedded
  // (prime) component.
  primdecSY(J);
  minAssChar(J);
  radical(J);
tst_status(1);$
