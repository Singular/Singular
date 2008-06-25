
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=-2*x*z^3+3*x^3+x*y^2+y*z^2,
2*y^3*z+3*x*y*z^2-x^2*z+2*y*z^2+z^2,
3*x*y^3+y^4+y*z^2+x;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    