LIB "tst.lib"; tst_init();

ring R = (0, y, t), x, lp;
number s;

tst_InitTimer();

for (int i = 1; i<= 10; i++)
{
  s = s+t^i/(y + i*t)^i;
}

tst_ReportTimer("D");

s;
kill R;

tst_status(1); $

