LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),wp(1,2,3);
poly f = x+y2+z3;
deg(f);             //deg; returns weighted degree (in case of 1 block)!
maxdeg(f);
matrix m[2][2]=f+x10,1,0,f^2;
maxdeg(m);
tst_status(1);$
