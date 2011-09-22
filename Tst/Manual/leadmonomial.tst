LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring s=0,(x,y,z),(c,dp);
leadmonomial((y+z+x+xyz)^2);
leadmonomial([(y+z+x+xyz)^2,xyz5]);
tst_status(1);$
