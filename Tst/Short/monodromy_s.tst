LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: monodromy_s.tst,v 1.5 1999-04-30 16:57:46 obachman Exp $");

LIB "monodrom.lib";
LIB "jordan.lib";

ring R=0,(x,y),ds;
jordan(monodromy(x2y2+x6+y6));

tst_status(1); $
