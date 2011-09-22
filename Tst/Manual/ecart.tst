LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),ls;
ecart((y+z+x+xyz)**2);
ring s=0,(x,y,z),dp;
ecart((y+z+x+xyz)**2);
tst_status(1);$
