LIB "tst.lib"; tst_init();
  ring r1=32003,(x,y,z),dp;
  ideal i=x,y,z;
  ring r2=32003,(a,b),dp;
  map f=r1,a,b,a+b;
  // maps from r1 to r2,
  // x -> a
  // y -> b
  // z -> a+b
  f(i);
  // operations like f(i[1]) or f(i*i) are not allowed
  ideal i=f(i);
  // objects in different rings may have the same name
  map g   = r2,a2,b2;
  map phi = g(f);
  // composition of map f and g
  // maps from r1 to r2,
  // x -> a2
  // y -> b2
  // z -> a2+b2
  phi(i);
tst_status(1);$
