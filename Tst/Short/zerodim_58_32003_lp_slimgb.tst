
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z, t),lp;
    ideal MYIDEALNAME=2*x^2-2*y^2+2*z^2-2*t^2-1,
2*x^3-2*y^3+2*z^3-2*t^3-1,
2*x^4-2*y^4+2*z^4-2*t^4-1,
2*x^5-2*y^5+2*z^5-2*t^5-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    