LIB "tst.lib"; tst_init();
LIB "weierstr.lib";
ring R = 0,(x,y),ds;
poly f = xy+y2+y4;
list l = weierstrPrep(f,5); l; "";
f*l[1]-l[2];                      // = 0 up to degree 5
tst_status(1);$
