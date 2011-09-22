LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  module m=gen(1),gen(3),[x,y,0,z],[x+y,0,0,0,1];
  print(m);
  print(prune(m));
tst_status(1);$
