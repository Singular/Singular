LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.9 2001-01-11 12:04:33 pfister Exp $");

LIB "mondromy.lib";

ring R=0,(x,y),ds;
jordan(monodromyB(x2y2+x6+y6));

tst_status(1); $
