LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r = 0,(x,y,z),wp(1,2,3);
poly f = x+y2+z3;
deg(f);            //deg returns weighted degree (in case of 1 block)!
maxdeg1(f);
intvec v = ringweights(r);
maxdeg1(f,v);                        //weighted maximal degree
matrix m[2][2]=f+x10,1,0,f^2;
maxdeg1(m,v);                        //absolute weighted maximal degree
tst_status(1);$
