  ring r=31991,(t,x,y,z,w),(c, ls);
  ideal T=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,
    t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
  list R3=sres(std(T),0);
  size(R3);
  list R4=minres(R3);
  size(R4);
LIB "tst.lib";tst_status(1);$
