LIB "tst.lib"; tst_init();
  LIB "homolog.lib";
  ring R=0,(x,y),ds;
  ideal i=x2-y3;
  qring q = std(i);      // defines the quotient ring k[x,y]_m/(x2-y3)
  ideal m = maxideal(1);
  module T1K = Ext(1,m,m);  // computes Ext^1(R/m,R/m)
  print(T1K);
  printlevel=2;             // gives more explanation
  module T2K=Ext(2,m,m);    // computes Ext^2(R/m,R/m)
  print(std(T2K));
  printlevel=0;
  module E = Ext(1,syz(m),syz(m));
  print(std(E));
  //The matrices which we have just computed are presentation matrices
  //of the modules T2K and E. Hence we may ignore those columns
  //containing 1 as an entry and see that T2K and E are isomorphic
  //as expected, but differently presented.
  //-------------------------------------------
  ring S=0,(x,y,z),dp;
  ideal  i = x2y,y2z,z3x;
  module E = Ext_R(2,i);
  print(E);
  // if a 3-rd argument of type int is given,
  // a list of Ext^k(R/i,R), a SB of Ext^k(R/i,R) and a vector space basis
  // is returned:
  list LE = Ext_R(3,i,0);
  LE;
  print(LE[2]);
  print(kbase(LE[2]));
tst_status(1);$
