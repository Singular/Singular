LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  poly f=(3x+6y)^5;
  f/5;
  cleardenom(f/5);
  vector w= [4x2+20,6x+2,0,8];   // application to a vector
  print(cleardenom(w));
tst_status(1);$
