LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r= 0,(x,y,z),ds;
poly g = 345x2-1234567890y+7/4z;
maxcoef(g);
ideal i = g,10/1234567890;
maxcoef(i);
// since i[2]=1/123456789
tst_status(1);$
