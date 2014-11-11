LIB "tst.lib";
tst_init();

// attrib is SB for newstruct:

ring r = 0, x, dp;
ideal I = x;
ideal J = x2;
J = std(J);
attrib(J, "isSB");
newstruct("foo", "ideal J");
foo F;
F.J = J;
reduce(I, F.J);
attrib(F.J, "isSB");

tst_status(1);$
