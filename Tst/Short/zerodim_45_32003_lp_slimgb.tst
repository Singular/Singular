
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(t, z, y, x),lp;
    ideal MYIDEALNAME=t+2*z+2*y+2*x-1,
z^2+2*t*y+2*z*x-y,
2*t*z+2*z*y+2*y*x-z,
t^2+2*z^2+2*y^2+2*x^2-t;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    