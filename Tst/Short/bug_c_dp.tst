LIB "tst.lib";
tst_init();

ring r1 = 31991,(t,x,y,z,w),(c, dp);
option(prot);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
std(i);

tst_status(1); $
