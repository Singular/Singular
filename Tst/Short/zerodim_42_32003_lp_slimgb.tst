
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10),lp;
    ideal MYIDEALNAME=x10^2-2*x5,
-x6*x8+x7*x9-210*x6,
-x4*x8+700000,
x4*x8-x7*x9-x9*x10-410*x9,
-x3*x8+x6*x8+x9*x10+210*x6+1300000,
10*x2*x8+10*x3*x8+10*x6*x8+10*x7*x9-10*x2*x10-11*x7*x10-10*x9*x10-10*x10^2+1400*x6-4200*x10,
-10*x1*x8-10*x2*x8-10*x3*x8-10*x4*x8-10*x6*x8+10*x2*x10+11*x7*x10+20000*x2+14*x5,
-64*x2*x7-10*x7*x9-11*x7*x10+320000*x1-16*x7+7000000,
-32*x2*x7-5*x2*x8-5*x2*x10+160000*x1-5000*x2,
64*x2*x7-10*x1*x8+10*x7*x9+11*x7*x10-320000*x1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    