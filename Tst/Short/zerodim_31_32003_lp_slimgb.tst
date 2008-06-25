
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y),lp;
    ideal MYIDEALNAME=3*x^2*y^9+y^9+5*x^4,
9*x^3*y^8+11*y^10+9*x*y^8;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    