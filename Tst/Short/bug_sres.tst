LIB "tst.lib";
// sres chokes here
option(prot);
ring r1 = 31991,(t,x,y,z,w),(c, dp);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
ideal j = std(i);
resolution rs = sres(j, 0); rs;
betti(rs);
list(rs);
kill r1;

ring r1 = 31991,(t,x,y,z,w),(c, ls);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
ideal j = std(i);
resolution rs = sres(j, 0); rs;
betti(rs);
list(rs);
tst_status(1); $


