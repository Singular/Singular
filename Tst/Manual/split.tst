LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring r= 0,(x,y,z),ds;
poly f = (x+y+z)^4;
split(string(f),50);
split(lprint(f));
tst_status(1);$
