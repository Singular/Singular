LIB "tst.lib";
// extended test of res commands for different orderings
ring r1 = 31991,(t,x,y,z,w),(c,dp);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;
ring r1 = 31991,(t,x,y,z,w),(c,lp);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(c,ds);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(c,ls);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;

ring r1 = 31991,(t,x,y,z,w),(C,dp);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;
ring r1 = 31991,(t,x,y,z,w),(c,lp);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(C,ds);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(c,ls);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;

ring r1 = 31991,(t,x,y,z,w),(dp,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;
ring r1 = 31991,(t,x,y,z,w),(lp,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(ds,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(ls,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;

ring r1 = 31991,(t,x,y,z,w),(dp,C);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;
ring r1 = 31991,(t,x,y,z,w),(lp,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(ds,C);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
ring r1 = 31991,(t,x,y,z,w),(ls,c);
ideal i=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
tst_test_res(i);
kill r1;

tst_status(1);
$

