LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
poly f = x2y2z2+x5+yx3+z6;
poly g = x5+yx3;
poly h = y2z5+x5+yx3;
chaincrit(f,g,h);
vector u = [x2y3-z2,x2y];
vector v = [x2y2+z2,x2-y2];
vector w = [x2y4+z3,x2+y2];
chaincrit(u,v,w);
tst_status(1);$
