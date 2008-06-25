
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=2*x^2+3*y^2+7*x*z+9*y*z+5*z^2+4*x,
3*x^4+6*y^3+x*y*z+3*x*z^2+2*y*z^2+4*z^2+5,
3*y^4*z+7*x^3+10*z^3+8*x*y+12*y^2+18*x*z+12;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    