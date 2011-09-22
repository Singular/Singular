LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  module m=[1,y],[0,x+z];
  m;
  print(m);  // the columns generate m
  string s=print(m,"%s"); s;
  s=print(m,"%2s"); s;
  s=print(m,"%l"); s;
  s=print(m,"%;"); s;
  s=print(m,"%t"); s;
  s=print(m,"%p"); s;
  intmat M=betti(mres(m,0));
  print(M,"betti");
  list l=r,M;
  s=print(l,"%s"); s;
  s=print(l,"%2s"); s;
  s=print(l,"%l"); s;
tst_status(1);$
