LIB "tst.lib"; tst_init();
  ring r1=0,(x,y),lp;
  // the basering is r1
  ring r2=32003,(a(1..8)),ds;
  // the basering is r2
  setring r1;
  // the basering is again r1
  nameof(basering);
  listvar();
tst_status(1);$
