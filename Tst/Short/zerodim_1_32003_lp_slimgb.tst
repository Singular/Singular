
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2),lp;
    ideal MYIDEALNAME=x1^2+x2^2-10,
x1^2+x1*x2+2*x2^2-16;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    