LIB "tst.lib"; tst_init();
  ring r=32003,(a,b,c,d),dp;
  ideal j=bc-ad,b3-a2c,c3-bd2,ac2-b2d;
  list T=mres(j,0); // 0 forces a full resolution
  // a minimal set of generators for j:
  print(T[1]);
  // second syzygy module of r/j which is the first
  // syzygy module of j (minimal generating set):
  print(T[2]);
  // the second syzygy module (minimal generating set):
  print(T[3]);
  print(T[4]);
  betti(T);
  // most useful for reading off the graded Betti numbers:
  print(betti(T),"betti");
tst_status(1);$
