  LIB "matrix.lib"; LIB "sing.lib";
  int n = 4;
  int m = 3;
  int N = n*(n+1) div 2;       // will become number of variables
  ring R = 32003,x(1..N),dp;
  matrix X = symmat(n);        // proc from matrix.lib
                               // creates the symmetric generic nxn matrix
  print(X);
  ideal J = minor(X,m);
  J=std(J);
  // Kaehler differentials D_k(R)
  // of R=k[x1..xn]/J:
  module D = J*freemodule(N)+transpose(jacob(J));
  ncols(D);
  nrows(D);
  //
  // Note: D is a submodule with 110 generators of a free module
  // of rank 10 over a polynomial ring in 10 variables.
  // Compute a full resolution of D with sres.
  // This takes about 17 sec on a Mac PB 520c and 2 sec an a HP 735
  module sD = std(D);
  list Dres = sres(sD,0);                // the full resolution
  intmat B = betti(Dres);
  print(B,"betti");
  N-ncols(B)+1;                          // the desired depth
  killall();
LIB "tst.lib";tst_status(1);$
