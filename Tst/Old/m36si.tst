  ring r=0,(x,y,z),dp;
  poly s1 = x2;
  poly s2 = y3;
  poly s3 = z;
  ideal i =  s1, s2-s1, s2*s3, s3^4;
  i;
  ideal j = i^2;
  // generators i[k]*i[l], 1<=k<=l<=size(i);
  ideal IJ = i*j;
  // generators i[k]*j[l], 1<=k<=size(i), 1<=l<=size(j)
  size(j), size(IJ);
LIB "tst.lib";tst_status(1);$
