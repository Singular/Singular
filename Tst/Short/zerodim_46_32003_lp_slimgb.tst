
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(t, x, y, z),lp;
    ideal MYIDEALNAME=t^2*z+x^2*z+y^2*z+z-1,
t^2*y+x^2*y+y*z^2+y-1,
t*x^2+t*y^2+t*z^2+t-1,
t^2*x+x*y^2+x*z^2+x-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    