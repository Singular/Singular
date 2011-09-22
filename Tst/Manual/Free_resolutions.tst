LIB "tst.lib"; tst_init();
  ring R= 0,(u,v,x,y,z),dp;
  ideal I = ux, vx, uy, vy;
  resolution resI = mres(I,0); resI;
  // The matrix A_1 is given by
  print(matrix(resI[1]));
  // We see that the columns of A_1 generate I.
  // The matrix A_2 is given by
  print(matrix(resI[3]));
tst_status(1);$
