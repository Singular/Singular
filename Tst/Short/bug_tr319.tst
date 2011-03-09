LIB "tst.lib";
tst_init();

// attribute handling
ring r;
intmat m[1][1] = -4;

attrib(r, "lll", "str1");
attrib(r, "ll", m);
attrib(r, "l", "str2");

attrib(r);

attrib(r, "ll");
attrib(r, "l");
attrib(r, "lll");

tst_status(1);$
