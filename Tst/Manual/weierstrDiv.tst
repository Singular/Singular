LIB "tst.lib"; tst_init();
LIB "weierstr.lib";
ring R = 0,(x,y),ds;
poly f = y - xy2 + x2;
poly g = y;
list l = weierstrDiv(g,f,10); l;"";
l[1]*f + l[2];               //g = l[1]*f+l[2] up to degree 10
tst_status(1);$
