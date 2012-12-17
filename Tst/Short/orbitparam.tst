LIB "tst.lib";
tst_init();

LIB"orbitparam.lib";

  ring R = 0,(x),dp;
  matrix L1[3][3] = 0,1,0, 0,0,0, 0,0,0;
  matrix L2[3][3] = 0,0,1, 0,0,0, 0,0,0;
  matrix L3[3][3] = 0,1,1, 0,0,1, 0,0,0;
  list L = L1,L2;
  matrix v[3][1] = 1,2,3;
  tangentGens(L,v);


  matrix A[4][4] = 0,0,1,0, 0,0,0,1;
  print(matrixExp(A));

  matrix A[4][4] = 0,1,0,0, 0,0,0,1;
  print(matrixExp(A));


  ring R = 0,(t(1..3)),dp;
  matrix L1[3][3] = 0,1,0, 0,0,0, 0,0,0;
  matrix L2[3][3] = 0,0,1, 0,0,0, 0,0,0;
  matrix L3[3][3] = 0,1,1, 0,0,1, 0,0,0;
  list L = L1,L2,L3;
  matrix v[3][1] = 1,2,3;
  tangentGens(L,v);


  parametrizeOrbit(L,v);

  ring R = 0,(t(1..2)),dp;
  matrix L1[4][4] = 0,1,0,0, 0,0,0,0, 0,0,0,1, 0,0,0,0;
  matrix L2[4][4] = 0,0,1,0, 0,0,0,1, 0,0,0,0, 0,0,0,0;
  list L = L1,L2;
  matrix v[4][1] = 1,2,3,4;
  parametrizeOrbit(L,v);

  ring R = 0,(t(1..3)),dp;
  matrix L1[4][4] = 0,1,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0;
  matrix L2[4][4] = 0,0,1,0, 0,0,0,0, 0,0,0,0, 0,0,0,0;
  matrix L3[4][4] = 0,0,0,0, 0,0,1,0, 0,0,0,0, 0,0,0,0;
  list L = L1,L2,L3;
  matrix v[4][1] = 1,2,3,4;
  parametrizeOrbit(L,v);


  ring R = 0,(x),dp;
  matrix L1[3][3] = 0,1,0, 0,0,0, 0,0,0;
  matrix L2[3][3] = 0,0,1, 0,0,0, 0,0,0;
  matrix L3[3][3] = 0,1,1, 0,0,1, 0,0,0;
  list L = L1,L2,L3;
  matrix v[3][1] = 1,2,3;
  maxZeros(L,v);

  ring R = 0,(x),dp;
  matrix L1[4][4] = 0,1,0,0, 0,0,0,0, 0,0,0,1, 0,0,0,0;
  matrix L2[4][4] = 0,0,1,0, 0,0,0,1, 0,0,0,0, 0,0,0,0;
  list L = L1,L2;
  matrix v[4][1] = 1,2,3,4;
  maxZeros(L,v);

  ring R = 0,(s,t),dp;
  matrix A[3][3] = 1,s,st/2, 0,1,t, 0,0,1;
  matrixLog(A);


tst_status(1);$

