LIB "tst.lib"; tst_init();
LIB "arcpoint.lib";
ring r=0,(x,y,z),dp;
ideal I = x3,y+z2-x2;
I;
removepower(I);
tst_status(1);$
