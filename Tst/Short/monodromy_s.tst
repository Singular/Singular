LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.7 1999-06-16 18:23:35 mschulze Exp $");

LIB "monodrmy.lib";

ring R=0,(x,y),ds;
jordan(monodromy(x2y2+x6+y6));

tst_status(1); $
