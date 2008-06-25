
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=2*x^2+4*x*y+3*y^2+7*x*z+9*y*z+5*z^2+2,
3*x^2+x*y+6*y^2+3*x*z+2*y*z+4*z^2+5,
7*x^2+8*x*y+12*y^2+18*x*z+3*y*z+10*z^2+12;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    