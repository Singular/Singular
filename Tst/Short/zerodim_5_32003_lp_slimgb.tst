
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3),lp;
    ideal MYIDEALNAME=-5*x1^2+2*x1*x2-2*x2^2+8*x1*x3-10*x2*x3-13*x3^2+x1-x2,
-2*x1^2-2*x2^2-4*x1*x3-4*x2*x3-4*x3^2+x1+x2,
6*x1^2-4*x1*x2+3*x2^2-8*x1*x3+10*x2*x3+13*x3^2-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    