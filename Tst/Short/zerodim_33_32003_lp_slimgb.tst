
    LIB "tst.lib";
    tst_init();
    ring MYRINGNAME=32003,(a, b, c, d, e, f, g),lp;
    ideal MYIDEALNAME=d^2+2*c*e+2*b*f+2*a*g+g,
2*c*d+2*b*e+2*a*f+g^2+f,
c^2+2*b*d+2*a*e+2*f*g+e,
b^2+2*a*c+2*e*f+2*d*g+c,
2*a*b+e^2+2*d*f+2*c*g+b,
2*a*b+2*b*c+2*a*d+e^2+2*d*f+2*c*g+2*e*g+b+d,
a^2+2*d*e+2*c*f+2*b*g+a;
    option(redSB);
    slimgb(MYIDEALNAME);
    kill MYRINGNAME;
    tst_status(1);exit;
    