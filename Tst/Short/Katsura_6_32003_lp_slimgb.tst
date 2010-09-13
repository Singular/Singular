
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x0, x1, x2, x3, x4, x5, x6),lp;
    ideal MYIDEALNAME=x0+2*x1+2*x2+2*x3+2*x4+2*x5+2*x6-1,
2*x2*x3+2*x1*x4+2*x0*x5+2*x1*x6-x5,
x2^2+2*x1*x3+2*x0*x4+2*x1*x5+2*x2*x6-x4,
2*x1*x2+2*x0*x3+2*x1*x4+2*x2*x5+2*x3*x6-x3,
x1^2+2*x0*x2+2*x1*x3+2*x2*x4+2*x3*x5+2*x4*x6-x2,
2*x0*x1+2*x1*x2+2*x2*x3+2*x3*x4+2*x4*x5+2*x5*x6-x1,
x0^2+2*x1^2+2*x2^2+2*x3^2+2*x4^2+2*x5^2+2*x6^2-x0;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);$
    
