  ring r  = 31991,(t,x,y,z,w),ls;
  ideal M = t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,
            t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
  list L  = mres(M,0);
  size(L);
  // projective dimension of M is 4
$
