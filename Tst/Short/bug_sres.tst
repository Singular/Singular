LIB "tst.lib";
ring r1 = 31991,(t,x,y,z,w),(c, ls);
option(prot);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
ideal j = std(i);
resolution rs = sres(std(j), 0); rs;
betti(rs);
list(rs);
tst_status(1); $


