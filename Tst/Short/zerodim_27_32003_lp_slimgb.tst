
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(w, x, y, z),lp;
    ideal MYIDEALNAME=-2*w^2+9*w*x+8*x^2+9*w*y+9*x*y+6*y^2-7*w*z-3*x*z-7*y*z-6*z^2-4*w+8*x+4*y+8*z+2,
3*w^2-5*w*x+4*x^2-3*w*y+2*x*y+9*y^2-6*w*z-2*x*z+6*y*z+7*z^2+9*w+7*x+5*y+7*z+5,
7*w^2+5*w*x+2*x^2+3*w*y+9*x*y-4*y^2-5*w*z-7*x*z-5*y*z-4*z^2-5*w+4*x+6*y-9*z+2,
8*w^2+5*w*x+5*x^2-4*w*y+2*x*y+7*y^2+2*w*z-7*x*z-8*y*z+7*z^2+3*w-7*x-7*y-8*z+8;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    