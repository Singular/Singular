
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=4*x+5*y+6,
2*x^2*z+4*y^2*z+4*y*z^2+3*x*y+25*y^2+7*x*z+2*y-3*z,
x^2*y+3*x*y*z+x*z^2+15*x^2+x*y+9*y*z+7;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    