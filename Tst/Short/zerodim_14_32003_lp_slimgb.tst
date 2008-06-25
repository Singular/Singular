
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4),lp;
    ideal MYIDEALNAME=-4*x1+x2+x3+x4,
-4*x1^2+x2^2+x3^2+x4^2+4*x1+x2+x3+x4-3,
5*x1^3+4*x3^2*x4+3*x2^2+2*x1*x4+4*x1+x2+x3+2*x4-1,
5*x3^4+x4^3+16*x1^2+3*x2^2-4*x4-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    