LIB "tst.lib"; tst_init();
LIB "linalg.lib";

proc hilbert (int n)
{
   matrix a[n][n];
   int i, j;
   int j;
   for (i = 1; i <= n; i++)
      {
      for (j = 1; j <= n; j++)
         {
         a[i, j] = 1/(number(i) + number(j) - 1);
         }
      }
   return(a);
}

ring R = 0, x, lp;

matrix h70[70][70];
h70 = hilbert(70);

tst_InitTimer();

matrix i_h70 = inverse(h70);

tst_ReportTimer("K");

matrix check;

tst_InitTimer();

check = i_h70 * h70;

tst_ReportTimer("L");

check == matrix(freemodule(70));

kill R, hilbert;

tst_status(1); $
