
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y, z),lp;
    ideal MYIDEALNAME=7*x*y+3*y*z+x+4*y+2*z+10,
x^3+x^2*y+2*y^3+3*x*y*z+6*y^2*z+5*x*y+y*z+3*x+1,
3*x^4+2*x^2*y^2+4*x^2*z^2+5*z^4+3*x^2*y+x*y*z+6*y^2*z+x*z^2;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    