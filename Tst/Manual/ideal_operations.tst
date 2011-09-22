LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z),dp;
  ideal I = 0,x,0,1;
  I;
  I + 0;    // simplification
  ideal J = I,0,x,x-z;;
  J;
  I * J;   //  multiplication with simplification
  I*x;
  vector V = [x,y,z];
  print(V*I);
  ideal m = maxideal(1);
  m^2;
  ideal II = I[2..4];
  II;
tst_status(1);$
