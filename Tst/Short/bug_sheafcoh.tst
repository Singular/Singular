LIB "tst.lib";
tst_init();

// Singular/Singular: Issue #1184
LIB"sheafcoh.lib";
ring r = 0,(x,y),dp;
vector s1 = [x];
vector s2 = [y];
module m = s1, s2;
intvec w = 1;
attrib(m, "isHomog", w);
sheafCohBGG(m, -3, 10);

tst_status(1);$