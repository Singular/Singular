
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31),lp;
    ideal MYIDEALNAME=x1+x2+x3+x4+x5+x6,
x1*x7+x2*x8+x3*x9+x4*x10+x5*x11+x6*x12,
x1*x7*x13+x2*x8*x14+x3*x9*x15+x4*x10*x16+x5*x11*x17+x6*x12*x18,
x1*x7*x13*x19+x2*x8*x14*x20+x3*x9*x15*x21+x4*x10*x16*x22+x5*x11*x17*x23+x6*x12*x18*x24,
x1*x7*x13*x19*x25+x2*x8*x14*x20*x26+x3*x9*x15*x21*x27+x4*x10*x16*x22*x28+x5*x11*x17*x23*x29+x6*x12*x18*x24*x30,
x1*x7*x13*x19*x25*x31-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);$
    
