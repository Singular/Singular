LIB "tst.lib"; tst_init();
  ring R=32003,(x,y,z),dp;
  ideal i=x2,y2,z2;
  ideal s=std(i);
  hilb(s);
  hilb(s,1);
  hilb(s,2);
  intvec w=2,2,2;
  hilb(s,1,w);
tst_status(1);$
