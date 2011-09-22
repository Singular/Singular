LIB "tst.lib"; tst_init();
  ring s = 0, x, dp;
  matrix A[100][100];
  int i; int j; int r;
  for (i = 1; i <= 100; i++)
  {
    for (j = 1; j <= 100; j++)
    {
      A[i, j] = random(-10, 10);
    }
  }
  r = rank(A); r;
tst_status(1);$
