LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
sameComponent([y+z+x,xyz],[z2,xyz]);
sameComponent([y+z+x,xyz],[z4,xyz]);
sameComponent(y+z+x+xyz, xy+z5);
tst_status(1);$
