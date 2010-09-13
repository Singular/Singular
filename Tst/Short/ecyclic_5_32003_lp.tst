
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21),lp;
    ideal MYIDEALNAME=x1+x2+x3+x4+x5,
x1*x6+x2*x7+x3*x8+x4*x9+x5*x10,
x1*x6*x11+x2*x7*x12+x3*x8*x13+x4*x9*x14+x5*x10*x15,
x1*x6*x11*x16+x2*x7*x12*x17+x3*x8*x13*x18+x4*x9*x14*x19+x5*x10*x15*x20,
x1*x6*x11*x16*x21-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);$
    
