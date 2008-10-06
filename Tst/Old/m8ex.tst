  // Two transversal cusps in (k^3,0):
  ring r1 = 0,(t,x,y,z),ls;
  ideal i1 = x-t2,y-t3,z;        // parametrization of the first branch
  ideal i2 = y-t2,z-t3,x;        // parametrization of the second branch
  ideal j1 = eliminate(i1,t);
  j1;                            // equations of the first branch
  ideal j2 = eliminate(i2,t);
  j2;                            // equations of the second branch
  // Now map to a ring with only x,y,z as variables and compute the
  // intersection of j1 and j2 there:
  ring r2 = 0,(x,y,z),ds;
  ideal j1= imap(r1,j1);         // imap is a convenient ringmap for
  ideal j2= imap(r1,j2);         // inclusions and projections of rings
  ideal i = intersect(j1,j2);
  i;                             // equations of both branches
  intvec v= qhweight(i);         // compute weights
  v;
  //----------------------------------------------------
  // The tangent developable of a projective variety given parametrically
  // by F=(f1,...,fn) : P^r --> P^n is the union of all tangent spaces
  // of the image. The tangent space at a smooth point F(t1,...,tr)
  // is given as the image of the tangent space at (t1,...,tr) under
  // the tangent map
  //   T(t1,...,tr): (y1,...,yr) --> jacob(f)*transpose((y1,...,yr))
  // where jacob(f) denotes the jacobian matrix of f with respect to the
  // t's evaluated at the point (t1,...,tr).
  // Hence we have to create the graph of this map and then to eliminate
  // the t's and y's.
  // The rational normal curve in P^4 is given as the image of
  //        F(s,t) = (s4,s3t,s2t2,st3,t4)
  // each component being homogeneous of degree 4.
  ring P = 0,(s,t,x,y,a,b,c,d,e),dp;
  ideal M = maxideal(1);
  ideal F = M[1..2];     // take the 1-st two generators of M
  F=F^4;
  // simplify(...,2); deletes 0-columns
  matrix jac = simplify(jacob(F),2);
  ideal T = x,y;
  ideal J = jac*transpose(T);
  ideal H = M[5..9];
  ideal i = H-J;         // this is tricky: difference between two
                         // ideals is not defined, but between two
                         // matrices. By automatic type conversion
                         // the ideals are converted to matrices,
                         // subtracted and afterwards converted
                         // to an ideal. Note that '+' is defined
                         // and adds (concatenates) two ideals
  i;
  // Now we define a ring with product ordering and weights 4
  // for the variables a,...,e.
  // Then we map i from P to P1 and eliminate s,t,x,y from i.
  ring P1 = 0,(s,t,x,y,a,b,c,d,e),(dp(4),wp(4,4,4,4,4));
  ideal i = fetch(P,i);
  ideal j= eliminate(i,stxy);    // equations of tangent developable
  j;
  // We can use the product ordering to eliminate s,t,x,y from i
  // by a std-basis computation.
  // We need proc 'nselect' from elim.lib.
  LIB "elim.lib";
  j = std(i);                    // compute a std basis j
  j = nselect(j,1..4);           // select generators from j not
  j;                             // containing variable 1,...,4
  killall();
LIB "tst.lib";tst_status(1);$
