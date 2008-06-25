
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(y, z, x, t),lp;
    ideal MYIDEALNAME=2*y*z*t+x*t^2-2*z-x,
y^2*z+2*y*x*t-z-2*x,
-z^3*x+4*y*z^2*t+4*z*x*t^2+2*y*t^3+4*z^2+4*z*x-10*y*t-10*t^2+2,
4*y^2*z*x-z*x^3+2*y^3*t+4*y*x^2*t-10*y^2+4*z*x+4*x^2-10*y*t+2;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    