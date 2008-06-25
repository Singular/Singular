
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, c, d, x, f),lp;
    ideal MYIDEALNAME=a+b+c+d+x+f,
a*b+b*c+c*d+d*x+a*f+x*f,
a*b*c+b*c*d+c*d*x+a*b*f+a*x*f+d*x*f,
a*b*c*d+b*c*d*x+a*b*c*f+a*b*x*f+a*d*x*f+c*d*x*f,
a*b*c*d*x+a*b*c*d*f+a*b*c*x*f+a*b*d*x*f+a*c*d*x*f+b*c*d*x*f,
a*b*c*d*x*f-1;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    