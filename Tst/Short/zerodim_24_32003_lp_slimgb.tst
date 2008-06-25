
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z, w),lp;
    ideal MYIDEALNAME=-3*w^2+x+3*y+2*z+13,
-11*z^2+x+3*z+7*w-9,
4*y^2+4*x+3*z+2*w+4,
5*x^2+6*x+3*y+6*z+2*w+3;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    