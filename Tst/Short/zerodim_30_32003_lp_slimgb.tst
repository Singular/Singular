
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z, t),lp;
    ideal MYIDEALNAME=2*y*z*t+x*t^2-x-2*z,
y^2*z+2*x*y*t-2*x-z,
-x*z^3+4*y*z^2*t+4*x*z*t^2+2*y*t^3+4*x*z+4*z^2-10*y*t-10*t^2+2,
-x^3*z+4*x*y^2*z+4*x^2*y*t+2*y^3*t+4*x^2-10*y^2+4*x*z-10*y*t+2;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    