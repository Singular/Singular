  LIB "primdec.lib";
  ring r = 0,(x,y,z,t,w),dp; 
  ideal i=
     x2+2y2+2z2+2t2-x, 
     2xy+2yz+2zt-y, 
     y2+2xz+2yt-z, 
     x+2y+2z+2t-1;
  decomp(i);
  // We consider now the ideal J of the base space of the 
  // miniversal deformation of the cone over the rational
  // normal curve computed in section *11* and compute 
  // its primary decomposition.
  ring R = 0,(A,B,C,D),dp;
  ideal J = CD, BD+D2, AD;
  decomp(J);
  // We see that there are two components which are both
  // prime, even linear subspaces, one 3-dimensional,
  // the other 1-dimensional
  // (This is Pinkhams example and was the first found 
  // surface singularity with two components of 
  // different dimensions)
  //
  // Let us now produce an embedded component in the last
  // example, compute the minimal associated primes and
  // the radical. We use the Characteristic set methods
  // from prim_dec.lib.
  J = intersect(J,maxideal(3));
  primdecSY(J,1);    //shows that the maximal ideal is
                    //embedded (takes a few seconds)
  min_ass_prim_charsets(J,1);  
  radical(J);
$
