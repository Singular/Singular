LIB "tst.lib"; tst_init();
LIB "linalg.lib";

proc hilbert (int n)
{
   matrix a[n][n];
   int j;
   for (int i = 1; i <= n; i++)
      {
      for (j = 1; j <= n; j++)
         {
         a[i, j] = 1/(number(i) + number(j) - 1);
         }
      }
   return(a);
}

ring R = 0, x, lp;

matrix h40[40][40];
h40 = hilbert(40);

tst_InitTimer();

matrix i_h40 = inverse(h40);

tst_ReportTimer("I");

matrix check;

tst_InitTimer();

check = i_h40 * h40;

tst_ReportTimer("J");

check == matrix(freemodule(40));

kill R, hilbert;

tst_status(1); $
