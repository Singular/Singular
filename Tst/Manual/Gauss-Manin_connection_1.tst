LIB "tst.lib"; tst_init();
  LIB "gmssing.lib";
  ring R=0,(x,y),ds;
  poly f=x5+x2y2+y5;
  list l=vfilt(f);
  print(l[1]); // spectral numbers
  print(l[2]); // corresponding multiplicities
  print(l[3]); // vector space of i-th graded part
  print(l[4]); // monomial vector space basis of H''/s*H''
  print(l[5]); // standard basis of Jacobian ideal
tst_status(1);$
