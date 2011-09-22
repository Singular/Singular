LIB "tst.lib"; tst_init();
LIB "weierstr.lib";
ring R = 2,(x,y,z),ls;
poly f = xyz;
lastvarGeneral(f);
tst_status(1);$
