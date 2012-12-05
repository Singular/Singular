LIB "tst.lib";
tst_init();

LIB "mondromy.lib";

ring R=0,(x,y),ds;
jordan(monodromyB(x2y2+x6+y6));

tst_status(1); $
