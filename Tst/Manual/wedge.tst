LIB "tst.lib"; tst_init();
  ring r;
  matrix m[2][3]=x,y,y,z,z,x;
  print(m);
  print(wedge(m,2));
tst_status(1);$
