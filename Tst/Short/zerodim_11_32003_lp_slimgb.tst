
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3),lp;
    ideal MYIDEALNAME=4*x1*x2+2*x1^2+2*x2^2+5*x3^3-10,
2*x1*x2+3*x1^2+3*x2^2+2*x3^3-2,
4*x1^3+x1^4+4*x2^3+x2^4+4*x3^4+12*x1*x2^2+12*x1^2*x2-4*x1*x2^3-4*x1^3*x2+6*x1^2*x2^2-4;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    