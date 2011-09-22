LIB "tst.lib"; tst_init();
  intmat m[2][4] = 1,0,2,4,0,1,-1,0,3,2,1,-2;
  m;
  m[2,3];          // entry at row 2, col 3
  size(m);         // number of entries
  intvec v = 1,0,-1,2;
  m * v;
  typeof(_);
  intmat m1[4][3] = 0,1,2,3,v,1;
  intmat m2 = m * m1;
  m2;             //  2 x 3 intmat
  m2*10;           // multiply each entry of m with 10;
  -m2;
  m2 % 2;
  m2 div 2;
  m2[2,1];          // entry at row 2, col 1
  m1[2..3,2..3];   // submatrix
  m2[nrows(m2),ncols(m2)];      // the last entry of intmat m2
tst_status(1);$
