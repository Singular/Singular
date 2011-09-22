LIB "tst.lib"; tst_init();
LIB "grwalk.lib";
//** compute a Groebner basis of I w.r.t. lp.
ring r = 32003,(z,y,x), lp;
ideal I = y3+xyz+y2z+xz3, 3+xy+x2y+y2z;
gwalk(I);
tst_status(1);$
