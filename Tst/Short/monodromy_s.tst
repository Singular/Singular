LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.4 1998-12-28 14:36:29 mschulze Exp $");

LIB "monodromy.lib";
LIB "jordan.lib";

ring R=0,(x,y),ds;
jordan(monodromy(x2y2+x6+y6));

tst_status(1); $
