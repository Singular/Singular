LIB "tst.lib"; tst_init();

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

int n = 70;

ring R = 0, x, lp;
matrix hn[n][n];
hn = hilbert(n);
module mn = module(hn);

tst_InitTimer();

poly dh70 = det(mn);

tst_ReportTimer("H");

dh70;

kill R, n, hilbert;

tst_status(1); $



