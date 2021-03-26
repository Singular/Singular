LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),ls;
poly f = x5+y2+z3;
ord(f);                  // ord returns weighted order of leading term!
mindeg(f);               // computes minimal degree
matrix m[2][2]=x10,1,0,f^2;
mindeg(m);               // computes matrix of minimum degrees
tst_status(1);$
