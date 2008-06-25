
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y),lp;
    ideal MYIDEALNAME=x^4+y^4-1,
x^5*y^2+x^2*y^5-4*x^3*y^3-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    