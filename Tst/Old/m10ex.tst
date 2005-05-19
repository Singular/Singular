  LIB "homolog.lib";
  ring R=0,(x,y),ds;
  ideal i=x2-y3;
  qring q = std(i);          // defines the quotient ring Loc_k[x,y]/(x2-y3)
  ideal m = maxideal(1);
  module T1K = Ext(1,m,m);   // computes Ext^1(R/m,R/m)
  print(T1K);
  printlevel=2;              // gives more explanation
  module T2K=Ext(2,m,m);     // computes Ext^2(R/m,R/m)
  print(std(T2K));
  printlevel=0;
  module E = Ext(1,syz(m),syz(m));
  print(std(E));               
  //We see from the matrices that T2K and E are isomorphic
  //as it should be; but both are differently presented
  //-------------------------------------------
  ring R=0,(x,y,z),dp;
  ideal  i = x2y,y2z,z3x;
  module E = Ext_R(2,i);
  print(E);
  // if a 3-rd argument is given (of type int)
  // a list of Ext^k(R/i,R), a SB of Ext^k(R/i,R) and a vector space basis
  // is returned:
  list LE = Ext_R(3,i,0);
  LE;
  print(LE[2]);
  print(kbase(LE[2]));
  killall();
LIB "tst.lib";tst_status(1);$
