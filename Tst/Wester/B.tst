LIB "tst.lib"; tst_init();

ring R = 0, x, lp;
number s;

tst_InitTimer();

for (int i = 1; i <= 1000; i++)
{
  s = s + 1/number(i);
}

tst_ReportTimer("B");

s;
kill R;

tst_status(1); $
