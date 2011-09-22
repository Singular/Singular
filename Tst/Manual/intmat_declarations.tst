LIB "tst.lib"; tst_init();
  intmat im[3][5]=1,3,5,7,8,9,10,11,12,13;
  im;
  im[3,2];
  intmat m[2][3] = im[1..2,3..5];  // defines a submatrix
  m;
tst_status(1);$
