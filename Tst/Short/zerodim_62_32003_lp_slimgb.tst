
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=y^4+x^3*z-2*x*z^3+6*x^2+y*z,
4*y^4-2*x*y*z^2-x^2*z+2*y^2*z,
5*x^2*y^2+y^3*z+3*y*z^3+z^3;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    