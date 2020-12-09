LIB "tst.lib"; tst_init();
  proc t1 { }
  proc t2 { }
  ring s;
  poly ss;
  ring r;
  poly f=x+y+z;
  int i=7;
  ideal I=f,x,y;
  listvar();
  listvar(r);
  listvar(t1);
  listvar(proc);
  LIB "polylib.lib";
  listvar(Polylib);
tst_status(1);$
