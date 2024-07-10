LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(b,c,t,h),dp;
ideal I=
bct-t2h+2th2+h3,
bt3-ct3-t4+b2th+c2th-2bt2h+2ct2h+2t3h-bch2-2bth2+2cth2+2th3,
b2c2+bt2h-ct2h-t3h+b2h2+2bch2+c2h2-2bth2+2cth2+t2h2-2bh3+2ch3+2th3+3h4,
c2t3+ct4-c3th-2c2t2h-2ct3h-t4h+bc2h2-2c2th2-bt2h2+4t3h2+2bth3-2cth3-t2h3
+bh4-6th4-2h5;
hilbPoly(I);

// hilbPoly(ideal(1)) PR #1233
hilbPoly(ideal(1));
tst_status(1);$
