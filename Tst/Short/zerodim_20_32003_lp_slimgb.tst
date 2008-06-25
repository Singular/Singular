
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(t1, t2, t3),lp;
    ideal MYIDEALNAME=4*t2^2*t3^2+2*t2^2+5*t2*t3+3*t3^2+1,
4*t1^2*t3^2+3*t1^2+5*t1*t3+2*t3^2+1,
4*t1^2*t2^2+2*t1^2+5*t1*t2+3*t2^2+1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    