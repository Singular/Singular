LIB "tst.lib"; tst_init();
LIB "teachstd.lib";
ring r=0,(x,y,z),dp;
poly f = y3z3+x5+yx3+z6;
poly g = x5+yx3;
prodcrit(f,g);
vector v = x3z2*gen(1)+x3y*gen(1)+x2y*gen(2);
vector w = y4*gen(1)+y3*gen(2)+xyz*gen(1);
prodcrit(v,w);
tst_status(1);$
