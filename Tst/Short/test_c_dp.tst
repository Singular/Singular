LIB "tst.lib";
tst_init();

ring r1 = 31991,(t,x,y,z,w),(c, dp);
option(prot);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
std(i);
module m= 
gen(1)*(t2x2+tx2y+x2yz) + gen(2)*(t2x2+tx2y+x2yz) + t2x2+tx2y+x2yz,
gen(2)*(t2y2+ty2z+y2zw)+gen(3)*(t2y2+ty2z+y2zw) + t2y2+ty2z+y2zw,
gen(3)*(t2z2+tz2w+xz2w)+gen(1)*(t2z2+tz2w+xz2w) + t2z2+tz2w+xz2w,
gen(1)*(t2w2+txw2+xyw2) + gen(2)*(t2w2+txw2+xyw2) +
gen(3)*(t2w2+txw2+xyw2) + t2w2+txw2+xyw2;
option(prot);
std(m);

tst_status(1);$
