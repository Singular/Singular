LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.8 1999-06-22 10:48:13 obachman Exp $");

LIB "mondromy.lib";

ring R=0,(x,y),ds;
jordan(monodromy(x2y2+x6+y6));

tst_status(1); $
