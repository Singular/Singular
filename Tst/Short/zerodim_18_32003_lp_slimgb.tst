
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y),lp;
    ideal MYIDEALNAME=x*y^2+x+y+1,
7*x^2*y^2+x^2*y+x+y;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    