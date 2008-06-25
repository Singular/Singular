
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y),lp;
    ideal MYIDEALNAME=y^2+2*x-7*y+5,
x^2+6*x+3*y-4;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    