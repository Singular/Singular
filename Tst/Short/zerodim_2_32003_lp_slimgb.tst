
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, c),lp;
    ideal MYIDEALNAME=a^2*b*c+a*b^2*c+a*b*c^2+a*b*c+a*b+a*c+b*c,
a^2*b^2*c+a*b^2*c^2+a^2*b*c+a*b*c+b*c+a+c,
a^2*b^2*c^2+a^2*b^2*c+a*b^2*c+a*b*c+a*c+c+1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    