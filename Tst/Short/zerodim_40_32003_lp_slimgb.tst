
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=x^2*y*z+x*y^2*z+x*y*z^2+x*y*z+x*y+x*z+y*z,
x^2*y^2*z+x*y^2*z^2+x^2*y*z+x*y*z+y*z+x+z,
x^2*y^2*z^2+x^2*y^2*z+x*y^2*z+x*y*z+x*z+z+1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    