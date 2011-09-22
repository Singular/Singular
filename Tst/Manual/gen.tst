LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),(c,dp);
  gen(3);
  vector v=gen(5);
  poly f=xyz;
  v=v+f*gen(4); v;
  ring rr=32003,(x,y,z),dp;
  fetch(r,v);
tst_status(1);$
