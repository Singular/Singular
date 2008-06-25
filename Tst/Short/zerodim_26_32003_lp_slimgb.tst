
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x, y),lp;
    ideal MYIDEALNAME=49*x^7*y+35*x*y^7+37*x*y^6+9*y^7+4*x^6+27*x^3*y^2+20*x*y^4+31*x^4+33*x^2*y+24*x^2+49*y+43,
45*x^8+3*x^7+39*x^6+30*x^5+13*x^4+41*x^3+5*x^2+46*x+7;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    