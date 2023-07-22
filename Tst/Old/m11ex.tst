  LIB "elim.lib";
  LIB "sing.lib";
  // Affine polar curve:
  ring R = 0,(x,z,t),dp;              // global ordering dp
  poly f = z5+xz3+x2-tz6;
  dim_slocus(f);                      // dimension of singular locus
  ideal j = diff(f,x),diff(f,z);
  dim(std(j));                        // dim V(j)
  dim(std(j+ideal(f)));               // V(j,f) also 1-dimensional
  // j defines a curve, but to get the polar curve we must remove the
  // branches contained in f=0 (they exist since dim V(j,f) = 1). This
  // gives the polar curve set theoretically. But for the structure we
  // may take either j:f or j:f^k for k sufficiently large. The first is
  // just the ideal quotient, the second the iterated ideal quotient
  // or saturation. In our case both is the same.
  ideal q = quotient(j,ideal(f));     // ideal quotient
  ideal qsat = sat(j,f);              // saturation, proc from elim.lib
  ideal sq = std(q);
  dim(sq);
  // 1-dimensional, hence
  // q defines the affine polar curve
  //
  // to check that q and qsat are the same, we show both inclusions, i.e.
  // both reductions must give the 0-ideal
  size(reduce(qsat,sq));
  size(reduce(q,std(qsat)));
  qsat;
  // We see that the affine polar curve does not pass through the origin,
  // hence we expect the local polar "curve" to be empty
  // ------------------------------------------------
  // Local polar curve:
  ring r = 0,(x,z,t),ds;              // local ordering ds
  poly f = z5+xz3+x2-tz6;
  dim_slocus(f);                      // dimension of singular locus
  ideal j = diff(f,x),diff(f,z);
  dim(std(j));                        // V(j) 1-dimensional
  dim(std(j+ideal(f)));               // V(j,f) also 1-dimensional
  ideal q = quotient(j,ideal(f));     // ideal quotient
  q;
  // The local polar "curve" is empty, i.e V(j) is contained in V(f)
  // ------------------------------------------------
  // Projective polar curve: (we need "sing.lib" and "elim.lib")
  ring P = 0,(x,z,t,y),dp;            // global ordering dp
  poly f = z5+xz3+x2-tz6;
  f = z5y+xz3y2+x2y4-tz6;             // homogenize f with respect to y
                                      // but consider t as parameter
  dim_slocus(f);              // projective 1-dimensional singular locus
  ideal j = diff(f,x),diff(f,z);
  dim(std(j));                        // V(j), projective 1-dimensional
  dim(std(j+ideal(f)));               // V(j,f) also projective 1-dimensional
  ideal q = quotient(j,ideal(f));
  ideal qsat = sat(j,f);              // saturation, proc from elim.lib
  dim(std(qsat));
  // projective 1-dimensional, hence q and/or qsat define the projective
  // polar curve. In this case, q and qsat are not the same, we needed
  // 2 quotients.
  // Let us check both reductions:
  size(reduce(qsat,std(q)));
  size(reduce(q,std(qsat)));
  // Hence q is contained in qsat but not conversely
  q;
  qsat;
  //
  // Now consider again the affine polar curve,
  // homogenize it with respect to y (deg t=0) and compare:
  // affine polar curve:
  ideal qa = 12zt+3z-10,5z2+12xt+3x,-144xt2-72xt-9x-50z;
  // homogenized:
  ideal qh = 12zt+3z-10y,5z2+12xyt+3xy,-144xt2-72xt-9x-50z;
  size(reduce(qh,std(qsat)));
  size(reduce(qsat,std(qh)));
  // both ideals coincide
LIB "tst.lib";tst_status(1);$
