
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, x, y, z, u, v, w),lp;
    ideal MYIDEALNAME=136*z-136,
-240*a+112*y+420*z-64*v,
66*a*z+78*z*v-1056*a+90*x+336*y-90*u,
-162*a^2+50*a*y+180*a*z+55*z*u-284*a*v+60*y*v-112*b+260*x+70*w,
28*a*z*v-648*a^2+36*b*x+128*a*y+36*b*z-300*a*u+40*y*u+44*x*v+192*w,
15*a*z*u-162*a^2*v+18*a*y*v-312*a*b+84*a*x+24*b*y+27*x*u+24*a*w+30*v*w,
6*a*b*z-162*a^2*u+8*a*y*u+10*a*x*v+14*b*x+48*a*w+16*u*w,
-162*a^2*b+2*a*b*y+3*a*x*u+4*a*v*w+6*b*w;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    