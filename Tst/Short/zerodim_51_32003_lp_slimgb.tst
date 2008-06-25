
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(z, y, x),lp;
    ideal MYIDEALNAME=y^4+z*y^2*x+z^2+y^2-2*y*x+x^2,
z^3*y*x-y^2*x^3+y^4+z*y^2*x-2*y*x,
z^4*y+y^4*x-2*y*x^2-3;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    