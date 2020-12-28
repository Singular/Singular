LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring  r = 0,(x,y,z),lp;
poly  p = (x+y)*(y+z);
poly  q = (z4+2)*(y+z);
lcm(p,q);
ideal i=p,q,y+z;
lcm(i,p);
lcm(2,3,6);
lcm(2..6);
tst_status(1);$
