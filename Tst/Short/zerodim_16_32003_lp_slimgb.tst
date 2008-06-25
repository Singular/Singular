
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=9*x^3+5*z^3+10*y^2+6,
6*x^2*y^3+5*x^2*z^3+x^4+16*x^3+10*x*y^2+11*y^2*z+7*y*z^2+z^3+25*y^2+40*y*z+15*z^2+7,
57*x*y^5+x^4+6*x^3*y+12*x^2*y^2+2*x^3*z+12*x^2*y*z+7*x*z^3+12*y*z^3+3*z^4+3*x^3+36*x*y*z+25*y*z^2+7*z^3+29*y^2+2*y*z-3*y-3*z;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    