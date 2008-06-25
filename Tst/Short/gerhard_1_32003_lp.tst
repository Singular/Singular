
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(w, t, x, y, z),lp;
    ideal MYIDEALNAME=7*w^2*y+4*x^2*y+x*y^2+2*w*t*z,
3*w^3*t*z+3*w^2*y*z^2+2*y*z^4,
2*t^2*x^5*y^3+5*w^4*t^3*x^2*z;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    