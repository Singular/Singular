
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37, x38, x39, x40, x41, x42, x43),lp;
    ideal MYIDEALNAME=x1+x2+x3+x4+x5+x6+x7,
x1*x8+x2*x9+x3*x10+x4*x11+x5*x12+x6*x13+x7*x14,
x1*x8*x15+x2*x9*x16+x3*x10*x17+x4*x11*x18+x5*x12*x19+x6*x13*x20+x7*x14*x21,
x1*x8*x15*x22+x2*x9*x16*x23+x3*x10*x17*x24+x4*x11*x18*x25+x5*x12*x19*x26+x6*x13*x20*x27+x7*x14*x21*x28,
x1*x8*x15*x22*x29+x2*x9*x16*x23*x30+x3*x10*x17*x24*x31+x4*x11*x18*x25*x32+x5*x12*x19*x26*x33+x6*x13*x20*x27*x34+x7*x14*x21*x28*x35,
x1*x8*x15*x22*x29*x36+x2*x9*x16*x23*x30*x37+x3*x10*x17*x24*x31*x38+x4*x11*x18*x25*x32*x39+x5*x12*x19*x26*x33*x40+x6*x13*x20*x27*x34*x41+x7*x14*x21*x28*x35*x42,
x1*x8*x15*x22*x29*x36*x43-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);$
    
