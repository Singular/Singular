LIB "tst.lib"; tst_init();

ring R = (0, y, t), x, lp;
number s;
proc abs (int n)
{
   if (n >= 0)
      {
      return(n);
      }
   else
      {
      return(-n);
      }
}

tst_InitTimer();

for (int i = 1; i<= 10; i++)
{
  s = s+t^i/(y + abs(5-i)*t)^i;
}

tst_ReportTimer("E");

s;

kill R;

tst_status(1); $
