
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6),lp;
    ideal MYIDEALNAME=x1+x2+x3+x4+x5+x6,
x1*x2+x2*x3+x3*x4+x4*x5+x1*x6+x5*x6,
x1*x2*x3+x1*x3*x4+x2*x3*x4+x1*x2*x6+x1*x5*x6+x4*x5*x6,
x1*x2*x3*x4+x2*x3*x4*x5+x1*x2*x3*x6+x1*x2*x5*x6+x1*x4*x5*x6+x3*x4*x5*x6,
x1*x2*x3*x4*x5+x1*x2*x3*x4*x6+x1*x2*x3*x5*x6+x1*x2*x4*x5*x6+x1*x3*x4*x5*x6+x2*x3*x4*x5*x6,
x1*x2*x3*x4*x5*x6-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    