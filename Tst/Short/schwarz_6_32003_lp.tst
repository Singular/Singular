
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, c, d, e, g, h),lp;
    ideal MYIDEALNAME=-2*d*e-2*c*g-a*h-2*b*h,
-d^2-2*c*e-a*g-2*b*g-h^2,
-2*c*d-a*e-2*b*e-2*g*h,
-c^2-a*d-2*b*d-g^2-2*e*h,
-a*c-2*b*c-2*e*g-2*d*h,
-a*b-b^2-e^2-2*d*g-2*c*h;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    