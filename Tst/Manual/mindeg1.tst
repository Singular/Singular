LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),ls;
poly f = x5+y2+z3;
ord(f);                  // ord returns weighted order of leading term!
intvec v = 1,-3,2;
mindeg1(f,v);            // computes minimal weighted degree
matrix m[2][2]=x10,1,0,f^2;
mindeg1(m,1..3);         // computes absolute minimum of weighted degrees
tst_status(1);$
