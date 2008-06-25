
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, c, d, u, v, w, x, y, z),lp;
    ideal MYIDEALNAME=210*z-210,
-80*d+180*y+855*z,
136*d*z-114*c+152*x+720*y,
112*d*y+105*c*z-144*b+126*w+595*x,
90*d*x+84*c*y+78*b*z-170*a+102*v+480*w,
70*d*w+65*c*x+60*b*y+55*a*z+80*u+375*v,
52*d*v+48*c*w+44*b*x+40*a*y+280*u,
36*d*u+33*c*v+20*b*w+27*a*x,
20*c*u+18*b*v+16*a*w,
8*b*u+7*a*v;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    