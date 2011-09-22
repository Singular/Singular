LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),ls;
tail((y+z+x+xyz)**2);
ring s=0,(x,y,z),dp;
tail((y+z+x+xyz)**2);
tst_status(1);$
