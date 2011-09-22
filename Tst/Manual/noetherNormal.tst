LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring r=0,(x,y,z),dp;
ideal i= xy,xz;
noetherNormal(i);
tst_status(1);$
