
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=x*y^3+y^4-2*x*z^3+y*z^2-z^3,
x^3*y+2*x*y^2*z+2*x^2*y,
2*x^3*y+y*z^3-3*x^2*y+2;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    