LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.6 1999-06-01 12:17:36 mschulze Exp $");

LIB "monodrmy.lib";
LIB "jordan.lib";

ring R=0,(x,y),ds;
jordan(monodromy(x2y2+x6+y6));

tst_status(1); $
