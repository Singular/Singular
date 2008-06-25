
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6, x7),lp;
    ideal MYIDEALNAME=x4^2+2*x3*x5+2*x2*x6+2*x1*x7+x7,
2*x3*x4+2*x2*x5+2*x1*x6+x7^2+x6,
x3^2+2*x2*x4+2*x1*x5+2*x6*x7+x5,
2*x2*x3+2*x1*x4+x6^2+2*x5*x7+x4,
x2^2+2*x1*x3+2*x5*x6+2*x4*x7+x3,
2*x1*x2+x5^2+2*x4*x6+2*x3*x7+x2,
x1^2+2*x4*x5+2*x3*x6+2*x2*x7+x1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    