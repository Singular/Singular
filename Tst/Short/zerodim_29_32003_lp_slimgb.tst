
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, u, v, w, x, y, z),lp;
    ideal MYIDEALNAME=136*z-136,
-240*a-64*v+112*y+420*z,
66*a*z+78*v*z-1056*a-90*u+90*x+336*y,
-162*a^2-284*a*v+50*a*y+60*v*y+180*a*z+55*u*z-112*b+70*w+260*x,
28*a*v*z-648*a^2+300*a*u+36*b*x+44*v*x+128*a*y+40*u*y+36*b*z+192*w,
-162*a^2*v+18*a*v*y+15*a*u*z-312*a*b+24*a*w+30*v*w+84*a*x+27*u*x+24*b*y,
-162*a^2*u+10*a*v*x+8*a*u*y+6*a*b*z+48*a*w+16*u*w+14*b*x,
-162*a^2*b+4*a*v*w+3*a*u*x+2*a*b*y+6*b*w;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    