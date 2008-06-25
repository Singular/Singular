
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(y, z, x),lp;
    ideal MYIDEALNAME=2*y+2*z+x-1,
2*y*z+2*y*x-y,
2*y^2+2*z^2+x^2-x;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    