LIB "tst.lib"; tst_init();
  ring r=0,(x),dp;
  matrix A[4][4]=1,1,1,0,1,2,3,1,1,3,5,2,1,4,7,3;
  matrix b[4][1]=2,5,8,11;
  list L=ludecomp(A);
  list Q=lusolve(L[1],L[2],L[3],b);
  if (Q[1] == 1)
  {
    "one solution:";
    print(Q[2]);
    "check whether result is correct (iff next is zero vector):";
    print(A*Q[2]-b);
    if ((nrows(Q[3])==1) and (ncols(Q[3])==1) and (Q[3][1,1]==0))
    { "printed solution is the only solution to given linear system" }
    else
    {
      "homogeneous solution space is spanned by columns of:";
      print(Q[3]);
    }
  }
tst_status(1);$
