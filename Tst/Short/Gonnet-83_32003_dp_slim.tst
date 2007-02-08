
  LIB "tst.lib";
  tst_init();
  ring MYRINGNAME=32003,(a0, a2, a3, a4, a5, b0, b1, b2, b3, b4, b5, c0, c1, c2, c3, c4, c5),dp;
  ideal MYIDEALNAME=a5*b5,
a5*b4+a4*b5,
a4*b4,
a5*b3+a3*b5,
a5*b3+a3*b5+2*a5*b5,
a3*b3+a5*b3+a3*b5+a5*b5,
2*a3*b3+a5*b3+a3*b5,
a4*b2+a2*b4,
a2*b2,
a5*b1+a4*b3+a3*b4+b5,
a4*b1+b4,
a2*b1+b2,
a0*b1+a4*b1+a3*b2+a2*b3+b0+2*b1+b4+c1,
a5*b0+a5*b1+a4*b3+a3*b4+2*a5*b4+a0*b5+2*a4*b5+b5+c5,
a4*b0+a4*b1+a5*b2+a0*b4+2*a4*b4+a2*b5+b4+c4,
a3*b0+2*a3*b1+a5*b1+a0*b3+a4*b3+a3*b4+2*b3+b5+c3,
a3*b0+a5*b0+a3*b1+a5*b1+a0*b3+a4*b3+a3*b4+a5*b4+a0*b5+a4*b5+b3+b5+c3+c5-1,
a2*b0+a2*b1+a0*b2+a4*b2+a2*b4+b2+c2,
a0*b0+a4*b0+a0*b1+a4*b1+a3*b2+a5*b2+a2*b3+a0*b4+a4*b4+a2*b5+b0+b1+b4+c0+c1+c4;
  option(redSB);
  slimgb(MYIDEALNAME);
  tst_status(1);$
  