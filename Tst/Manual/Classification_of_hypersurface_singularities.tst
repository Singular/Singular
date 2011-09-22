LIB "tst.lib"; tst_init();
  LIB "classify.lib";
  ring r=0,(x,y,z),ds;
  poly p=singularity("E[6k+2]",2)[1];
  p=p+z^2;
  p;
  // We received an E_14 singularity in normal form
  // from the database of normal forms. Since only the residual
  // part is saved in the database, we added z^2 to get an E_14
  // of embedding dimension 3.
  //
  // Now we apply a coordinate change in order to deal with a
  // singularity which is not in normal form:
  map phi=r,x+y,y+z,x;
  poly q=phi(p);
  // Yes, q really looks ugly, now:
  q;
  // Classification
  classify(q);
  // The library also provides routines to determine the corank of q
  // and its residual part without going through the whole
  // classification algorithm.
  corank(q);
  morsesplit(q);
tst_status(1);$
