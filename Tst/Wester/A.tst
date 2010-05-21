LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r = 0,x,lp;
int i;
number p;


tst_InitTimer();

for (i=1; i<=100; i++)
{
  p = factorial(1000+i)/factorial(900+i);
}

tst_ReportTimer("A");
p;
kill r, i;

tst_status(1); $

