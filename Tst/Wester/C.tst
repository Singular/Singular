LIB "tst.lib"; tst_init();
ring R = 0, a, lp;
number x = 13*17*31;
number y = 13*19*29;
number q;

tst_InitTimer();

for (int i = 1; i <= 200; i++)
{
  q = gcd(x^(300 + (i mod 181)), y^(200 + (i mod 183)));
}

tst_ReportTimer("C");

q;
kill R;

tst_status(1); $
